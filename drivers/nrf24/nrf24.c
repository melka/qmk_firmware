/*
Copyright 2020 Kamel Makhloufi <https://github.com/melka>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <avr/io.h>
#include <util/delay.h>
#include "spi_master.h"
#include "nrf24.h"
#include "kp/kp_aes.h"
#include "kp/kp_hardware.h"
#include "kp/kp_nrf24.h"

#if defined(__AVR_ATmega32U4__)
#ifndef NRF24_CE
#define NRF24_CE 5   // b5
#endif
#ifndef NRF24_CSN
#define NRF24_CSN 6  // b6
#endif
#define MOSI 2   // b2
#define MISO 3   // b3
#define SCK  1   // b1
#elif defined(__AVR_ATmega328P__)
#ifndef NRF24_CE
#define NRF24_CE 1   // b1
#endif
#ifndef NRF24_CSN
#define NRF24_CSN 2  // b2
#endif
#define MOSI 3   // b3
#define MISO 4   // b4
#define SCK  5   // b5
#endif

#define CREATE_BIT_SETTER(func, port, port_bit) \
void func(uint8_t val) { \
  if (val) { \
    PORT##port |= (1 << port_bit);\
  } else { \
    PORT##port &= ~(1 << port_bit);\
  } \
} \
inline void ddr_##func(uint8_t val) { \
  if (val) { \
    DDR##port |= (1 << port_bit);\
  } else { \
    DDR##port &= ~(1 << port_bit);\
  } \
} \

#if defined(__AVR_ATmega32U4__)
CREATE_BIT_SETTER(nrf24_ce, B, NRF24_CE);
CREATE_BIT_SETTER(nrf24_csn, B, NRF24_CSN);
#elif defined(__AVR_ATmega328P__)
CREATE_BIT_SETTER(nrf24_ce, B, NRF24_CE);
CREATE_BIT_SETTER(nrf24_csn, B, NRF24_CSN);
#endif

void nrf24_init(void) {
    /* TODO: change this define to an appropriate one for the promicro */
    ddr_nrf24_csn(1);
    ddr_nrf24_ce(1);
    DDRB |= (0<<MISO)|(1<<MOSI)|(1<<SCK);
    nrf24_csn(1);
    nrf24_ce(0);

    // nrf supports 10Mbs on spi
    // setup spi with: msb, mode0, clk/2
    SPCR = (1<<SPI2X) | (1<<SPE) | (1<<MSTR);

    aes_key_init(AES_EKEY, AES_DKEY);
}

uint8_t nrf24_spi_send_byte(uint8_t byte) {
    SPDR = byte;
    while(!(SPSR & (1<<SPIF)));
    return SPDR;
}

void led_testing_toggle(uint8_t led_num) {
}

#if defined(AES_CRYPTO_LIB)

#include <string.h>
#include "aes/avr-crypto-lib/aes.h"

static aes128_ctx_t aes_ctx;

void aes_key_init(const uint8_t *ekey, const uint8_t *dkey) {
    aes128_init(ekey, &aes_ctx);
}

void aes_encrypt(uint8_t *block) {
    aes128_enc(block, &aes_ctx);
}

void aes_decrypt(uint8_t *block) {
    aes128_dec(block, &aes_ctx);
}
#endif
// #include "timer.h"
// #include "print.h"

// #include <string.h>

// #include "progmem.h"
