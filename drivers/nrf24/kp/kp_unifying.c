// Copyright 2017 jem@seethis.link
// Licensed under the MIT license (http://opensource.org/licenses/MIT)

#include "kp_unifying.h"

#include <string.h>
#include <stdlib.h>

#include "kp_hardware.h"
#include "kp_hidpp20.h"
#include "kp_led.h"
#include "kp_nrf24.h"
#include "kp_rf.h"
#include "kp_settings.h"
#include "kp_timer.h"
#include "kp_usb_commands.h"
#include "kp_mouse.h"

#include "hid_reports/mouse_report.h"

// NOTE: For uniyfing devices to work, we need to dedicated at least 2 pipes
// for the device. These two pipes have the form:
// X0 = XX:XX:XX:XX:00
// X1 = XX:XX:XX:XX:DD
// where the 4 upper most bytes match. When a unifying device starts, it tries
// to connect to the dongle by pinging the dongles address X0. If it gets a
// response it, will then start sending messages on its own pipe X1. If it
// disconnects, it will go back to pinging X0.
#define UNIFYING_ADDR_WIDTH 5

/// Address for the initial unifying pairing request.
///
/// Note: Addresses are stored in little endian format, because writes to
/// the address registers are written LSB bit first.
/// BB:0A:DC:A5:75
static XRAM uint8_t unifying_pairing_addr[5] = {0x75, 0xA5, 0xDC, 0x0A, 0xBB};
static XRAM uint8_t last_pairing_step = UNIFYING_PAIR_DISABLED;
static XRAM uint8_t pairing_target_addr[UNIFYING_ADDR_WIDTH];
static XRAM uint8_t tmp_buffer[32];
static XRAM uint16_t pairing_timeout;
static XRAM uint16_t packet_timeout;

// TODO: move to init function
static XRAM uint8_t s_extra_button_last_state = 0;

// Seems to be a bug in the mouse firmware. It seems to send an extra report
// after some requests (that seems to correspond to a left click, might
// be specific to m560???).
static XRAM uint8_t s_ignore_buggy_report = 0;

// HID++ communication state
static XRAM uint8_t s_index = 0;

void unifying_set_pairing_address(const XRAM uint8_t *target_addr, uint8_t addr_lsb);

/// Two's complement checksum used by unifying packets
uint8_t unifying_calc_checksum(const XRAM uint8_t *data, const uint8_t len) {
    uint8_t i;
    uint8_t result = 0;

    for (i=0; i < len; ++i) {
        result += data[i];
    }

    return -result;
}

#if USE_NRF52_ESB
static nrf_esb_payload_t        tx_payload = NRF_ESB_CREATE_PAYLOAD(
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
);
#endif

static void write_ack_payload(
    const XRAM uint8_t *data,
    uint8_t len,
    uint8_t pipe
) {
#if USE_NRF52_ESB && USE_NRF24
    if (g_rf_settings.hw_type == RF_HW_NRF24L01) {
#else
    {
#endif
    #if USE_NRF24
        if (nrf24_read_status() & STATUS_TX_FULL_bm) {
            nrf24_flush_tx();
        }

        rf_disable_receive_irq();
        nrf24_write_ack_payload(data, len, pipe);
        rf_enable_receive_irq();
    #endif
#if USE_NRF52_ESB && USE_NRF24
    } else if (
        g_rf_settings.hw_type == RF_HW_NRF52_ESB
        || g_rf_settings.hw_type == RF_HW_BLE_AND_ESB
    ) {
#endif
    #if USE_NRF52_ESB
        tx_payload.pipe = pipe;
        tx_payload.length = len;
        memcpy(tx_payload.data, data, len);

        rf_esb_write_ack_payload(&tx_payload);
    #endif
    }
}

/// Send a packet to the device using ACK payloads.
void unifying_send_packet(const XRAM uint8_t *data, uint8_t size) {
    write_ack_payload(data, size, UNIFYING_RF_PIPE_MOUSE);
}

void unifying_read_packet(const XRAM uint8_t *nrf_packet, uint8_t width) {
    const uint8_t nrf_packet_type = nrf_packet[1];

#if 0
    // For debugging print unifying packets
    if (nrf_packet_type == 0x40 || nrf_packet_type == 0x4F) {
    // } else if (nrf_packet_type == 0xC2) {
    } else {
        usb_print(nrf_packet, width);
    }
#endif

    switch (nrf_packet_type) {
        case UNIFYING_FRAME_MOUSE: {
            uint16_t x = ((nrf_packet[5] & 0x0f) << 8) | nrf_packet[4];
            uint16_t y = (uint16_t)((nrf_packet[6]) << 4) | (uint16_t)((nrf_packet[5] & 0xf0) >> 4);

            if (s_ignore_buggy_report) {
                s_ignore_buggy_report = 0;
                break;
            }

#if USE_MOUSE_GESTURE
            // On left mouse click, send a HID++ packet.
            if ((g_mouse_state.buttons_1 & 0x01) == 0 && (nrf_packet[2] & 0x01)) {
                // TODO: integrate this functionality so it happens automatically
                // on device power on and receiving packets from the mouse
                unifying_hidpp20_long_t XRAM* report = (unifying_hidpp20_long_t*)tmp_buffer;
                uint8_t size = sizeof(unifying_hidpp20_long_t);
                memset(report, 0, size);
                report->id = 0x00;
                report->frame_type = UNIFYING_FRAME_HIDPP_LONG;
                report->device_index = 0x01;

#if 0
                // Discover a feature index using IRoot_GetFeature()
                {
                    uint16_t feature = HIDPP20_REPROG_CONTROLS_V4;

                    report->feature_index = HIDPP20_ROOT_INDEX;
                    report->function_id = HIDPP20_IRoot_GetFeature;
                    report->software_id = 1;
                    report->parameters[0] = (feature >> 8) & 0xff; // big endian
                    report->parameters[1] = (feature >> 0) & 0xff;
                }
#endif

#if 1
                // HIDPP20 HIDPP20_REPROG_CONTROLS_V4
                {
                    uint8_t cid_tab[3] = {
                        HIDPP20_CID_SCROLL_LEFT,
                        HIDPP20_CID_SCROLL_RIGHT,
                        HIDPP20_CID_GESTURE,
                    };
                    uint8_t cid = cid_tab[s_index%3];

                    uint8_t flags = (
                        ((1<<0) * 1) | // divert
                        ((1<<1) * 1) | // dvalid
                        ((1<<2) * 1) | // persist
                        ((1<<3) * 1) | // pvalid
                        ((1<<4) * 0) | // rawXY
                        ((1<<5) * 0)   // rvalid
                    );
                    uint8_t remap = 0; // 0-> don't change

                    report->feature_index = 0x0b;

                    // report->function_id = HIDPP20_SpecialKeysMSEButtons_GetCount;
                    // report->function_id = HIDPP20_SpecialKeysMSEButtons_GetCidInfo;
                    // report->function_id = HIDPP20_SpecialKeysMSEButtons_GetCidReporting;
                    report->function_id = HIDPP20_SpecialKeysMSEButtons_SetCidReporting;
                    report->software_id = KEYPLUS_HIDPP_SOFTWARE_ID;

                    report->parameters[0] = (cid>>8) & 0xff;
                    report->parameters[1] = (cid>>0) & 0xff;

                    report->parameters[2] = flags;

                    report->parameters[3] = (remap>>8) & 0xff;
                    report->parameters[4] = (remap>>0) & 0xff;
                }
#endif

                report->checksum = unifying_calc_checksum((uint8_t *XRAM)report, size-1);

                s_index++;

                unifying_send_packet((uint8_t *XRAM)report, size);
            }
#endif


            g_mouse_state.buttons_1 = nrf_packet[2] | s_extra_button_last_state;
            g_mouse_state.buttons_2 = nrf_packet[3];
            g_mouse_state.x = sign_extend_12(x);
            g_mouse_state.y = sign_extend_12(y);
            g_mouse_state.wheel_y = nrf_packet[7];
            g_mouse_state.wheel_x = nrf_packet[8];

            g_mouse_activity = UNIFYING_MOUSE_ACTIVE;
        } break;

        case UNIFYING_FRAME_EXTRA_BUTTON: { // 0xD1


            // NOTE: All packets I've recored so far have the bytes
            // 63 0A following D1.  This may be a constant or contain some
            // other information.
            // Example packet: 00 D1 63 0A ?? ?? XX ??
            //
            // The XX value is a constant that is different depending on
            // which key is pressed. It's not a bitmask. If multiple buttons
            // are pressed at once, it seems the mouse incorrectly reports
            // which button was pressed in some cases.
            //
            // Zero or more packets with type 0xC1 seem to be sent after this
            // which seem to contain some other information.
            switch (nrf_packet[6]) {
                case UNIFYING_EXTRA_MIDDLE: {    // AF: middle mouse button
                    s_extra_button_last_state |= UNIFYING_MSB_MIDDLE;
                    s_ignore_buggy_report = 1;
                } break;
                case UNIFYING_EXTRA_SIDE_UP: {   // B0: side button 1
                    s_extra_button_last_state |= UNIFYING_MSB_EXTRA_1;
                } break;
                case UNIFYING_EXTRA_SIDE_DOWN: { // AE: side button 2
                    s_extra_button_last_state |= UNIFYING_MSB_EXTRA_2;
                } break;

                // Clear extra button state
                //
                // When we receive this button, we don't know which button
                // was released. Therefore we must release all buttons to be
                // safe.
                case 0x00: {
                    g_mouse_state.buttons_1 &= ~s_extra_button_last_state;
                    s_extra_button_last_state = 0;
                } break;
            }

            g_mouse_state.buttons_1 |= s_extra_button_last_state;

            g_mouse_activity = UNIFYING_MOUSE_EXTRA_BUTTON;
        } break;

        case UNIFYING_FRAME_HIDPP_LONG_RESP: {
            uint8_t i;
            unifying_hidpp20_diverted_buttons_t *report = (void*)nrf_packet;

            // Forward the packet to the USB data stream
            if (report->software_id != KEYPLUS_HIDPP_SOFTWARE_ID) {
                queue_vendor_in_packet(
                    CMD_UNIFYING_RECV_LONG,
                    nrf_packet + 2,
                    20 - 1,
                    STATIC_LENGTH_CMD
                );
            }

            // TODO: make this generic, currently only works for m720
            if (report->feature_index != 0x0b && report->function_id != 0) {
                break;
            }

            s_extra_button_last_state = 0;

            // Read the list of buttons that are currently down.
            for (i = 0; i < 4; ++i) {
                const uint8_t cid = ntohs(report->control_id_list[i]);
                switch (cid) {
                    case HIDPP20_CID_SCROLL_LEFT: {
                        s_extra_button_last_state |= UNIFYING_MSB_EXTRA_1;
                    } break;

                    case HIDPP20_CID_SCROLL_RIGHT: {
                        s_extra_button_last_state |= UNIFYING_MSB_EXTRA_2;
                    } break;

                    case HIDPP20_CID_GESTURE: {
                        s_extra_button_last_state |= UNIFYING_MSB_EXTRA_3;
                    } break;

                    case HIDPP20_CID_NONE: {
                    } break;
                }
            }

            // When reporting in this mode, we control these bits directly
            g_mouse_state.buttons_1 &= ~(0xE0);
            g_mouse_state.buttons_1 |= s_extra_button_last_state;

            g_mouse_activity = UNIFYING_MOUSE_EXTRA_BUTTON;
        } break;
#if 0
        // Unecrypted HID packet
        case UNIFYING_FRAME_UNENCRYPTED_HID: { // 0xC1
            // Not sure what data this carries
        }

        case UNIFYING_FRAME_KEEP_ALIVE_2: { // 0x4F
            // g_mouse_activity = true;
        } break; */

        case UNIFYING_FRAME_KEEP_ALIVE_1: { // 0x40 */
            mouse_active = false;
            if (mouse_active) {
            g_mouse_activity = true;
            }
            memset(&g_mouse_state, 0, sizeof(g_mouse_state));
        }
#endif
    }
}

/*********************************************************************
 *                         unifying pairing                          *
 *********************************************************************/

// TODO: need to add nrf_esb support for unifying pairing

/// Set appropriate nrf24 radio settings and address for unifying pairing
void unifying_begin_pairing(void) {
    uint8_t config;

#if USE_NRF52_ESB
    NRF_LOG_INFO("Unifying begin pairing");
    if (g_rf_settings.hw_type == RF_HW_NRF52_ESB) {
        ret_code_t err_code;
        err_code = nrf_esb_stop_rx();
        APP_ERROR_CHECK(err_code);

        nrf52_esb_init_unifying_pair(
            unifying_pairing_addr,
            g_rf_settings.pipe_addr_1,
            g_rf_settings.pipe_addr_4
        );

        packet_buffer_clear();

        memcpy(pairing_target_addr, g_rf_settings.pipe_addr_1, UNIFYING_ADDR_WIDTH);
        pairing_target_addr[0] = g_rf_settings.pipe_addr_4;

        led_testing_set(1, 1);
    } else {
#else
    {
#endif
        disable_interrupts();

        rf_init_receive(); // set rf settings for receive mode

        unifying_set_pairing_address(g_rf_settings.pipe_addr_1, g_rf_settings.pipe_addr_4);

        // TODO: probably add interrupt based mode later, but for now just mask the
        // IRQ for the NRF
        config = nrf24_read_reg(CONFIG);
        config = config | MASK_ALL_IRQ_bm;
        nrf24_write_reg(CONFIG, config);

        enable_interrupts();
    }

    last_pairing_step = 0;
    pairing_timeout = timer_read16_ms() + UNIFYING_PAIRING_TIMEOUT;
}

void unifying_end_pairing(void) {
    last_pairing_step = UNIFYING_PAIR_DISABLED;
    rf_init_receive();
}

bit_t unifying_is_pairing_active(void) {
    return last_pairing_step != UNIFYING_PAIR_DISABLED;
}

#if USE_NRF24
// Note: Assumes all other relevant settings are in place
void unifying_set_pairing_address(const XRAM uint8_t *target_addr, uint8_t addr_lsb) {
    // Use first three pipes
    // P0 = BB:0A:DC:A5:75
    // P1 = TARGET_ADDRESS
    // P2 = TARGET_ADDRESS with LSB set to 0
    // P3..P5 = disabled
    nrf24_write_addr(TX_ADDR, unifying_pairing_addr, UNIFYING_ADDR_WIDTH);
    nrf24_write_addr(RX_ADDR_P0, unifying_pairing_addr, UNIFYING_ADDR_WIDTH);

    memcpy(pairing_target_addr, target_addr, UNIFYING_ADDR_WIDTH);
    pairing_target_addr[0] = addr_lsb;

    // set MSB of addr, assumes addresses are stored in little endian
    nrf24_write_addr(RX_ADDR_P1, pairing_target_addr, UNIFYING_ADDR_WIDTH);
    nrf24_write_reg(RX_ADDR_P2, 0);

    nrf24_write_reg(EN_RXADDR, 0b0111);
}
#endif

bit_t handle_pairing(uint8_t pipe_num) {
    unifying_packet_t *packet = (unifying_packet_t*)tmp_buffer;
    uint8_t width;
#if USE_NRF52_ESB
    if (g_rf_settings.hw_type == RF_HW_NRF52_ESB) {
        pipe_num = packet_buffer_get();
        width = packet_buffer_get();
        if (width > UNIFYING_MAX_PACKET_SIZE || width > packet_buffer_len())  {
            packet_buffer_clear();
            return false;
        }

        // read out the packet payload into the buffer
        packet_buffer_take(tmp_buffer, width);
    } else
#endif
    {
        width = nrf24_read_rx_payload_width();
        nrf24_read_rx_payload(tmp_buffer, width);
    }

    if (pipe_num > 1) {
        return false;
    }

    if ( unifying_calc_checksum(tmp_buffer, width) ) {
        return false;
    }

    if ( !(packet->header.type == UNIFYING_FRAME_PAIRING ||
           packet->header.type == 0x0f ||
           last_pairing_step >= 3)) {
        return false;
    }

    last_pairing_step++;

    tmp_buffer[width] = last_pairing_step;
    usb_print(tmp_buffer, width+1);

    packet_timeout = timer_read16_ms() + UNIFYING_PAIRING_PACKET_TIMEOUT;

    if (packet->header.step == 1 && last_pairing_step == 1) {
        //req_1->step = 1;
        packet->req_1.frame_type = 0x1f;

        packet->req_1.addr[0] = pairing_target_addr[4];
        packet->req_1.addr[1] = pairing_target_addr[3];
        packet->req_1.addr[2] = pairing_target_addr[2];
        packet->req_1.addr[3] = pairing_target_addr[1];
        packet->req_1.addr[4] = pairing_target_addr[0];

        packet->req_1.checksum = unifying_calc_checksum(tmp_buffer, sizeof(unifying_req_1_t)-1);
        unifying_send_packet(tmp_buffer, sizeof(unifying_req_1_t));
    } else if (packet->header.step == 2 && last_pairing_step == 2) {
        //req_2->step = 2;
        packet->req_2.frame_type = 0x1f;
        packet->req_2.checksum = unifying_calc_checksum(tmp_buffer, sizeof(unifying_req_2_t)-1);
        unifying_send_packet(tmp_buffer, sizeof(unifying_req_2_t));
    } else if (packet->header.step == 3 && last_pairing_step == 3) {
        //resp_3->step = 3;
        packet->resp_3.frame_type = 0x0f;
        packet->resp_3.checksum = unifying_calc_checksum(tmp_buffer, sizeof(unifying_resp_3_t)-1);
        unifying_send_packet(tmp_buffer, sizeof(unifying_resp_3_t));
    } else if (last_pairing_step >= 4) {
        // Successfully paired with the device, and received a packet from it
        // after it has paired
        return true;
    }
    return false;
}

// TODO: add interrupt based mode?
void unifying_pairing_poll(void) {
    uint8_t pairing_complete = false;

#if USE_NRF52_ESB
    if (g_rf_settings.hw_type == RF_HW_NRF52_ESB) {
        NVIC_DisableIRQ(ESB_EVT_IRQ);
        while (packet_buffer_has_data()) {
            NRF_LOG_INFO("Unifying handling pairing packet");
            pairing_complete |= handle_pairing(0);
            led_testing_set(1, 0);
            led_testing_set(2, 0);
            led_testing_toggle(3);
        }
        NVIC_EnableIRQ(ESB_EVT_IRQ);

        if (pairing_complete) {
            NRF_LOG_INFO("Unifying Pairing complete");
        }

    } else
#endif
    {
        uint8_t pipe_num = nrf24_get_rx_pipe_num();

        while (pipe_num != 0b111) {
            pairing_complete |= handle_pairing(pipe_num);

            led_testing_toggle(0);
            pipe_num = nrf24_get_rx_pipe_num();
        }
    }

    if (has_passed_time16(timer_read16_ms(), packet_timeout)) {
        last_pairing_step = 0;
    }

    if (
        pairing_complete ||
        has_passed_time16(timer_read16_ms(), pairing_timeout)
    ) {
        reset_mcu();
    }
}
