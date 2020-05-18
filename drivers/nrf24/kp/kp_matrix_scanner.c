// Copyright 2017 jem@seethis.link
// Licensed under the MIT license (http://opensource.org/licenses/MIT)
/// @file core/matrix_scanner.c

#include "config.h"

#include "core/matrix_scanner.h"

#include <string.h>

#include "core/error.h"
#include "core/io_map.h"
#include "core/layout.h"
#include "core/packet.h"
#include "core/settings.h"
#include "core/timer.h"
#include "core/usb_commands.h"

#define MAX_UPDATE_LIST 16
#define MAX_DOWN_LIST 16

// can probably make this static
XRAM uint8_t g_matrix[MAX_NUM_ROWS][IO_PORT_COUNT*sizeof(port_mask_t)];

/// This variable will be set to true if any row/col position in the matrix is
/// updated (even if it is unused is the keymap).
XRAM uint8_t s_has_raw_matrix_updated;
XRAM uint8_t s_has_updated;

// #if INTERNAL_SCAN_METHOD == MATRIX_SCANNER_INTERNAL_FAST_ROW_COL
// XRAM uint8_t g_col_masks[IO_PORT_COUNT];
// #endif

static XRAM uint8_t s_is_debouncing[MAX_NUM_ROWS][IO_PORT_COUNT];
static XRAM uint8_t s_debounce_time[MAX_NUM_KEYS];
static XRAM uint8_t s_invalid_key_debounce_time;
static XRAM uint8_t s_debounce_type[KEY_NUMBER_BITMAP_SIZE];
static XRAM uint8_t s_matrix_number_keys_down;
static XRAM uint8_t s_matrix_number_keys_debouncing;

XRAM uint8_t g_key_num_bitmap[KEY_NUMBER_BITMAP_SIZE];

XRAM matrix_scan_plan_t g_scan_plan;

XRAM uint8_t g_delta_list[MAX_UPDATE_LIST];
XRAM uint8_t g_delta_list_len;

XRAM uint8_t g_down_list[MAX_DOWN_LIST];
XRAM uint8_t g_down_list_len;

static void scanner_init_debouncer(void);

// TODO: probably change this
uint8_t get_matrix_compressed_size(void) {
    return INT_DIV_ROUND_UP(g_scan_plan.max_key_num, 8);
}

static inline uint8_t get_key_number(uint8_t row, uint8_t col) {
#if INTERNAL_SCAN_METHOD == MATRIX_SCANNER_INTERNAL_FAST_ROW_COL
    return flash_read_byte(
        LAYOUT_PORT_KEY_NUM_MAP_ADDR + row*(g_scan_plan.max_col_pin_num+1) + col
    );
#elif INTERNAL_SCAN_METHOD == MATRIX_SCANNER_INTERNAL_SLOW_ROW_COL
    return flash_read_byte(
        LAYOUT_PORT_KEY_NUM_MAP_ADDR + row*g_scan_plan.cols + col
    );
#endif
}

static inline uint8_t get_debounce_time(uint8_t key_num) {
    if (key_num == INVALID_KEY_NUMBER) {
        return s_invalid_key_debounce_time;
    } else {
        return s_debounce_time[key_num];
    }
}

static inline void set_debounce_time(uint8_t key_num, uint8_t time) {
    if (key_num == INVALID_KEY_NUMBER) {
        s_invalid_key_debounce_time = time;
    } else {
        s_debounce_time[key_num] = time;
    }
}

int init_matrix_scanner_utils(void) {
    if (has_critical_error()) {
        return -1;
    }

    if (
        g_scan_plan.rows > MAX_NUM_ROWS ||
        g_scan_plan.max_col_pin_num > IO_PORT_MAX_PIN_NUM
    ) {
        memset((uint8_t*)&g_scan_plan, 0, sizeof(matrix_scan_plan_t));
        register_error(ERROR_MATRIX_PINS_CONFIG_TOO_LARGE);
        return -1;
    }

    g_delta_list_len = 0;
    g_down_list_len = 0;
    // TODO: load scan key map
    memset(g_key_num_bitmap, 0, KEY_NUMBER_BITMAP_SIZE);
    s_has_raw_matrix_updated = 0;

    scanner_init_debouncer();

    return 0;
}

// TODO:
void scanner_add_matrix_key(uint8_t key_num) {
    s_has_raw_matrix_updated = 1;
    if (key_num == INVALID_KEY_NUMBER) {
        return;
    }
    s_has_updated = 1;

    // add then key to the delta list
    if (g_delta_list_len < MAX_UPDATE_LIST) {
        g_delta_list[g_delta_list_len] = MATRIX_DELTA_TYPE_PRESSED | key_num;
        g_delta_list_len++;
    }

    if (g_down_list_len < MAX_DOWN_LIST) {
        g_down_list[g_down_list_len] = key_num;
        g_down_list_len++;
    }

    { // add key code to the key number bitmap
        g_key_num_bitmap[key_num / 8] |= (1 << (key_num % 8));
    }
}

void scanner_del_matrix_key(uint8_t key_num) {
    s_has_raw_matrix_updated = 1;
    if (key_num == INVALID_KEY_NUMBER) {
        return;
    }
    s_has_updated = 1;

    // add then key to the delta list
    if (g_delta_list_len < MAX_UPDATE_LIST) {
        g_delta_list[g_delta_list_len] = MATRIX_DELTA_TYPE_RELEASED | key_num;
        g_delta_list_len++;
    }

    // delete key from the down key list
    if (g_down_list_len > 0) { // sanity check
        uint8_t i;
        // find key, and replace it with the last key in the list
        for (i = 0; i < g_down_list_len; ++i) {
            if (g_down_list[i] == key_num) {
                g_down_list[i] = g_down_list[g_down_list_len - 1];
                g_down_list_len--;
                break;
            }
        }
    }

    { // delete key code from the key number bitmap
        g_key_num_bitmap[key_num / 8] &= ~(1 << (key_num % 8));
    }
}

uint8_t get_matrix_data(uint8_t *dest, bool use_deltas) {
    const uint8_t matrix_size = get_matrix_compressed_size();
    const uint8_t num_keys_down = get_matrix_num_keys_down();

#if SCANNER_MATRIX_DELTA != 0
    const uint8_t num_keys_changed = g_delta_list_len;
    g_delta_list_len = 0; // clear update list even if we don't use it

    if (use_deltas || (matrix_size > (PACKET_PAYLOAD_LENGTH-1) && num_keys_down > PACKET_PAYLOAD_LENGTH-1)) {
        if ( (num_keys_changed < num_keys_down) && (num_keys_changed < matrix_size) ) {
            *dest = (PACKET_MATRIX_DELTA_LIST << PACKET_MATRIX_TYPE_BIT_POS) | (num_keys_changed & PACKET_MATRIX_SIZE_MASK);
            dest++;
            memcpy(dest, g_delta_list, num_keys_changed);
            return num_keys_changed + 1;
        }
    }
#endif

    // A few keys down, send a list of keys instead of the whole matrix.
    if (num_keys_down < matrix_size) {
        *dest = (PACKET_MATRIX_KEY_LIST << PACKET_MATRIX_TYPE_BIT_POS) | (num_keys_down & PACKET_MATRIX_SIZE_MASK);
        dest++;
        memcpy(dest, g_down_list, g_down_list_len);
        return num_keys_down+1;
    }

    // Lots of keys down, more efficient to send the whole matrix.
    else {
        *dest = (PACKET_MATRIX_RAW << PACKET_MATRIX_TYPE_BIT_POS) | (matrix_size & PACKET_MATRIX_SIZE_MASK);
        dest++;
        memcpy(dest, g_key_num_bitmap, matrix_size);
        return matrix_size+1;
    }
}

static void scanner_init_debouncer(void) {
    memset(s_is_debouncing, 0, sizeof(s_is_debouncing));
    s_matrix_number_keys_down = 0;
    s_matrix_number_keys_debouncing = 0;
}

bool scanner_debounce_row(
    uint8_t row,
    const uint8_t *new_row,
    uint8_t bytes_per_row
) REENT {
    const uint8_t cur_time = timer_read8_ms();
    // Will be set to tru by add_key/del_key
    s_has_updated = false;

    for (uint8_t i = 0; i < bytes_per_row; ++i) {
        uint8_t old_row = g_matrix[row][i];
        uint8_t changed_pins = old_row ^ new_row[i];
        uint8_t pin_mask;
        uint8_t col;

        if (s_is_debouncing[row][i]==0 && !changed_pins) {
            // not debouncing and nothing changed, so nothing to do for this row
            continue;
        }

        pin_mask = 0x01;
        col = i*8;
        for ( ; pin_mask != 0 ; col++, pin_mask<<=1 ) {
            if (s_is_debouncing[row][i] & pin_mask) {
                const uint8_t key_num = get_key_number(row, col);

                // if (key_num == INVALID_KEY_NUMBER) {
                //     continue;
                // }

                // key debouncing:
                // check if the key has finished debouncing
                if (bitmap_get_bit(s_debounce_type, key_num)) {
                    // debouncing key press
                    const uint8_t bounce_duration = (uint8_t)(
                        cur_time - get_debounce_time(key_num)
                    );
                    if (!(old_row & pin_mask)) {
                        // key press not registered yet
                        if (bounce_duration >= g_scan_plan.trigger_time_press) {
                            if (new_row[i] & pin_mask) {
                                // if still down after DEBOUNCE_PRESS_TRIGGER_TIME
                                // register the key press
                                g_matrix[row][i] |= pin_mask;
                                s_matrix_number_keys_down++;
                                scanner_add_matrix_key(key_num);
                            } else {
                                // reject key press and reset debouncing state
                                s_is_debouncing[row][i] &= ~pin_mask;
                                s_matrix_number_keys_debouncing--;
                            }
                        }
                    } else {
                        // key press has already been registered, wait until the
                        // debounce time is over
                        if ( bounce_duration >= g_scan_plan.debounce_time_press) {
                            // debounce time is now over
                            s_is_debouncing[row][i] &= ~pin_mask;
                            s_matrix_number_keys_debouncing--;
                        }
                    }
                } else {
                    // debouncing key release
                    if (new_row[i] & pin_mask) {
                        // key bounced back to the down state, reset timer
                        set_debounce_time(key_num, cur_time);
                    } else {
                        // key in up state
                        const uint8_t bounce_duration = (uint8_t)(
                            cur_time - get_debounce_time(key_num)
                        );

                        // if we have triggered the key release
                        if ((old_row & pin_mask) && bounce_duration >= g_scan_plan.trigger_time_release) {
                            // key has been in the up state for DEBOUNCE_RELEASE_TRIGGER_TIME,
                            // accept that the key has actual been release now
                            g_matrix[row][i] &= ~pin_mask;
                            s_matrix_number_keys_down--;
                            scanner_del_matrix_key(key_num);
                        } else if (bounce_duration >= g_scan_plan.debounce_time_release ) {
                            // debounce over
                            s_is_debouncing[row][i] &= ~pin_mask;
                            s_matrix_number_keys_debouncing--;
                        }
                    }
                }
            } else {
                bool is_key_down;
                uint8_t key_num;
                // not debouncing, so a key was pressed/released.

                if (!(changed_pins & pin_mask)) {
                    // ignore pins in this row that haven't changed
                    continue;
                }

                key_num = get_key_number(row, col);

                // If the key press/release trigger time is 0, then that means
                // we should trigger the key immediately after seeing that it
                // has changed state.
                is_key_down = new_row[i] & pin_mask;
                if (g_scan_plan.trigger_time_press == 0 && is_key_down) {
                    // debounce press trigger time is 0, so register he key press
                    // immediately. The debouncing algorithm then waits until
                    // DEBOUNCE_PRESS_TIME has elapsed before accepting any more
                    // changes in key state.
                    // state.
                    g_matrix[row][i] |= pin_mask;
                    s_matrix_number_keys_down++;
                    scanner_add_matrix_key(key_num);
                } else if (g_scan_plan.trigger_time_release == 0 && !is_key_down) {
                    // debounce release trigger time is 0, so register the key press
                    // immediately. The debouncing algorithm then waits until
                    // DEBOUNCE_RELEASE_TIME has elapsed before accepting any
                    // more changes in key state.
                    g_matrix[row][i] &= ~pin_mask;
                    s_matrix_number_keys_down--;
                    scanner_del_matrix_key(key_num);
                }

                // this pin has changed, so we start it's debounce timer
                if (is_key_down) {
                    bitmap_set_bit(s_debounce_type, key_num); // indicates key press debounce

                } else {
                    bitmap_clear_bit(s_debounce_type, key_num); // indicates key release debounce
                }

                s_is_debouncing[row][i] |= pin_mask;
                set_debounce_time(key_num, cur_time);
                s_matrix_number_keys_debouncing++;
            }
        }
    }

    return s_has_updated;
}

uint8_t get_matrix_num_keys_down(void) {
    return s_matrix_number_keys_down;
}

uint8_t get_matrix_num_keys_debouncing(void) {
    return s_matrix_number_keys_debouncing;
}

#define PASSTHROUGH_BITMAP_SIZE (MAX_NUM_KEYS/8)

void passthrough_keycodes_task(void) REENT {
    if (is_passthrough_enabled() && (s_has_raw_matrix_updated)) {
        // Use `passthrough` as a bitmap for matrix data
        static XRAM uint8_t passthrough_bitmap[PASSTHROUGH_BITMAP_SIZE];
        const uint8_t row_size = INT_DIV_ROUND_UP(g_scan_plan.max_col_pin_num+1, 8);
        uint8_t row, col, logical_col;
        memset(passthrough_bitmap, 0, PASSTHROUGH_BITMAP_SIZE);
        logical_col = 0;

        // walk through `g_matrix` and extract any bits that represent
        // matrix data.
        for (row = 0; row < g_scan_plan.rows; ++row) {
            for (col = 0; col < row_size; ++col) {
                uint8_t matrix_byte = g_matrix[row][col];
                // uint8_t col_mask = io_map_get_col_port_mask(col);
                // uint8_t col_mask = g_col_masks[col];
                uint8_t col_mask = get_col_mask(col);
                uint8_t mask = 0x01;
                while ((uint8_t) mask) {
                    if (col_mask & mask) {
                        if (matrix_byte & mask) {
                            bitmap_set_bit(passthrough_bitmap, logical_col);
                        }
                        logical_col += 1;
                    }
                    mask <<= 1;
                }
            }
        }
        // send the raw matrix data to the host
        queue_vendor_in_packet(
            CMD_PASSTHROUGH_MATRIX,
            (uint8_t*)passthrough_bitmap,
            INT_DIV_ROUND_UP(logical_col, 8),
            STATIC_LENGTH_CMD // TODO: make this a variable?
        );
        s_has_raw_matrix_updated = 0;
    }
}
