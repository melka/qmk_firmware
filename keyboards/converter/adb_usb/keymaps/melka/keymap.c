#include QMK_KEYBOARD_H
#include "keymap_french_osx.h"

#define _BASE 0
#define _FUNC 1

/*
Security Note: While it is possible to use macros to send passwords, credit card numbers,
and other sensitive information it is a supremely bad idea to do so. Anyone who gets a hold
of your keyboard will be able to access that information by opening a text editor.
*/
enum custom_keycodes
{
  PASS_A = SAFE_RANGE,
  PASS_B,
  BSP_DEL
};

/* M0118 Apple Standard Keyboard French ISO
 *                     +-------+
 *                     | power |
 *                     +-------+
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+------+ +---+---+---+---+
 * |esc| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0 | ) | - | bksp | |clr| = | / | * |
 * +----------------------------------------------------------+ +---+---+---+---+
 * | tab | a | z | e | r | t | y | u | i | o | p | ^ | $ |ent | | 7 | 8 | 9 | + |
 * +-----------------------------------------------------+|   | +---+---+---+---+
 * | caps | q | s | d | f | g | h | j | k | l | m | ù | ` |   | | 4 | 5 | 6 | - |
 * +----------------------------------------------------------+ +---+---+---+---+
 * | st | < | w | x | c | v | b | n | , | ; | : | = | rsf |up | | 1 | 2 | 3 |   |
 * +----------------------------------------------------------+ +-------+---|ent|
 * |ctr |opt |comnd |                    | comnd |lef| rig|dwn| |   0   | . |   |
 * +----------------------------------------------------------+ +-------+---+---+
 */

#define LAYOUT_m0118_iso_fr( \
                           K7F,                                                \
   K35,K12,K13,K14,K15,K17,K16,K1A,K1C,K19,K1D,K1B,K18,K33,   K47,K51,K4B,K43, \
   K30,K0C,K0D,K0E,K0F,K11,K10,K20,K22,K1F,K23,K21,K1E,K24,   K59,K5B,K5C,K4E, \
   K39,K00,K01,K02,K03,K05,K04,K26,K28,K25,K29,K27,K2A,       K56,K57,K58,K45, \
   K38,K32,K06,K07,K08,K09,K0B,K2D,K2E,K2B,K2F,K2C,K7B,K3E,   K53,K54,K55,K4C, \
   K36,K3A,K37,            K31,                K3B,K3C,K3D,   K52,    K41      \
) { \
  { K00,   K01,   K02,   K03,   K04,   K05,   K06,   K07   }, \
  { K08,   K09,   K32,   K0B,   K0C,   K0D,   K0E,   K0F   }, \
  { K10,   K11,   K12,   K13,   K14,   K15,   K16,   K17   }, \
  { K18,   K19,   K1A,   K1B,   K1C,   K1D,   K1E,   K1F   }, \
  { K20,   K21,   K22,   K23,   K24,   K25,   K26,   K27   }, \
  { K28,   K29,   K2A,   K2B,   K2C,   K2D,   K2E,   K2F   }, \
  { K30,   K31,   KC_NO,   K33,   KC_NO, K35,   K36,   K37   }, \
  { K38,   K39,   K3A,   K3B,   K3C,   K3D,   K3E,   KC_NO }, \
  { KC_NO, K41,   KC_NO, K43,   KC_NO, K45,   KC_NO, K47   }, \
  { KC_NO, KC_NO, KC_NO, K4B,   K4C,   KC_NO, K4E,   KC_NO }, \
  { KC_NO, K51,   K52,   K53,   K54,   K55,   K56,   K57   }, \
  { K58,   K59,   KC_NO, K5B,   K5C,   KC_NO, KC_NO, KC_NO }, \
  { KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO }, \
  { KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO }, \
  { KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO }, \
  { KC_NO, KC_NO, KC_NO, K7B,   KC_NO, KC_NO, KC_NO, K7F   }  \
}

/* M0487 Apple Standard Keyboard French ISO
 *                     +-------+
 *                     | power |
 *                     +-------+
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+------+ +---+---+---+---+
 * | @ | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0 | ) | - | bksp | |clr| = | / | * |
 * +----------------------------------------------------------+ +---+---+---+---+
 * | tab | a | z | e | r | t | y | u | i | o | p | ^ | $ |ent | | 7 | 8 | 9 | + |
 * +-----------------------------------------------------+|   | +---+---+---+---+
 * | caps | q | s | d | f | g | h | j | k | l | m | ù | ` |   | | 4 | 5 | 6 | - |
 * +----------------------------------------------------------+ +---+---+---+---+
 * | st | < | w | x | c | v | b | n | , | ; | : | = | rsf |up | | 1 | 2 | 3 |   |
 * +----------------------------------------------------------+ +-------+---|ent|
 * |ctr|opt| comnd |                          |esc|lef|rig|dwn| |   0   | . |   |
 * +----------------------------------------------------------+ +-------+---+---+
 */

#define LAYOUT_m0487_iso_fr( \
                           K7F,                                                \
   K0A,K12,K13,K14,K15,K17,K16,K1A,K1C,K19,K1D,K1B,K18,K33,   K47,K51,K4B,K43, \
   K30,K0C,K0D,K0E,K0F,K11,K10,K20,K22,K1F,K23,K21,K1E,K24,   K59,K5B,K5C,K4E, \
   K39,K00,K01,K02,K03,K05,K04,K26,K28,K25,K29,K27,K2A,       K56,K57,K58,K45, \
   K38,K32,K06,K07,K08,K09,K0B,K2D,K2E,K2B,K2F,K2C,K7B,K3E,   K53,K54,K55,K4C, \
   K36,K3A,K37,            K31,        K35,    K3B,K3C,K3D,   K52,    K41      \
) { \
  { K00,   K01,   K02,   K03,   K04,   K05,   K06,   K07   }, \
  { K08,   K09,   K0A,   K0B,   K0C,   K0D,   K0E,   K0F   }, \
  { K10,   K11,   K12,   K13,   K14,   K15,   K16,   K17   }, \
  { K18,   K19,   K1A,   K1B,   K1C,   K1D,   K1E,   K1F   }, \
  { K20,   K21,   K22,   K23,   K24,   K25,   K26,   K27   }, \
  { K28,   K29,   K2A,   K2B,   K2C,   K2D,   K2E,   K2F   }, \
  { K30,   K31,   K32,   K33,   KC_NO, K35,   K36,   K37   }, \
  { K38,   K39,   K3A,   K3B,   K3C,   K3D,   K3E,   KC_NO }, \
  { KC_NO, K41,   KC_NO, K43,   KC_NO, K45,   KC_NO, K47   }, \
  { KC_NO, KC_NO, KC_NO, K4B,   K4C,   KC_NO, K4E,   KC_NO }, \
  { KC_NO, K51,   K52,   K53,   K54,   K55,   K56,   K57   }, \
  { K58,   K59,   KC_NO, K5B,   K5C,   KC_NO, KC_NO, KC_NO }, \
  { KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO }, \
  { KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO }, \
  { KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO }, \
  { KC_NO, KC_NO, KC_NO, K7B,   KC_NO, KC_NO, KC_NO, K7F   }  \
}

/* M0115/M3501 Apple Extended Keyboard French ISO
 * ,---.   ,---------------. ,---------------. ,---------------. ,-----------.             ,---.
 * |Esc|   |F1 |F2 |F3 |F4 | |F5 |F6 |F7 |F8 | |F9 |F10|F11|F12| |PrS|ScL|Pau|             |Pwr|
 * `---'   `---------------' `---------------' `---------------' `-----------'             `---'
 * ,-----------------------------------------------------------. ,-----------. ,---------------.
 * | @ |  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  )|  -|Backspa| |Ins|Hom|PgU| |NmL|  =|  /|  *|
 * |-----------------------------------------------------------| |-----------| |---------------|
 * |Tab  |  A|  Z|  E|  R|  T|  Y|  U|  I|  O|  P|  ^|  $|     | |Del|End|PgD| |  7|  8|  9|  -|
 * |------------------------------------------------------| Ret| `-----------' |---------------|
 * |CapsLo|  Q|  S|  D|  F|  G|  H|  J|  K|  L|  M|  ù|  `|    |               |  4|  5|  6|  +|
 * |-----------------------------------------------------------|     ,---.     |---------------|
 * |Shft|  <|  W|  X|  C|  V|  B|  N|  ,|  ;|  :|  =|Shift     |     |Up |     |  1|  2|  3|   |
 * |-----------------------------------------------------------| ,-----------. |-----------|Ent|
 * |Ctrl |Opt |Cmd |         Space           |     |Opt |Ctrl  | |Lef|Dow|Rig| |      0|  .|   |
 * `-----------------------------------------------------------' `-----------' `---------------'
 */
#define LAYOUT_AEKII_iso_fr( \
    K35,    K7A,K78,K63,K76,K60,K61,K62,K64,K65,K6D,K67,K6F, K69,K6B,K71,              K7F, \
    K0A,K12,K13,K14,K15,K17,K16,K1A,K1C,K19,K1D,K1B,K18,K33, K72,K73,K74,  K47,K51,K4B,K43, \
    K30,K0C,K0D,K0E,K0F,K11,K10,K20,K22,K1F,K23,K21,K1E,K24, K75,K77,K79,  K59,K5B,K5C,K4E, \
    K39,K00,K01,K02,K03,K05,K04,K26,K28,K25,K29,K27,K2A,                   K56,K57,K58,K45, \
    K38,K32,K06,K07,K08,K09,K0B,K2D,K2E,K2B,K2F,K2C,    K7B,     K3E,      K53,K54,K55,     \
    K36,K3A,K37,        K31,                        K7C,K7D, K3B,K3D,K3C,  K52,    K41,K4C  \
) { \
    { K00,   K01,   K02,   K03, K04,   K05,   K06,   K07    }, \
    { K08,   K09,   K0A,   K0B, K0C,   K0D,   K0E,   K0F    }, \
    { K10,   K11,   K12,   K13, K14,   K15,   K16,   K17    }, \
    { K18,   K19,   K1A,   K1B, K1C,   K1D,   K1E,   K1F    }, \
    { K20,   K21,   K22,   K23, K24,   K25,   K26,   K27    }, \
    { K28,   K29,   K2A,   K2B, K2C,   K2D,   K2E,   K2F    }, \
    { K30,   K31,   K32,   K33, KC_NO, K35,   K36,   K37    }, \
    { K38,   K39,   K3A,   K3B, K3C,   K3D,   K3E,   KC_NO, }, \
    { KC_NO, K41,   KC_NO, K43, KC_NO, K45,   KC_NO, K47    }, \
    { KC_NO, KC_NO, KC_NO, K4B, K4C,   KC_NO, K4E,   KC_NO, }, \
    { KC_NO, K51,   K52,   K53, K54,   K55,   K56,   K57    }, \
    { K58,   K59,   KC_NO, K5B, K5C,   KC_NO, KC_NO, KC_NO, }, \
    { K60,   K61,   K62,   K63, K64,   K65,   KC_NO, K67    }, \
    { KC_NO, K69,   KC_NO, K6B, KC_NO, K6D,   KC_NO, K6F    }, \
    { KC_NO, K71,   K72,   K73, K74,   K75,   K76,   K77    }, \
    { K78,   K79,   K7A,   K7B, K7C,   K7D,   KC_NO, K7F    }  \
}

/*const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_m0487_iso_fr(
															 LT(_FUNC, RESET),
		FR_AT,   KC_1,    KC_2,    KC_3,    KC_4,   KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    FR_RPRN, FR_MINS, BSP_DEL,        KC_NLCK, KC_PEQL, KC_PSLS, KC_PAST,
		KC_TAB,  FR_A,    FR_Z,    KC_E,    KC_R,   KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    FR_CIRC, FR_DLR,  KC_ENT,         KC_P7,   KC_P8,   KC_P9,   KC_PMNS,
		KC_LCAP, FR_Q,    KC_S,    KC_D,    KC_F,   KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    FR_M,    FR_UGRV, FR_GRV,                  KC_P4,   KC_P5,   KC_P6,   KC_PPLS,
		KC_LSFT, FR_LESS, FR_W,    KC_X,    KC_C,   KC_V,    KC_B,    KC_N,    FR_COMM, FR_SCLN, FR_COLN, FR_EQL,  KC_RSFT, KC_UP,          KC_P1,   KC_P2,   KC_P3,   KC_PENT,
		KC_LCTL, KC_LALT, KC_LGUI,                                KC_SPC,                        KC_ESC,  KC_LEFT, KC_RIGHT,KC_DOWN,        KC_P0,            KC_PDOT
    ),
    [_FUNC] = LAYOUT_m0487_iso_fr(
															 _______,
		FR_AT,   KC_1,    KC_2,    KC_3,    KC_4,   KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_MUTE, KC_VOLD, KC_VOLU, BSP_DEL,        KC_NLCK, KC_PEQL, KC_PSLS, KC_PAST,
		KC_TAB,  FR_A,    FR_Z,    KC_E,    KC_R,   KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    FR_CIRC, FR_DLR,  KC_ENT,         KC_P7,   KC_P8,   KC_P9,   KC_PMNS,
		KC_LCAP, FR_Q,    KC_S,    KC_D,    KC_F,   KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    FR_M,    FR_UGRV, FR_GRV,                  KC_P4,   KC_P5,   KC_P6,   KC_PPLS,
		KC_LSFT, FR_LESS, FR_W,    KC_X,    KC_C,   KC_V,    KC_B,    KC_N,    FR_COMM, FR_SCLN, FR_COLN, FR_EQL,  KC_RSFT, PASS_A,          KC_P1,   KC_P2,   KC_P3,   KC_PENT,
		KC_LCTL, KC_LALT, KC_LGUI,                                KC_SPC,                        KC_NUBS,  KC_LEFT, KC_RIGHT,PASS_B,        KC_P0,            KC_PDOT
    ),
};*/

/*const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_m0118_iso_fr(
															 LT(_FUNC, RESET),
		KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,   KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    FR_RPRN, FR_MINS, BSP_DEL,        KC_NLCK, KC_PEQL, KC_PSLS, KC_PAST,
		KC_TAB,  FR_A,    FR_Z,    KC_E,    KC_R,   KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    FR_CIRC, FR_DLR,  KC_ENT,         KC_P7,   KC_P8,   KC_P9,   KC_PMNS,
		KC_LCAP, FR_Q,    KC_S,    KC_D,    KC_F,   KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    FR_M,    FR_UGRV, FR_GRV,                  KC_P4,   KC_P5,   KC_P6,   KC_PPLS,
		KC_LSFT, KC_GRV,  FR_W,    KC_X,    KC_C,   KC_V,    KC_B,    KC_N,    FR_COMM, FR_SCLN, FR_COLN, FR_EQL,  KC_RSFT, KC_UP,          KC_P1,   KC_P2,   KC_P3,   KC_PENT,
		KC_LCTL, KC_LALT, KC_LGUI,                                KC_SPC,                                 KC_LEFT, KC_RIGHT,KC_DOWN,        KC_P0,            KC_PDOT
    ),
    [_FUNC] = LAYOUT_m0118_iso_fr(
															 _______,
		FR_LABK, KC_1,    KC_2,    KC_3,    KC_4,   KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_MUTE, KC_VOLD, KC_VOLU, BSP_DEL,        KC_NLCK, KC_PEQL, KC_PSLS, KC_PAST,
		KC_TAB,  FR_A,    FR_Z,    KC_E,    KC_R,   KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    FR_CIRC, FR_DLR,  KC_ENT,         KC_P7,   KC_P8,   KC_P9,   KC_PMNS,
		KC_LCAP, FR_Q,    KC_S,    KC_D,    KC_F,   KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    FR_M,    FR_UGRV, FR_GRV,                  KC_P4,   KC_P5,   KC_P6,   KC_PPLS,
		KC_LSFT, KC_GRV,  FR_W,    KC_X,    KC_C,   KC_V,    KC_B,    KC_N,    FR_COMM, FR_SCLN, FR_COLN, FR_EQL,  KC_RSFT, PASS_A,         KC_P1,   KC_P2,   KC_P3,   KC_PENT,
		KC_LCTL, KC_LALT, KC_LGUI,                                KC_SPC,                                 KC_LEFT, KC_RIGHT,PASS_B,         KC_P0,            KC_PDOT
    ),
};*/

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_AEKII_iso_fr(
        KC_ESC,  KC_BRID, KC_BRIU,KC_F3,  KC_F4,  KC_F5,  KC_F6,  KC_MPRV,KC_MPLY, KC_MNXT,KC_MUTE, KC_VOLD, KC_VOLU,                KC_PSCR,KC_SLCK, KC_PAUS,                               LT(_FUNC, RESET),
        FR_AT,   KC_1,    KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   KC_8,    KC_9,   KC_0,    FR_RPRN, FR_MINS, BSP_DEL,       KC_INS, KC_HOME, KC_PGUP,     KC_NLCK, KC_EQL, KC_PSLS, KC_PAST,
        KC_TAB,  FR_A,    FR_Z,   KC_E,   KC_R,   KC_T,   KC_Y,   KC_U,   KC_I,    KC_O,   KC_P,    FR_CIRC, FR_DLR,  KC_ENT,        KC_DEL, KC_END,  KC_PGDN,     KC_P7,   KC_P8,  KC_P9,   KC_PMNS,
        KC_LCAP, FR_Q,    KC_S,   KC_D,   KC_F,   KC_G,   KC_H,   KC_J,   KC_K,    KC_L,   FR_M,    FR_UGRV,          FR_GRV,                                      KC_P4,   KC_P5,  KC_P6,   KC_PPLS,
        KC_LSFT, FR_LESS, FR_W,   KC_X,   KC_C,   KC_V,   KC_B,   KC_N,   FR_COMM, FR_SCLN,FR_COLN, FR_EQL,           KC_RSFT,                KC_UP,               KC_P1,   KC_P2,  KC_P3,
        KC_LCTL, KC_LALT, KC_LGUI,                KC_SPC,                                  KC_RALT, KC_RCTL,                         KC_LEFT, KC_DOWN, KC_RGHT,    KC_P0,           KC_PDOT, KC_PENT
    ),
    [_FUNC] = LAYOUT_AEKII_iso_fr(
        KC_ESC,  KC_BRID, KC_BRIU,KC_F3,  KC_F4,  KC_F5,  KC_F6,  KC_MPRV,KC_MPLY, KC_MNXT,KC_MUTE, KC_VOLD, KC_VOLU,                KC_PSCR,KC_SLCK, KC_PAUS,                               KC_EJCT,
        FR_AT,   KC_1,    KC_2,   KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   KC_8,    KC_9,   KC_0,    FR_RPRN, FR_MINS, BSP_DEL,       KC_INS, KC_HOME, KC_PGUP,     KC_NLCK, KC_EQL, KC_PSLS, KC_PAST,
        KC_TAB,  FR_A,    FR_Z,   KC_E,   KC_R,   KC_T,   KC_Y,   KC_U,   KC_I,    KC_O,   KC_P,    FR_CIRC, FR_DLR,  KC_ENT,        KC_DEL, KC_END,  KC_PGDN,     KC_P7,   KC_P8,  KC_P9,   KC_PMNS,
        KC_LCAP, FR_Q,    KC_S,   KC_D,   KC_F,   KC_G,   KC_H,   KC_J,   KC_K,    KC_L,   FR_M,    FR_UGRV,          FR_GRV,                                      KC_P4,   KC_P5,  KC_P6,   KC_PPLS,
        KC_LSFT, FR_LESS, FR_W,   KC_X,   KC_C,   KC_V,   KC_B,   KC_N,   FR_COMM, FR_SCLN,FR_COLN, FR_EQL,           KC_RSFT,                PASS_A,              KC_P1,   KC_P2,  KC_P3,
        KC_LCTL, KC_LALT, KC_LGUI,                KC_SPC,                                  KC_RALT, KC_RCTL,                         KC_LEFT, PASS_B, KC_RGHT,     KC_P0,           KC_PDOT, KC_PENT
    ),
};

void matrix_init_user(void) {
    #ifndef NO_DEBUG
    debug_enable = true;
    debug_matrix = true;
    debug_keyboard = true;
    debug_mouse = true;
    print("debug enabled.\n");
    #endif
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  static uint8_t saved_mods   = 0;

  switch (keycode) {
    case BSP_DEL:
        if (record->event.pressed) {
            saved_mods = get_mods() & MOD_BIT(KC_LCTRL);

            if (saved_mods == MOD_BIT(KC_LCTRL)) {
                del_mods(saved_mods);
                register_code(KC_DEL);
                add_mods(saved_mods);
            } else {
                register_code(KC_BSPC);
            }
        } else {
            unregister_code(KC_DEL);
            unregister_code(KC_BSPC);
        }
        return false;
    case PASS_A:
        if (record->event.pressed) {
            SEND_STRING(PASSWORD_A "\n");
        }
        return false;
    case PASS_B:
        if (record->event.pressed) {
            SEND_STRING(PASSWORD_B "\n");
        }
        return false;
    default:
        return true;
  }

  return true;
}
