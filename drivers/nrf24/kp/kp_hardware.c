// Copyright 2017 jem@seethis.link
// Licensed under the MIT license (http://opensource.org/licenses/MIT)

#include "kp_hardware.h"
#include "kp_aes.h"
#include "kp_rf.h"
#include "kp_settings.h"
#include "kp_matrix_scanner.h"
#include "kp_led.h"
#include "kp_usb_commands.h"
#include "kp_error.h"

#include "hid_reports/hid_reports.h"

#if USE_I2C
#include "wired.h"
#endif

bit_t g_slow_clock_mode = 1;

#if (USE_I2C || USE_NRF24) && USE_USB
bit_t g_has_usb_port;
#endif

void software_reset(void) {
    init_error_system();
    settings_load_from_flash();
#if USE_SCANNER
    io_map_init();
    matrix_scanner_init();
#endif
#if USE_NRF24
    aes_key_init(g_rf_settings.ekey, g_rf_settings.dkey);

    if (!g_runtime_settings.feature.ctrl.rf_disabled) {
        rf_init_receive();
    }
#endif
#if USE_I2C
    if (g_has_usb_port && !g_runtime_settings.feature.ctrl.wired_disabled) {
        i2c_init();
    }
#endif
    // led_init();
#if USE_USB
    reset_usb_reports();
#else
    reset_hid_reports();
#endif
    // keyboards_init();
}
