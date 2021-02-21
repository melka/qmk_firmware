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

void initPins(void) {
    setPinOutput(LED_CLK_PIN);
    writePinLow(LED_CLK_PIN);

    setPinOutput(MATRIX_MUX_A);
    setPinOutput(MATRIX_MUX_B);
    setPinOutput(MATRIX_MUX_C);
    setPinOutput(MATRIX_MUX_D);

    setPinInput(MATRIX_COL_0);
    setPinInput(MATRIX_COL_1);
    setPinInput(MATRIX_COL_2);
    setPinInput(MATRIX_COL_3);
    setPinInput(MATRIX_COL_4);
    setPinInput(MATRIX_COL_5);
    setPinInput(MATRIX_COL_6);
    setPinInput(MATRIX_COL_7);
}

void strobeLeds(void) {
    writePinHigh(LED_CLK_PIN);
    wait_ms(10);
    writePinLow(LED_CLK_PIN);
}

int invertPin(long unsigned int pin) {
    return readPin(pin) == 1 ? 0 : 1;
}

// I'll clean that shit later
// Spaghetto code

int scanRow(void) {
    int row = 0;
    row |= (invertPin(MATRIX_COL_0) << 7);
    row |= (invertPin(MATRIX_COL_1) << 6);
    row |= (invertPin(MATRIX_COL_2) << 5);
    row |= (invertPin(MATRIX_COL_3) << 4);
    row |= (invertPin(MATRIX_COL_4) << 3);
    row |= (invertPin(MATRIX_COL_5) << 2);
    row |= (invertPin(MATRIX_COL_6) << 1);
    row |= (invertPin(MATRIX_COL_7) << 0);
    return row;
}

void setRow(int row) {
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
        default :
            writePinLow(MATRIX_MUX_A);
            writePinLow(MATRIX_MUX_B);
            writePinLow(MATRIX_MUX_C);
            writePinLow(MATRIX_MUX_D);
            break;
    }
}

void matrix_init_custom(void) {
    // TODO: initialize hardware here

    debug_enable=true;
    debug_matrix=true;

    initPins();
    setRow(4);
    strobeLeds();
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool matrix_has_changed = false;

    // TODO: add matrix scanning routine here
    print("--------------\n");
    // for (int i = 0; i < 8; i++) {
    //     setRow(0);
    //     wait_ms(10);
    //     xprintf("Row %i : ", i); pbin(scanRow()); print("\n");
    // }
    print("Row 4 : "); pbin(scanRow()); print("\n");

    return matrix_has_changed;
}
