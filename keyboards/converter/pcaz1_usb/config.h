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

#pragma once

#include "config_common.h"

/* USB Device descriptor parameter */
#define VENDOR_ID    0xFEED
#define PRODUCT_ID   0x0000
#define DEVICE_VER   0x0001
#define MANUFACTURER Rohde & Schwarz
#define PRODUCT      PCA-Z1

/* key matrix size */
#define MATRIX_ROWS 12
#define MATRIX_COLS 8

                                // Future remap for easier port manipulation / cleaner code

#define LED_CLK_PIN         B12 // PB9
#define LED_NUM_LOCK_PIN    B5  // PB12
#define LED_CAPS_LOCK_PIN   B6  // PB13
#define LED_ENC_H_PIN       B3  // PB14
#define LED_ENC_V_PIN       B4  // PB15

#define ENCODERS_PAD_A { B14 }  // PB7
#define ENCODERS_PAD_B { B15 }  // PB8

#define MATRIX_MUX_A A0     // PB3
#define MATRIX_MUX_B A1     // PB4
#define MATRIX_MUX_C A2     // PB5
#define MATRIX_MUX_D A3     // PB6

#define MATRIX_ROW_0 A4     // A0
#define MATRIX_ROW_1 A5     // A1
#define MATRIX_ROW_2 A6     // A2
#define MATRIX_ROW_3 A7     // A3
#define MATRIX_ROW_4 B0     // A4
#define MATRIX_ROW_5 B1     // A5
#define MATRIX_ROW_6 C15    // A6
#define MATRIX_ROW_7 B10    // A7

// #define LED_SCROLL_LOCK_PIN B2
// #define LED_COMPOSE_PIN B3
// #define LED_KANA_PIN B4

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE 0

/* define if matrix has ghost (lacks anti-ghosting diodes) */
//#define MATRIX_HAS_GHOST

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
// #define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
// #define LOCKING_RESYNC_ENABLE

/* If defined, GRAVE_ESC will always act as ESC when CTRL is held.
 * This is useful for the Windows task manager shortcut (ctrl+shift+esc).
 */
//#define GRAVE_ESC_CTRL_OVERRIDE

/*
 * Force NKRO
 *
 * Force NKRO (nKey Rollover) to be enabled by default, regardless of the saved
 * state in the bootmagic EEPROM settings. (Note that NKRO must be enabled in the
 * makefile for this to work.)
 *
 * If forced on, NKRO can be disabled via magic key (default = LShift+RShift+N)
 * until the next keyboard reset.
 *
 * NKRO may prevent your keystrokes from being detected in the BIOS, but it is
 * fully operational during normal computer usage.
 *
 * For a less heavy-handed approach, enable NKRO via magic key (LShift+RShift+N)
 * or via bootmagic (hold SPACE+N while plugging in the keyboard). Once set by
 * bootmagic, NKRO mode will always be enabled until it is toggled again during a
 * power-up.
 *
 */
//#define FORCE_NKRO

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT

/* disable these deprecated features by default */
#define NO_ACTION_MACRO
#define NO_ACTION_FUNCTION

/* Bootmagic Lite key configuration */
//#define BOOTMAGIC_LITE_ROW 0
//#define BOOTMAGIC_LITE_COLUMN 0
