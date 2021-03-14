/*
Copyright 2021 Kamel Makhloufi

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

#include QMK_KEYBOARD_H
#include "quantum.h"
#include "print.h"
#include "debug.h"

// matrix state buffer(1:on, 0:off)
static matrix_row_t matrix[MATRIX_ROWS];

static void setRow(int col);
static uint8_t scanRow(int col);
static void initPins(void);
static int invertPin(long unsigned int pin);

__attribute__ ((weak))
void matrix_init_kb(void) {
    matrix_init_user();
}

__attribute__ ((weak))
void matrix_scan_kb(void) {
    matrix_scan_user();
}

__attribute__ ((weak))
void matrix_init_user(void) {
}

__attribute__ ((weak))
void matrix_scan_user(void) {
}

void matrix_init(void) {
    // TODO: initialize hardware here

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) matrix[i] = 0x00;

    matrix_init_quantum();

    debug_enable=true;
    debug_matrix=true;

    initPins();
}

uint8_t matrix_scan(void) {
    // TODO: add matrix scanning routine here
    for (int i = 0; i < MATRIX_ROWS; i++) {
        matrix[i] = scanRow(i);
    }

    matrix_scan_quantum();
    return 1;
}

void matrix_print(void){
    // DO NOTHING
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

bool led_update_kb(led_t led_state) {
    bool res = led_update_user(led_state);

    if(res) {
#ifdef CONSOLE_ENABLE
        uprintf("Led state : alpha: %d, num: %d, kana: %d, compose: %d\n", led_state.num_lock, led_state.caps_lock, led_state.kana, led_state.compose);
#endif
        writePin(LED_NUMLOCK_PIN, !led_state.num_lock);
        writePin(LED_CAPSLOCK_PIN, !led_state.caps_lock);
        // Strobe output
        writePinHigh(LED_CLK_PIN);
        wait_ms(5);
        writePinLow(LED_CLK_PIN);
    }

    return res;
}

// Initializes pins state
inline
static void initPins(void) {
    setPinOutput(LED_ENC_H_PIN);
    setPinOutput(LED_ENC_V_PIN);
    setPinOutput(LED_NUMLOCK_PIN);
    setPinOutput(LED_CAPSLOCK_PIN);

    setPinOutput(LED_CLK_PIN);
    writePinLow(LED_CLK_PIN);

    setPinOutput(MATRIX_MUX_A);
    setPinOutput(MATRIX_MUX_B);
    setPinOutput(MATRIX_MUX_C);
    setPinOutput(MATRIX_MUX_D);

    setPinInput(MATRIX_ROW_0);
    setPinInput(MATRIX_ROW_1);
    setPinInput(MATRIX_ROW_2);
    setPinInput(MATRIX_ROW_3);
    setPinInput(MATRIX_ROW_4);
    setPinInput(MATRIX_ROW_5);
    setPinInput(MATRIX_ROW_6);
    setPinInput(MATRIX_ROW_7);
}

// Pins are inactive High, so we invert the state when reading
inline
static int invertPin(long unsigned int pin) {
    return readPin(pin) == 1 ? 0 : 1;
}

inline
static void setRow(int row) {
    switch (row) {
        case 0 :
            writePinLow(MATRIX_MUX_A);
            writePinLow(MATRIX_MUX_B);
            writePinLow(MATRIX_MUX_C);
            writePinLow(MATRIX_MUX_D);
            break;
        case 1 :
            writePinHigh(MATRIX_MUX_A);
            writePinLow(MATRIX_MUX_B);
            writePinLow(MATRIX_MUX_C);
            writePinLow(MATRIX_MUX_D);
            break;
        case 2 :
            writePinLow(MATRIX_MUX_A);
            writePinHigh(MATRIX_MUX_B);
            writePinLow(MATRIX_MUX_C);
            writePinLow(MATRIX_MUX_D);
            break;
        case 3 :
            writePinHigh(MATRIX_MUX_A);
            writePinHigh(MATRIX_MUX_B);
            writePinLow(MATRIX_MUX_C);
            writePinLow(MATRIX_MUX_D);
            break;
        case 4 :
            writePinLow(MATRIX_MUX_A);
            writePinLow(MATRIX_MUX_B);
            writePinHigh(MATRIX_MUX_C);
            writePinLow(MATRIX_MUX_D);
            break;
        case 5 :
            writePinHigh(MATRIX_MUX_A);
            writePinLow(MATRIX_MUX_B);
            writePinHigh(MATRIX_MUX_C);
            writePinLow(MATRIX_MUX_D);
            break;
        case 6 :
            writePinLow(MATRIX_MUX_A);
            writePinHigh(MATRIX_MUX_B);
            writePinHigh(MATRIX_MUX_C);
            writePinLow(MATRIX_MUX_D);
            break;
        case 7 :
            writePinHigh(MATRIX_MUX_A);
            writePinHigh(MATRIX_MUX_B);
            writePinHigh(MATRIX_MUX_C);
            writePinLow(MATRIX_MUX_D);
            break;
        case 8 :
            writePinLow(MATRIX_MUX_A);
            writePinLow(MATRIX_MUX_B);
            writePinLow(MATRIX_MUX_C);
            writePinHigh(MATRIX_MUX_D);
            break;
        case 9 :
            writePinHigh(MATRIX_MUX_A);
            writePinLow(MATRIX_MUX_B);
            writePinLow(MATRIX_MUX_C);
            writePinHigh(MATRIX_MUX_D);
            break;
        case 10 :
            writePinLow(MATRIX_MUX_A);
            writePinHigh(MATRIX_MUX_B);
            writePinLow(MATRIX_MUX_C);
            writePinHigh(MATRIX_MUX_D);
            break;
        case 11 :
            writePinHigh(MATRIX_MUX_A);
            writePinHigh(MATRIX_MUX_B);
            writePinLow(MATRIX_MUX_C);
            writePinHigh(MATRIX_MUX_D);
            break;
        case 12 :
            writePinLow(MATRIX_MUX_A);
            writePinLow(MATRIX_MUX_B);
            writePinHigh(MATRIX_MUX_C);
            writePinHigh(MATRIX_MUX_D);
            break;
        case 13 :
            writePinHigh(MATRIX_MUX_A);
            writePinLow(MATRIX_MUX_B);
            writePinHigh(MATRIX_MUX_C);
            writePinHigh(MATRIX_MUX_D);
            break;
        case 14 :
            writePinLow(MATRIX_MUX_A);
            writePinHigh(MATRIX_MUX_B);
            writePinHigh(MATRIX_MUX_C);
            writePinHigh(MATRIX_MUX_D);
            break;
        case 15 :
            writePinHigh(MATRIX_MUX_A);
            writePinHigh(MATRIX_MUX_B);
            writePinHigh(MATRIX_MUX_C);
            writePinHigh(MATRIX_MUX_D);
            break;
        default :
            writePinLow(MATRIX_MUX_A);
            writePinLow(MATRIX_MUX_B);
            writePinLow(MATRIX_MUX_C);
            writePinLow(MATRIX_MUX_D);
            break;
    }
}

inline
static uint8_t scanRow(int row) {
    setRow(row);
    wait_ms(1);

    uint8_t res = 0;
    res |= (invertPin(MATRIX_ROW_0) << 7);
    res |= (invertPin(MATRIX_ROW_1) << 6);
    res |= (invertPin(MATRIX_ROW_2) << 5);
    res |= (invertPin(MATRIX_ROW_3) << 4);
    res |= (invertPin(MATRIX_ROW_4) << 3);
    res |= (invertPin(MATRIX_ROW_5) << 2);
    res |= (invertPin(MATRIX_ROW_6) << 1);
    res |= (invertPin(MATRIX_ROW_7) << 0);
    return res;
}
