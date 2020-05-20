#include "nrf24.h"
#include "nRF24L01.h"
// #include "lib/RF24.hpp"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
// #include <alloca.h>
// #include "debug.h"
// #include "timer.h"
// #include "action_util.h"
// #include "ringbuffer.hpp"
// #include <string.h>
// #include "wait.h"
// #include "analog.h"

// These are the pin assignments for the XD60 rev3 PCB.
// You need to unsolder the 2 10ohm pullup resistors next
// to the I2C header on the back of the PCB and wire your
// NRF24L01+ module using SDA (F7) for CE and SCL (F4) for CS
// You may define them to something else in your config.h
// if yours is wired up differently.
#define NRF24_CE F7
#define NRF24_CS F4

// IRQ is not implemented yet.
#define NRF24_IRQ E6

// RF modules support 10 Mhz SPI bus speed
#define NRF24_SPI_CLOCK_SPEED 10000000  // SCK frequency

#define SCK_DIVISOR (F_CPU / NRF24_SPI_CLOCK_SPEED)

enum unifying_params {
    pairingMacAddress   = 0xBB0ADCA575LL,
    payloadSize         = 22,
    channel             = 5,
};

static struct {
    bool is_connected;
    bool initialized;
    bool configured;
} state;

bool nrf24_init(void) {

    state.initialized  = false;
    state.configured   = false;
    state.is_connected = false;

    spi_status_t setup = 0;

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

    // Reset NRF_CONFIG and enable 16-bit CRC.
    nrf24_write_register( NRF_CONFIG, 0x0C ) ;

    // Set 1500uS (minimum for 32B payload in ESB@250KBPS) timeouts, to make testing a little easier
    // WARNING: If this is ever lowered, either 250KBS mode with AA is broken or maximum packet
    // sizes must never be used. See documentation for a more complete explanation.
    nrf24_setRetries(5,15);

    setup = nrf24_read_register(RF_SETUP);

    // Then set data rate to 2MBPS
    nrf24_setDataRate( RF24_2MBPS ) ;

    // Disable dynamic payloads, to match dynamic_payloads_enabled setting - Reset value is 0
    nrf24_toggleFeatures();
    nrf24_write_register(FEATURE,0 );
    nrf24_write_register(DYNPD,0);

    // Reset current status
    // Notice reset and flush is the last thing we do
    nrf24_write_register(NRF_STATUS,_BV(RX_DR) | _BV(TX_DS) | _BV(MAX_RT) );

    // Set up default configuration.  Callers can always change it later.
    // This channel should be universally safe and not bleed over into adjacent
    // spectrum.
    nrf24_setChannel(76);

    // Flush buffers
    nrf24_flush_rx();
    nrf24_flush_tx();

    nrf24_powerUp(); //Power up by default when begin() is called

    // Enable PTX, do not write CE high so radio will remain in standby I mode ( 130us max to transition to RX or TX instead of 1500us from powerUp )
    // PTX should use only 22uA of power
    nrf24_write_register(NRF_CONFIG, ( nrf24_read_register(NRF_CONFIG) ) & ~_BV(PRIM_RX) );

    // if setup is 0 or ff then there was no response from module
    state.initialized = setup != 0 && setup != 0xff ? true : false;
    return state.initialized;
}

/****************************************************************************/

void nrf24_end_transaction(void) {
    writePinLow(NRF24_CS);
}

void nrf24_begin_transaction(void) {
    writePinLow(NRF24_CS);
}

/****************************************************************************/

spi_status_t nrf24_read_register_len(uint8_t reg, uint8_t* buf, uint8_t len) {
  spi_status_t status;

  nrf24_begin_transaction();
  status = spi_write( R_REGISTER | ( REGISTER_MASK & reg ) );
  spi_receive(buf, len);
  nrf24_end_transaction();

  return status;
}

spi_status_t nrf24_read_register(uint8_t reg) {
  spi_status_t result;

  nrf24_begin_transaction();
  spi_write( R_REGISTER | ( REGISTER_MASK & reg ) );
  result = spi_read();
  nrf24_end_transaction();

  return result;
}

/****************************************************************************/

spi_status_t nrf24_write_register_len(uint8_t reg, const uint8_t* buf, uint8_t len) {
    uint8_t status;

    nrf24_begin_transaction();
    status =spi_write( W_REGISTER | ( REGISTER_MASK & reg ) );
    spi_transmit(buf, len);
    nrf24_end_transaction();

    return status;
}

spi_status_t nrf24_write_register(uint8_t reg, uint8_t value) {
    spi_status_t status;

    nrf24_begin_transaction();
    status = spi_write( W_REGISTER | ( REGISTER_MASK & reg ) );
    spi_write(value);
    nrf24_end_transaction();

    return status;
}

spi_status_t nrf24_flush_rx(void)
{
  return nrf24_spi_trans( FLUSH_RX );
}

spi_status_t nrf24_flush_tx(void)
{
  return nrf24_spi_trans( FLUSH_TX );
}

spi_status_t nrf24_spi_trans(uint8_t cmd){

  spi_status_t status;

  nrf24_begin_transaction();
  status = spi_write( cmd );
  nrf24_end_transaction();

  return status;
}

/****************************************************************************/

//Power up now. Radio will not power down unless instructed by MCU for config changes etc.
void nrf24_powerUp(void)
{
   spi_status_t cfg = nrf24_read_register(NRF_CONFIG);

   // if not powered up then power up and wait for the radio to initialize
   if (!(cfg & _BV(PWR_UP))){
      nrf24_write_register(NRF_CONFIG, cfg | _BV(PWR_UP));

      // For nRF24L01+ to go from power down mode to TX or RX mode it must first pass through stand-by mode.
	  // There must be a delay of Tpd2stby (see Table 16.) after the nRF24L01+ leaves power down mode before
	  // the CEis set high. - Tpd2stby can be up to 5ms per the 1.0 datasheet
      wait_ms(5);
   }
}

void nrf24_toggleFeatures(void)
{
    nrf24_begin_transaction();
	spi_write( ACTIVATE );
    spi_write( 0x73 );
	nrf24_end_transaction();
}

void nrf24_setChannel(uint8_t channel)
{
  const uint8_t max_channel = 125;
  nrf24_write_register(RF_CH,rf24_min(channel,max_channel));
}

bool nrf24_setDataRate(rf24_datarate_e speed) {
  bool result = false;
  spi_status_t setup = nrf24_read_register(RF_SETUP) ;

  // HIGH and LOW '00' is 1Mbs - our default
  setup &= ~(_BV(RF_DR_LOW) | _BV(RF_DR_HIGH)) ;

  txDelay=85;
  if( speed == RF24_250KBPS )
  {
    // Must set the RF_DR_LOW to 1; RF_DR_HIGH (used to be RF_DR) is already 0
    // Making it '10'.
    setup |= _BV( RF_DR_LOW ) ;
	txDelay=155;
  }
  else
  {
    // Set 2Mbs, RF_DR (RF_DR_HIGH) is set 1
    // Making it '01'
    if ( speed == RF24_2MBPS )
    {
      setup |= _BV(RF_DR_HIGH);
	  txDelay=65;
    }
  }
  nrf24_write_register(RF_SETUP, setup);

  // Verify our result
  if ( nrf24_read_register(RF_SETUP) == setup )
  {
    result = true;
  }
  return result;
}

void nrf24_setRetries(uint8_t delay, uint8_t count) {
    nrf24_write_register(SETUP_RETR,(delay&0xf)<<ARD | (count&0xf)<<ARC);
}

void nrf24_disableCRC(void) {
  uint8_t disable = nrf24_read_register(NRF_CONFIG) & ~_BV(EN_CRC) ;
  nrf24_write_register( NRF_CONFIG, disable ) ;
}
