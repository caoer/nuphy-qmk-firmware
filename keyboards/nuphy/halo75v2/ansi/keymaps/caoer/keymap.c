// Copyright 2024 caoer - Custom keymap with Nav Layers
// Based on NuPhy Halo75 V2 VIA keymap by Evgeny Kapusta (@ryodeushii)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ansi.h"
#include "keycodes.h"
#include QMK_KEYBOARD_H

// ============================================
// PUNCTUATION MODS - Right hand modifiers (mirrors left hand)
// ============================================
// ; = Cmd when held (mirrors A)
// ' = Ctrl when held
// / = Alt when held
// , = Hyper when held (except comma+space = ", ")
//
// Hyper = Cmd+Shift+Alt (3 modifiers, no Ctrl)
// True Hyper: all 4 mods. No Carbon superset exists → safest for macOS hotkeys.
#define MY_HYPR(kc)   LCTL(LSFT(LALT(LGUI(kc))))
#define MY_HYPR_T(kc) MT(MOD_LCTL | MOD_LSFT | MOD_LALT | MOD_LGUI, kc)
#define MY_HYPR_MODS  (MOD_BIT(KC_LCTL) | MOD_BIT(KC_LSFT) | MOD_BIT(KC_LALT) | MOD_BIT(KC_LGUI))

#define MOD_SCLN RGUI_T(KC_SCLN)  // ; = Cmd
#define MOD_QUOT RCTL_T(KC_QUOT)  // ' = Ctrl
#define MOD_SLSH LALT_T(KC_SLSH)  // / = Alt

// ============================================
// CAPS LOCK = Ctrl (hold) / Esc (tap)
// ============================================
// Ctrl+HJKL → arrow keys via Key Overrides (all other Ctrl combos pass through)
// Key Override has GUI negmod: Hyper (includes Ctrl) + HJKL sends letter, not arrow
#define CAPS_NAV LCTL_T(KC_ESC)

// All alpha keys are plain keys for clean fast typing

// ============================================
// CUSTOM KEYCODE: Comma with Hyper (space exception)
// ============================================
// Use QK_USER range for keymap-specific custom keycodes
#define COMM_HYP (QK_USER + 0)  // , = Hyper when held, but comma+space = ", "

// ============================================
// SPACE NAV CTRL+HJKL: Custom keycodes for vim-tmux-navigator
// ============================================
// Space+HJKL → Ctrl+HJKL (pane switching). Uses register_code() directly
// to bypass Key Override (which would convert Ctrl+H to arrow on Layer 0).
#define CTRL_H (QK_USER + 1)
#define CTRL_J (QK_USER + 2)
#define CTRL_K (QK_USER + 3)
#define CTRL_L (QK_USER + 4)

// ============================================
// HYPER KEY: Pure modifier, no mod-tap machinery
// ============================================
// MY_HYPR_T(KC_NO) caused logout on double-press — KC_NO (0x00) through
// NuPhy firmware's mod-tap path sends stray keycodes. This custom keycode
// bypasses mod-tap entirely: hold = Hyper mods, release = clean unregister.
#define HYPR_KEY (QK_USER + 5)

// ============================================
// SPACE NAV LAYER (Layer 6) — RETIRED (scheme A+)
// ============================================
// Space-Fn is gone on EVERY base layer (advisor ruling 5): layer 0 and layer 2
// both carry a plain KC_SPC, so LT(6, KC_SPC) has no user left and the NAV_SPC
// macro is removed along with the per-key tap/hold tuning it needed.

// ============================================
// TAP DANCE DEFINITIONS
// ============================================
enum {
    TD_F3_MCTL,   // Tap = F3, Hold = Mission Control
    TD_F10_MUTE,  // Tap = F10, Hold = Mute
};

typedef struct {
    bool is_press_action;
    uint8_t state;
} tap_dance_tap_hold_t;

enum {
    TD_NONE,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
};

static uint8_t dance_state(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        else return TD_SINGLE_HOLD;
    }
    return TD_NONE;
}

// F3 tap dance: tap = F3, hold = Mission Control (Ctrl+Up)
static tap_dance_tap_hold_t td_f3_state = {false, TD_NONE};

void td_f3_finished(tap_dance_state_t *state, void *user_data) {
    td_f3_state.state = dance_state(state);
    switch (td_f3_state.state) {
        case TD_SINGLE_TAP:  register_code(KC_F3); break;
        case TD_SINGLE_HOLD:
            register_code(KC_LCTL);
            register_code(KC_UP);
            break;
    }
}

void td_f3_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_f3_state.state) {
        case TD_SINGLE_TAP:  unregister_code(KC_F3); break;
        case TD_SINGLE_HOLD:
            unregister_code(KC_UP);
            unregister_code(KC_LCTL);
            break;
    }
    td_f3_state.state = TD_NONE;
}

// F10 tap dance: tap = F10, hold = Mute
static tap_dance_tap_hold_t td_f10_state = {false, TD_NONE};

void td_f10_finished(tap_dance_state_t *state, void *user_data) {
    td_f10_state.state = dance_state(state);
    switch (td_f10_state.state) {
        case TD_SINGLE_TAP:  register_code(KC_F10); break;
        case TD_SINGLE_HOLD: register_code(KC_MUTE); break;
    }
}

void td_f10_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_f10_state.state) {
        case TD_SINGLE_TAP:  unregister_code(KC_F10); break;
        case TD_SINGLE_HOLD: unregister_code(KC_MUTE); break;
    }
    td_f10_state.state = TD_NONE;
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_F3_MCTL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_f3_finished, td_f3_reset),
    [TD_F10_MUTE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_f10_finished, td_f10_reset),
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
// ============================================
// LAYER 0: Mac Base
// ============================================
// Right column = F18 chords; WezTerm expands them to herdr chords (see common.lua).
// macOS drops F21-F24 (virtual keycodes end at kVK_F20) and PC-Insert — all
// slots ride the proven F18 transport with modifiers instead.
//   DEL=F18→prefix+z zoom | HOME=Alt+F18→alt+shift+k | END=Cmd+F18→alt+shift+j
//   PGUP=Shift+F18→alt+up | PGDN=Ctrl+F18→alt+down
//   PRTSC=Cmd+Alt+F18→prefix+b toggle sidebar (was Cmd+Shift+5 screenshot)
//   INS=Ctrl+Shift+F18→alt+1 (focus_agent rank 1)
// Scheme A+: Space is a plain KC_SPC again (was NAV_SPC = LT(6, KC_SPC)) — the
// Space-Fn layer is retired, see Layer 6 below. Caps, the comma Hyper producer
// (COMM_HYP) and the left-Ctrl Hyper producer (HYPR_KEY) are unchanged.
[0] = LAYOUT_ansi_84(
    KC_ESC,  KC_F1,    KC_F2,    TD(TD_F3_MCTL), KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, TD(TD_F10_MUTE), KC_F11, KC_F12, G(A(KC_F18)), C(S(KC_F18)), KC_F18,
    KC_GRV,  KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,           A(KC_F18),
    KC_TAB,  KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,           G(KC_F18),
    CAPS_NAV, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, MOD_SCLN, MOD_QUOT, KC_ENT,                     S(KC_F18),
    KC_LSFT,           KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     COMM_HYP, KC_DOT,  MOD_SLSH, KC_RSFT,           KC_UP,   C(KC_F18),
    HYPR_KEY, KC_LOPT,  KC_LCMD,                                KC_SPC,                               KC_RCMD, MO(1),             KC_LEFT, KC_DOWN, KC_RIGHT),

// ============================================
// LAYER 1: Mac Fn (media keys on F-row)
// ============================================
[1] = LAYOUT_ansi_84(
    _______, KC_BRMD,  KC_BRMU,  MAC_TASK, MAC_SEARCH, MAC_VOICE, MAC_DND, KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, MAC_PRT, TOG_POWER_ON_ANIMATION, _______,
    _______, LNK_BLE1, LNK_BLE2, LNK_BLE3, LNK_RF,   _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______,           _______,
    _______, _______,  _______,  _______,  _______,  _______,  DEBOUNCE_PRESS_DEC, DEBOUNCE_PRESS_SHOW, DEBOUNCE_PRESS_INC, TOG_DEEP_SLEEP, TOG_USB_SLP, DEV_RESET, SLEEP_MODE, BAT_SHOW, _______,
    TOG_CAPS_IND, SLEEP_TIMEOUT_DEC, SLEEP_TIMEOUT_SHOW, SLEEP_TIMEOUT_INC, _______, _______, DEBOUNCE_RELEASE_DEC, DEBOUNCE_RELEASE_SHOW, DEBOUNCE_RELEASE_INC, _______, _______, _______, _______,           _______,
    _______,           _______,  _______,  RGB_TEST, _______,  _______,  _______,  MO(4),    RM_SPDD, RM_SPDU, _______, _______,           RM_VALU, _______,
    _______, _______,  _______,                                _______,                               _______, MO(1),             RM_NEXT, RM_VALD, RM_HUEU),

// ============================================
// LAYER 2: Windows Base (same right-column F18-chord transport as Mac)
// ============================================
// Scheme A+ (advisor ruling 5): Space is a plain KC_SPC here too — it was
// NAV_SPC = LT(6, KC_SPC), which after Layer 6 was emptied only bought a 150 ms
// tap/hold wait before an all-transparent layer. No Space layer-tap remains on
// any base layer.
[2] = LAYOUT_ansi_84(
    KC_ESC,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,   KC_F10,  KC_F11,  KC_F12,  WIN_PRTA, C(S(KC_F18)), KC_F18,
    KC_GRV,  KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,           A(KC_F18),
    KC_TAB,  KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,           G(KC_F18),
    CAPS_NAV, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, MOD_SCLN, MOD_QUOT, KC_ENT,                     S(KC_F18),
    KC_LSFT,           KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     COMM_HYP, KC_DOT,  MOD_SLSH, KC_RSFT,           KC_UP,   C(KC_F18),
    MY_HYPR_T(KC_NO), KC_LWIN,  KC_LALT,                                KC_SPC,                               KC_RALT, MO(3),             KC_LEFT, KC_DOWN, KC_RIGHT),

// ============================================
// LAYER 3: Windows Fn (unchanged from original)
// ============================================
[3] = LAYOUT_ansi_84(
    _______, KC_BRID,  KC_BRIU,  SOCDON,   SOCDOFF,  SOCDTOG,  _______,  KC_MPRV,  KC_MPLY,  KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU, KC_PSCR, TOG_POWER_ON_ANIMATION, _______,
    _______, LNK_BLE1, LNK_BLE2, LNK_BLE3, LNK_RF,   _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______,           _______,
    _______, _______,  _______,  _______,  _______,  _______,  DEBOUNCE_PRESS_DEC, DEBOUNCE_PRESS_SHOW, DEBOUNCE_PRESS_INC, TOG_DEEP_SLEEP, TOG_USB_SLP, DEV_RESET, SLEEP_MODE, BAT_SHOW, _______,
    TOG_CAPS_IND, SLEEP_TIMEOUT_DEC, SLEEP_TIMEOUT_SHOW, SLEEP_TIMEOUT_INC, _______, _______, DEBOUNCE_RELEASE_DEC, DEBOUNCE_RELEASE_SHOW, DEBOUNCE_RELEASE_INC, _______, _______, _______, _______,           _______,
    _______,           _______,  _______,  RGB_TEST, _______,  _______,  _______,  MO(4),    RM_SPDD, RM_SPDU, _______, _______,           RM_VALU, _______,
    _______, _______,  _______,                                _______,                               _______, MO(3),             RM_NEXT, RM_VALD, RM_HUEU),

// ============================================
// LAYER 4: Side lighting controls (unchanged)
// ============================================
[4] = LAYOUT_ansi_84(
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______, _______, _______,
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______,          _______,
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______,          _______,
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______,                   _______,
    _______,           _______,  _______,  _______,  _______,  _______,  _______,  _______,  SIDE_SPD,SIDE_SPI,SIDE_MOD, _______,         SIDE_VAI,_______,
    _______, _______,  _______,                                _______,                               _______, MO(4),             SIDE_MOD, SIDE_VAD,SIDE_HUI),

// ============================================
// LAYER 5: Reserved
// ============================================
[5] = LAYOUT_ansi_84(
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______, _______, _______,
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______,          _______,
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______,          _______,
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______,                   _______,
    _______,           _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______,          _______, _______,
    _______, _______,  _______,                                _______,                               _______, _______,          _______, _______, _______),

// ============================================
// LAYER 6: RETIRED — Space Nav removed (scheme A+)
// ============================================
// Scheme A+ (candidate-schemes.md § 1+ lines 470-473): Space-Fn is gone in full.
// Layer 0's Space is a plain KC_SPC again, so nothing reaches this layer from the
// Mac base layer; every position is transparent. The layer is kept (rather than
// deleted) to preserve layer numbering for layers 7 and the Windows base layer.
//
// What used to live here, and where the function went under A+:
//   YUIO = Home/PgUp/PgDn/End      → Hyper+Y/U/I/O
//   HJKL = Ctrl+HJKL               → retired; Caps+HJKL stays arrows (A+ keeps the
//                                    Ctrl+HJKL→arrow overrides; the four consumers rebind)
//   R = F13 (Raycast)              → Hyper+Space       W = F19 (WezTerm dialog) → Hyper+Q
//   Z = F20 (slow paste)           → Hyper+Z           X = Hyper+M (maximize)   → unchanged mask
//   E/D = Hyper+E / Hyper+D        → unchanged mask
//   G = F16 (window mode), C = F17 (float cycle), A/S/V = Hyper+A/S/V → dropped
// ============================================
[6] = LAYOUT_ansi_84(
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______, _______, _______,
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______,          _______,
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______,          _______,
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______,                   _______,
    _______,           _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______,          _______, _______,
    _______, _______,  _______,                                _______,                               _______, _______,          _______, _______, _______),

// ============================================
// LAYER 7: Reserved (display focus sublayer removed — AeroSpace gone)
// Kept as transparent passthrough to preserve layer numbering.
// ============================================
[7] = LAYOUT_ansi_84(
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______, _______, _______,
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______,          _______,
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______,          _______,
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______,                   _______,
    _______,           _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______,          _______, _______,
    _______, _______,  _______,                                _______,                               _______, _______,          _______, _______, _______),
};
// clang-format on

// ============================================
// KEY OVERRIDES: Ctrl+HJKL → Arrow keys
// ============================================
// When Caps (Ctrl) + HJKL is pressed, emit arrow keys instead.
// Additional modifiers (Shift, Cmd, Alt) pass through naturally.
// e.g. Caps+Shift+H = Shift+Left, Caps+Shift+Cmd+H = Shift+Cmd+Left
//
// Negative mod GUI: suppresses override when Cmd is held, so
// Hyper+HJKL (which includes Ctrl) sends Hyper+letter, not Hyper+arrow.
//
// Report ordering (2026-09-13): stock key overrides put "Ctrl released" and
// "Left pressed" into ONE HID report and trust the host to read the modifier
// byte first. macOS with Karabiner grabbing the board (seen over Bluetooth)
// processes the key first, so the app receives Ctrl+Left instead of Left.
// custom_action runs after the Ctrl suppression is set and before the arrow
// is added, so flushing a report there makes the release its own packet.
// Deactivation ("Left up" + "Ctrl back") is left alone: key-first order is
// harmless there.
static bool nav_flush_ctrl(bool activated, void *context) {
    (void)context;
    if (activated) {
        send_keyboard_report();
        wait_ms(5); // let USB / the wireless module ship it before the next report
    }
    return true;
}

// ko_make_with_layers_negmods_and_options(MOD_MASK_CTRL, key, arrow, ~0, MOD_MASK_GUI, ko_options_default)
// with .custom_action wired — the helper macro hardcodes it to NULL.
#define NAV_KO(trigger_key, replacement_key)      \
    ((const key_override_t){                      \
        .trigger_mods      = MOD_MASK_CTRL,       \
        .layers            = ~0,                  \
        .suppressed_mods   = MOD_MASK_CTRL,       \
        .options           = ko_options_default,  \
        .negative_mod_mask = MOD_MASK_GUI,        \
        .custom_action     = nav_flush_ctrl,      \
        .context           = NULL,                \
        .trigger           = (trigger_key),       \
        .replacement       = (replacement_key),   \
        .enabled           = NULL,                \
    })

const key_override_t nav_h = NAV_KO(KC_H, KC_LEFT);
const key_override_t nav_j = NAV_KO(KC_J, KC_DOWN);
const key_override_t nav_k = NAV_KO(KC_K, KC_UP);
const key_override_t nav_l = NAV_KO(KC_L, KC_RGHT);

const key_override_t *key_overrides[] = {
    &nav_h, &nav_j, &nav_k, &nav_l,
    NULL
};

// KC_F16 on Layer 6 G position — ghost-release tracking.
// Without tracking, releasing Space before G leaves F16 stuck forever.
static bool     layer6_f16_active = false;

// Ghost-release tracking for Space Nav (Layer 6) Ctrl+HJKL keys.
// If Space released before H/J/K/L, layer reverts and release arrives as KC_H etc.
static uint16_t layer6_ctrl_active_kc = 0;

// ============================================
// COMMA-HYPER: Custom handling for comma+space exception
// ============================================
static bool comm_hyp_held = false;
static uint16_t comm_hyp_timer = 0;
static bool comm_hyp_activated = false;  // Was Hyper activated?

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case COMM_HYP:
            if (record->event.pressed) {
                comm_hyp_held = true;
                comm_hyp_timer = timer_read();
                comm_hyp_activated = false;
            } else {
                // Key released
                if (comm_hyp_activated) {
                    // Was used as Hyper, unregister all mods
                    unregister_mods(MY_HYPR_MODS);
                } else if (comm_hyp_held) {
                    // Plain tap: nothing consumed the comma while it was down
                    tap_code(KC_COMM);
                }
                // else: the ", " roll below already sent the comma on Space
                // press and cleared comm_hyp_held — sending it again here
                // typed ", ," whenever Space went down before comma came up.
                comm_hyp_held = false;
                comm_hyp_activated = false;
            }
            return false;  // We handle this key completely

        case KC_SPC:
            if (record->event.pressed && comm_hyp_held && !comm_hyp_activated) {
                // Special case: comma + space = ", " (not Hyper+Space)
                tap_code(KC_COMM);
                comm_hyp_held = false;
                // Space continues normally (nav layer or regular space)
            }
            return true;

        // KC_F16 on Layer 6 G position — track press/release for cleanup.
        case KC_F16:
            if (record->event.pressed) {
                if (IS_LAYER_ON(6)) {
                    layer6_f16_active = true;
                }
            } else {
                layer6_f16_active = false;
            }
            return true;

        // Ghost release: Space released before G — layer reverted to 0,
        // so G's release arrives as KC_G instead of KC_F16.
        // Same class of bug as NAV_H/J/K/L ghost releases.
        case KC_G:
            if (!record->event.pressed && layer6_f16_active) {
                unregister_code(KC_F16);
                layer6_f16_active = false;
                return false;
            }
            return true;

        // ============================================
        // CTRL+HJKL: Space Nav vim-tmux-navigator keys
        // ============================================
        // Uses register_code() directly to bypass Key Override
        // (which would convert Ctrl+H to Left arrow on Layer 0).
        case CTRL_H:
        case CTRL_J:
        case CTRL_K:
        case CTRL_L: {
            uint16_t kc;
            switch (keycode) {
                case CTRL_H: kc = KC_H; break;
                case CTRL_J: kc = KC_J; break;
                case CTRL_K: kc = KC_K; break;
                default:     kc = KC_L; break;
            }
            if (record->event.pressed) {
                // Clean up any prior stuck state
                if (layer6_ctrl_active_kc) {
                    unregister_code(layer6_ctrl_active_kc);
                    unregister_mods(MOD_BIT(KC_LCTL));
                }
                register_mods(MOD_BIT(KC_LCTL));
                register_code(kc);
                layer6_ctrl_active_kc = kc;
            } else {
                unregister_code(kc);
                unregister_mods(MOD_BIT(KC_LCTL));
                layer6_ctrl_active_kc = 0;
            }
            return false;
        }

        // Ghost releases — Space released before HJKL,
        // layer reverted to 0, release arrives as plain alpha.
        case KC_H:
        case KC_J:
        case KC_K:
        case KC_L:
            if (!record->event.pressed && layer6_ctrl_active_kc == keycode) {
                unregister_code(keycode);
                unregister_mods(MOD_BIT(KC_LCTL));
                layer6_ctrl_active_kc = 0;
                return false;
            }
            return true;

        // Pure Hyper key — no mod-tap, no tap keycode, no stray events.
        case HYPR_KEY:
            if (record->event.pressed) {
                register_mods(MY_HYPR_MODS);
            } else {
                unregister_mods(MY_HYPR_MODS);
            }
            return false;

        default:
            if (record->event.pressed && comm_hyp_held && !comm_hyp_activated) {
                // Any other key pressed while holding comma -> activate Hyper
                register_mods(MY_HYPR_MODS);
                comm_hyp_activated = true;
            }
            return true;
    }
}

// Check for comma held past tapping term (becomes Hyper without needing another key)
void matrix_scan_user(void) {
    if (comm_hyp_held && !comm_hyp_activated && timer_elapsed(comm_hyp_timer) > TAPPING_TERM) {
        register_mods(MY_HYPR_MODS);
        comm_hyp_activated = true;
    }
}

// ============================================
// LAYER CHANGE: Clean up ghost keys when Layer 6 deactivates
// ============================================
layer_state_t layer_state_set_user(layer_state_t state) {
    // Clean up Space Nav (Layer 6) ghost keys
    if (!layer_state_cmp(state, 6)) {
        if (layer6_ctrl_active_kc) {
            unregister_code(layer6_ctrl_active_kc);
            unregister_mods(MOD_BIT(KC_LCTL));
            layer6_ctrl_active_kc = 0;
        }
        if (layer6_f16_active) {
            unregister_code(KC_F16);
            layer6_f16_active = false;
        }
    }
    return state;
}

// ============================================
// PER-KEY TAPPING TERM / QUICK TAP TERM — REMOVED (scheme A+)
// ============================================
// Both overrides existed only for NAV_SPC (Space's 150 ms tapping term and its
// quick-tap disable). With Space plain on every base layer their sole case is
// gone and each function would return nothing but the global default, so both
// are removed: Caps Nav and the punctuation mod-taps keep TAPPING_TERM (200 ms)
// and QUICK_TAP_TERM (120 ms) exactly as before.

// ============================================
// SUSPEND/WAKEUP: Reset all custom state
// ============================================
// QMK's clear_keyboard() clears HID report on suspend but does NOT
// reset user static variables. Without this, state survives USB/BT
// profile switches — nav_repeat fires ghost keypresses on reconnect.
void suspend_wakeup_init_user(void) {
    layer6_f16_active = false;
    layer6_ctrl_active_kc = 0;
    comm_hyp_held = false;
    comm_hyp_activated = false;
}

// ============================================
// PER-KEY PERMISSIVE HOLD
// ============================================
// Enabled for mod-tap keys (faster modifier activation when rolling).
// Caps Nav enabled: deliberate hold should activate layer immediately.
// The NAV_SPC exception is gone with Space-Fn — Space is no longer a mod-tap.
bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        default:
            return true;   // Caps Nav, punctuation mods: activate on rolls
    }
}
