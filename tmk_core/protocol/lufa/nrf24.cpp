#include "nrf24.h"
#include "rf24./h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <alloca.h>
#include "debug.h"
#include "timer.h"
#include "action_util.h"
#include "ringbuffer.hpp"
#include <string.h>
#include "spi_master.h"
#include "wait.h"
#include "analog.h"

// These are the pin assignments for the XD60 rev3 PCB.
// You need to unsolder the 2 10ohm pullup resistors next
// to the I2C header on the back of the PCB and wire your
// NRF24L01+ module using SDA (F7) for CE and SCL (F4) for CS
// You may define them to something else in your config.h
// if yours is wired up differently.
#ifndef NRF24_CE
#    define NRF24_CE F7
#endif

#ifndef NRF24_CS
#    define NRF24_CS F4
#endif

// IRQ is not implemented yet.
#ifndef NRF24_IRQ
#    define NRF24_IRQ E6
#endif

// RF modules support 10 Mhz SPI bus speed
#define NRF24_SPI_CLOCK_SPEED 10000000  // SCK frequency

#define SCK_DIVISOR (F_CPU / NRF24_SPI_CLOCK_SPEED)

enum unifying_params {
    pairingMacAddress = 0xBB0ADCA575LL,
    payloadSize       = 22,
    channel           = 5,
    BleUartRx     = 0x0a02,
};

static bool nrf24_write_register(uint8_t reg, uint8_t value) {

}

static void nrf24_end_transaction() {
    writePinLow(NRF24_CS);
}

static void nrf24_begin_transaction() {
    writePinLow(NRF24_CS);
}


static bool nrf24_init(void) {
    state.initialized  = false;
    state.configured   = false;
    state.is_connected = false;

    // Initialize pins
    setPinOutput(NRF24_CE);
    setPinOutput(NRF24_CS);
    setPinInput(NRF24_IRQ);

    spi_init();

    // Perform a hardware reset
    writePinLow(NRF24_CE);
    writePinHigh(NRF24_CS);

    // Must allow the radio time to settle else configuration bits will not necessarily stick.
    // This is actually only required following power up but some settling time also appears to
    // be required after resets too. For full coverage, we'll always assume the worst.
    // Enabling 16b CRC is by far the most obvious case if the wrong timing is used - or skipped.
    // Technically we require 4.5ms + 14us as a worst case. We'll just call it 5ms for good measure.
    // WARNING: Delay is based on P-variant whereby non-P *may* require different timing.
    wait_ms(5);



    state.initialized = true;
    return state.initialized;
}

static inline uint8_t min(uint8_t a, uint8_t b) { return a < b ? a : b; }

static bool read_response(char *resp, uint16_t resplen, bool verbose) {
    char *dest = resp;
    char *end  = dest + resplen;

    while (true) {
        struct sdep_msg msg;

        if (!sdep_recv_pkt(&msg, 2 * SdepTimeout)) {
            dprint("sdep_recv_pkt failed\n");
            return false;
        }

        if (msg.type != SdepResponse) {
            *resp = 0;
            return false;
        }

        uint8_t len = min(msg.len, end - dest);
        if (len > 0) {
            memcpy(dest, msg.payload, len);
            dest += len;
        }

        if (!msg.more) {
            // No more data is expected!
            break;
        }
    }

    // Ensure the response is NUL terminated
    *dest = 0;

    // "Parse" the result text; we want to snip off the trailing OK or ERROR line
    // Rewind past the possible trailing CRLF so that we can strip it
    --dest;
    while (dest > resp && (dest[0] == '\n' || dest[0] == '\r')) {
        *dest = 0;
        --dest;
    }

    // Look back for start of preceeding line
    char *last_line = strrchr(resp, '\n');
    if (last_line) {
        ++last_line;
    } else {
        last_line = resp;
    }

    bool              success       = false;
    static const char kOK[] PROGMEM = "OK";

    success = !strcmp_P(last_line, kOK);

    if (verbose || !success) {
        dprintf("result: %s\n", resp);
    }
    return success;
}

static bool at_command(const char *cmd, char *resp, uint16_t resplen, bool verbose, uint16_t timeout) {
    const char *    end = cmd + strlen(cmd);
    struct sdep_msg msg;

    if (verbose) {
        dprintf("ble send: %s\n", cmd);
    }

    if (resp) {
        // They want to decode the response, so we need to flush and wait
        // for all pending I/O to finish before we start this one, so
        // that we don't confuse the results
        resp_buf_wait(cmd);
        *resp = 0;
    }

    // Fragment the command into a series of SDEP packets
    while (end - cmd > SdepMaxPayload) {
        sdep_build_pkt(&msg, BleAtWrapper, (uint8_t *)cmd, SdepMaxPayload, true);
        if (!sdep_send_pkt(&msg, timeout)) {
            return false;
        }
        cmd += SdepMaxPayload;
    }

    sdep_build_pkt(&msg, BleAtWrapper, (uint8_t *)cmd, end - cmd, false);
    if (!sdep_send_pkt(&msg, timeout)) {
        return false;
    }

    if (resp == NULL) {
        auto now = timer_read();
        while (!resp_buf.enqueue(now)) {
            resp_buf_read_one(false);
        }
        auto later = timer_read();
        if (TIMER_DIFF_16(later, now) > 0) {
            dprintf("waited %dms for resp_buf\n", TIMER_DIFF_16(later, now));
        }
        return true;
    }

    return read_response(resp, resplen, verbose);
}

bool at_command_P(const char *cmd, char *resp, uint16_t resplen, bool verbose) {
    auto cmdbuf = (char *)alloca(strlen_P(cmd) + 1);
    strcpy_P(cmdbuf, cmd);
    return at_command(cmdbuf, resp, resplen, verbose);
}

bool nrf24_is_connected(void) { return state.is_connected; }

bool nrf24_enable_keyboard(void) {
    char resbuf[128];

    if (!state.initialized && !ble_init()) {
        return false;
    }

    state.configured = false;

    // Disable command echo
    static const char kEcho[] PROGMEM = "ATE=0";
    // Make the advertised name match the keyboard
    static const char kGapDevName[] PROGMEM = "AT+GAPDEVNAME=" STR(PRODUCT);
    // Turn on keyboard support
    static const char kHidEnOn[] PROGMEM = "AT+BLEHIDEN=1";

    // Adjust intervals to improve latency.  This causes the "central"
    // system (computer/tablet) to poll us every 10-30 ms.  We can't
    // set a smaller value than 10ms, and 30ms seems to be the natural
    // processing time on my macbook.  Keeping it constrained to that
    // feels reasonable to type to.
    static const char kGapIntervals[] PROGMEM = "AT+GAPINTERVALS=10,30,,";

    // Reset the device so that it picks up the above changes
    static const char kATZ[] PROGMEM = "ATZ";

    // Turn down the power level a bit
    static const char  kPower[] PROGMEM             = "AT+BLEPOWERLEVEL=-12";
    static PGM_P const configure_commands[] PROGMEM = {
        kEcho, kGapIntervals, kGapDevName, kHidEnOn, kPower, kATZ,
    };

    uint8_t i;
    for (i = 0; i < sizeof(configure_commands) / sizeof(configure_commands[0]); ++i) {
        PGM_P cmd;
        memcpy_P(&cmd, configure_commands + i, sizeof(cmd));

        if (!at_command_P(cmd, resbuf, sizeof(resbuf))) {
            dprintf("failed BLE command: %S: %s\n", cmd, resbuf);
            goto fail;
        }
    }

    state.configured = true;

    // Check connection status in a little while; allow the ATZ time
    // to kick in.
    state.last_connection_update = timer_read();
fail:
    return state.configured;
}

static void set_connected(bool connected) {
    if (connected != state.is_connected) {
        if (connected) {
            print("****** BLE CONNECT!!!!\n");
        } else {
            print("****** BLE DISCONNECT!!!!\n");
        }
        state.is_connected = connected;

        // TODO: if modifiers are down on the USB interface and
        // we cut over to BLE or vice versa, they will remain stuck.
        // This feels like a good point to do something like clearing
        // the keyboard and/or generating a fake all keys up message.
        // However, I've noticed that it takes a couple of seconds
        // for macOS to to start recognizing key presses after BLE
        // is in the connected state, so I worry that doing that
        // here may not be good enough.
    }
}

void nrf24_task(void) {
    char resbuf[48];

    if (!state.configured && !nrf24_enable_keyboard()) {
        return;
    }
    resp_buf_read_one(true);
    send_buf_send_one(SdepShortTimeout);

    if (resp_buf.empty() && (state.event_flags & UsingEvents) && readPin(AdafruitBleIRQPin)) {
        // Must be an event update
        if (at_command_P(PSTR("AT+EVENTSTATUS"), resbuf, sizeof(resbuf))) {
            uint32_t mask = strtoul(resbuf, NULL, 16);

            if (mask & BleSystemConnected) {
                set_connected(true);
            } else if (mask & BleSystemDisconnected) {
                set_connected(false);
            }
        }
    }

    if (timer_elapsed(state.last_connection_update) > ConnectionUpdateInterval) {
        bool shouldPoll = true;
        if (!(state.event_flags & ProbedEvents)) {
            // Request notifications about connection status changes.
            // This only works in SPIFRIEND firmware > 0.6.7, which is why
            // we check for this conditionally here.
            // Note that at the time of writing, HID reports only work correctly
            // with Apple products on firmware version 0.6.7!
            // https://forums.adafruit.com/viewtopic.php?f=8&t=104052
            if (at_command_P(PSTR("AT+EVENTENABLE=0x1"), resbuf, sizeof(resbuf))) {
                at_command_P(PSTR("AT+EVENTENABLE=0x2"), resbuf, sizeof(resbuf));
                state.event_flags |= UsingEvents;
            }
            state.event_flags |= ProbedEvents;

            // leave shouldPoll == true so that we check at least once
            // before relying solely on events
        } else {
            shouldPoll = false;
        }

        static const char kGetConn[] PROGMEM = "AT+GAPGETCONN";
        state.last_connection_update         = timer_read();

        if (at_command_P(kGetConn, resbuf, sizeof(resbuf))) {
            set_connected(atoi(resbuf));
        }
    }

#ifdef SAMPLE_BATTERY
    if (timer_elapsed(state.last_battery_update) > BatteryUpdateInterval && resp_buf.empty()) {
        state.last_battery_update = timer_read();

        state.vbat = analogRead(BATTERY_LEVEL_PIN);
    }
#endif
}

static bool process_queue_item(struct queue_item *item, uint16_t timeout) {
    char cmdbuf[48];
    char fmtbuf[64];

    // Arrange to re-check connection after keys have settled
    state.last_connection_update = timer_read();

#if 1
    if (TIMER_DIFF_16(state.last_connection_update, item->added) > 0) {
        dprintf("send latency %dms\n", TIMER_DIFF_16(state.last_connection_update, item->added));
    }
#endif

    switch (item->queue_type) {
        case QTKeyReport:
            strcpy_P(fmtbuf, PSTR("AT+BLEKEYBOARDCODE=%02x-00-%02x-%02x-%02x-%02x-%02x-%02x"));
            snprintf(cmdbuf, sizeof(cmdbuf), fmtbuf, item->key.modifier, item->key.keys[0], item->key.keys[1], item->key.keys[2], item->key.keys[3], item->key.keys[4], item->key.keys[5]);
            return at_command(cmdbuf, NULL, 0, true, timeout);

        case QTConsumer:
            strcpy_P(fmtbuf, PSTR("AT+BLEHIDCONTROLKEY=0x%04x"));
            snprintf(cmdbuf, sizeof(cmdbuf), fmtbuf, item->consumer);
            return at_command(cmdbuf, NULL, 0, true, timeout);

#ifdef MOUSE_ENABLE
        case QTMouseMove:
            strcpy_P(fmtbuf, PSTR("AT+BLEHIDMOUSEMOVE=%d,%d,%d,%d"));
            snprintf(cmdbuf, sizeof(cmdbuf), fmtbuf, item->mousemove.x, item->mousemove.y, item->mousemove.scroll, item->mousemove.pan);
            if (!at_command(cmdbuf, NULL, 0, true, timeout)) {
                return false;
            }
            strcpy_P(cmdbuf, PSTR("AT+BLEHIDMOUSEBUTTON="));
            if (item->mousemove.buttons & MOUSE_BTN1) {
                strcat(cmdbuf, "L");
            }
            if (item->mousemove.buttons & MOUSE_BTN2) {
                strcat(cmdbuf, "R");
            }
            if (item->mousemove.buttons & MOUSE_BTN3) {
                strcat(cmdbuf, "M");
            }
            if (item->mousemove.buttons == 0) {
                strcat(cmdbuf, "0");
            }
            return at_command(cmdbuf, NULL, 0, true, timeout);
#endif
        default:
            return true;
    }
}

bool nrf24_send_keys(uint8_t hid_modifier_mask, uint8_t *keys, uint8_t nkeys) {
    struct queue_item item;
    bool              didWait = false;

    item.queue_type   = QTKeyReport;
    item.key.modifier = hid_modifier_mask;
    item.added        = timer_read();

    while (nkeys >= 0) {
        item.key.keys[0] = keys[0];
        item.key.keys[1] = nkeys >= 1 ? keys[1] : 0;
        item.key.keys[2] = nkeys >= 2 ? keys[2] : 0;
        item.key.keys[3] = nkeys >= 3 ? keys[3] : 0;
        item.key.keys[4] = nkeys >= 4 ? keys[4] : 0;
        item.key.keys[5] = nkeys >= 5 ? keys[5] : 0;

        if (!send_buf.enqueue(item)) {
            if (!didWait) {
                dprint("wait for buf space\n");
                didWait = true;
            }
            send_buf_send_one();
            continue;
        }

        if (nkeys <= 6) {
            return true;
        }

        nkeys -= 6;
        keys += 6;
    }

    return true;
}

bool nrf24_send_consumer_key(uint16_t keycode, int hold_duration) {
    struct queue_item item;

    item.queue_type = QTConsumer;
    item.consumer   = keycode;

    while (!send_buf.enqueue(item)) {
        send_buf_send_one();
    }
    return true;
}

#ifdef MOUSE_ENABLE
bool nrf24_send_mouse_move(int8_t x, int8_t y, int8_t scroll, int8_t pan, uint8_t buttons) {
    struct queue_item item;

    item.queue_type        = QTMouseMove;
    item.mousemove.x       = x;
    item.mousemove.y       = y;
    item.mousemove.scroll  = scroll;
    item.mousemove.pan     = pan;
    item.mousemove.buttons = buttons;

    while (!send_buf.enqueue(item)) {
        send_buf_send_one();
    }
    return true;
}
#endif

uint32_t nrf24_read_battery_voltage(void) { return state.vbat; }

bool nrf24_set_mode_leds(bool on) {
    if (!state.configured) {
        return false;
    }

    // The "mode" led is the red blinky one
    at_command_P(on ? PSTR("AT+HWMODELED=1") : PSTR("AT+HWMODELED=0"), NULL, 0);

    // Pin 19 is the blue "connected" LED; turn that off too.
    // When turning LEDs back on, don't turn that LED on if we're
    // not connected, as that would be confusing.
    at_command_P(on && state.is_connected ? PSTR("AT+HWGPIO=19,1") : PSTR("AT+HWGPIO=19,0"), NULL, 0);
    return true;
}

// https://learn.adafruit.com/adafruit-feather-32u4-bluefruit-le/ble-generic#at-plus-blepowerlevel
bool nrf24_set_power_level(int8_t level) {
    char cmd[46];
    if (!state.configured) {
        return false;
    }
    snprintf(cmd, sizeof(cmd), "AT+BLEPOWERLEVEL=%d", level);
    return at_command(cmd, NULL, 0, false);
}
