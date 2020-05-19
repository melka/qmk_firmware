/* NRF24L01+ support for QMK.
 * Author: Kamel Makhloufi, 2016
 * Supports using an SPI NRF24L01+ module, fur use with unpatched Logitech Unifying dongles (v12.01).
 *
 * First test implementation based on the arduino code by Ronan Gaillard
 * https://github.com/ronangaillard/logitech-mouse
 *
 * NRF code adapted from RF24 library by J. Coliz <maniacbug@ymail.com>
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// #include "config_common.h"
// #include "progmem.h"

/* Instruct the module to enable HID keyboard support and reset */
extern bool nrf24_enable_keyboard(void);

/* Query to see if the BLE module is connected */
extern bool nrf24_query_is_connected(void);

/* Returns true if we believe that the BLE module is connected.
 * This uses our cached understanding that is maintained by
 * calling ble_task() periodically. */
extern bool nrf24_is_connected(void);

/* Call this periodically to process BLE-originated things */
extern void nrf24_task(void);

/* Generates keypress events for a set of keys.
 * The hid modifier mask specifies the state of the modifier keys for
 * this set of keys.
 * Also sends a key release indicator, so that the keys do not remain
 * held down. */
extern bool nrf24_send_keys(uint8_t hid_modifier_mask, uint8_t *keys, uint8_t nkeys);

/* Send a consumer keycode, holding it down for the specified duration
 * (milliseconds) */
extern bool nrf24_send_consumer_key(uint16_t keycode, int hold_duration);

#    ifdef MOUSE_ENABLE
/* Send a mouse/wheel movement report.
 * The parameters are signed and indicate positive of negative direction
 * change. */
extern bool nrf24_send_mouse_move(int8_t x, int8_t y, int8_t scroll, int8_t pan, uint8_t buttons);
#    endif

/* Compute battery voltage by reading an analog pin.
 * Returns the integer number of millivolts */
extern uint32_t nrf24_read_battery_voltage(void);

extern bool nrf24_set_mode_leds(bool on);
extern bool nrf24_set_power_level(int8_t level);
