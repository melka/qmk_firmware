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
#include "spi_master.h"

// #include "config_common.h"
// #include "progmem.h"

typedef enum { RF24_1MBPS = 0, RF24_2MBPS, RF24_250KBPS } rf24_datarate_e;
typedef enum { RF24_CRC_DISABLED = 0, RF24_CRC_8, RF24_CRC_16 } rf24_crclength_e;

#define rf24_max(a,b) (a>b?a:b)
#define rf24_min(a,b) (a<b?a:b)

/**
  *
  * The driver will delay for this duration when stopListening() is called
  *
  * When responding to payloads, faster devices like ARM(RPi) are much faster than Arduino:
  * 1. Arduino sends data to RPi, switches to RX mode
  * 2. The RPi receives the data, switches to TX mode and sends before the Arduino radio is in RX mode
  * 3. If AutoACK is disabled, this can be set as low as 0. If AA/ESB enabled, set to 100uS minimum on RPi
  *
  * @warning If set to 0, ensure 130uS delay after stopListening() and before any sends
  */
uint32_t txDelay;

bool nrf24_init(void);

bool nrf24_setDataRate(rf24_datarate_e speed);
void nrf24_setRetries(uint8_t delay, uint8_t count);
void nrf24_disableCRC(void);
void nrf24_powerUp(void);
void nrf24_toggleFeatures(void);
void nrf24_setChannel(uint8_t channel);

void nrf24_end_transaction(void);
void nrf24_begin_transaction(void);

spi_status_t nrf24_read_register_len(uint8_t reg, uint8_t* buf, uint8_t len);
spi_status_t nrf24_read_register(uint8_t reg);

spi_status_t nrf24_write_register_len(uint8_t reg, const uint8_t* buf, uint8_t len);
spi_status_t nrf24_write_register(uint8_t reg, uint8_t value);

spi_status_t nrf24_flush_rx(void);
spi_status_t nrf24_flush_tx(void);

spi_status_t nrf24_spi_trans(uint8_t cmd);

