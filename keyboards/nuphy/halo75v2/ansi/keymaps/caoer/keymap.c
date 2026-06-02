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
// HJKL arrow navigation via Key Overrides (Ctrl+HJKL → arrows)
// All other Ctrl combos work naturally (Ctrl+C, Ctrl+V, etc.)
#define CAPS_NAV LCTL_T(KC_ESC)

// All alpha keys are plain keys for clean fast typing

// ============================================
// CUSTOM KEYCODE: Comma with Hyper (space exception)
// ============================================
// Use QK_USER range for keymap-specific custom keycodes
#define COMM_HYP (QK_USER + 0)  // , = Hyper when held, but comma+space = ", "

// ============================================
// NAV HJKL: Custom keycodes for Caps+Space differentiation
// ============================================
// Space+HJKL → Ctrl+Alt+HJKL (focus)
// Caps+Space+HJKL → Ctrl+Alt+Shift+HJKL (move) — Caps Ctrl converted to Shift
// Shift+Space+HJKL → Ctrl+Shift+HJKL (join-with) — Shift replaces base, drops Alt
// Option+Space+HJKL → Cmd+Shift+Alt+HJKL (swap) — Hyper (no Ctrl)
#define NAV_H (QK_USER + 1)
#define NAV_J (QK_USER + 2)
#define NAV_K (QK_USER + 3)
#define NAV_L (QK_USER + 4)

// ============================================
// HYPER KEY: Pure modifier, no mod-tap machinery
// ============================================
// MY_HYPR_T(KC_NO) caused logout on double-press — KC_NO (0x00) through
// NuPhy firmware's mod-tap path sends stray keycodes. This custom keycode
// bypasses mod-tap entirely: hold = Hyper mods, release = clean unregister.
#define HYPR_KEY (QK_USER + 5)

// ============================================
// SPACE NAV LAYER (Layer 6)
// ============================================
// Space = Nav layer 6 when held, Space when tapped
// Layer 6: YUIO = Home/PgUp/PgDn/End, HJKL = NAV custom keycodes, app launchers
#define NAV_SPC LT(6, KC_SPC)

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
[0] = LAYOUT_ansi_84(
    KC_ESC,  KC_F1,    KC_F2,    TD(TD_F3_MCTL), KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, TD(TD_F10_MUTE), KC_F11, KC_F12, MAC_PRTA, KC_INS, MY_HYPR(KC_N),
    KC_GRV,  KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,           MY_HYPR(KC_M),
    KC_TAB,  KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,           MY_HYPR(KC_COMM),
    CAPS_NAV, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, MOD_SCLN, MOD_QUOT, KC_ENT,                     MY_HYPR(KC_DOT),
    KC_LSFT,           KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     COMM_HYP, KC_DOT,  MOD_SLSH, KC_RSFT,           KC_UP,   MY_HYPR(KC_SLSH),
    HYPR_KEY, KC_LOPT,  KC_LCMD,                                NAV_SPC,                              KC_RCMD, MO(1),             KC_LEFT, KC_DOWN, KC_RIGHT),

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
// LAYER 2: Windows Base
// ============================================
[2] = LAYOUT_ansi_84(
    KC_ESC,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,   KC_F10,  KC_F11,  KC_F12,  WIN_PRTA, KC_INS,  MY_HYPR(KC_N),
    KC_GRV,  KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,           MY_HYPR(KC_M),
    KC_TAB,  KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,           MY_HYPR(KC_COMM),
    CAPS_NAV, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, MOD_SCLN, MOD_QUOT, KC_ENT,                     MY_HYPR(KC_DOT),
    KC_LSFT,           KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     COMM_HYP, KC_DOT,  MOD_SLSH, KC_RSFT,           KC_UP,   MY_HYPR(KC_SLSH),
    MY_HYPR_T(KC_NO), KC_LWIN,  KC_LALT,                                NAV_SPC,                              KC_RALT, MO(3),             KC_LEFT, KC_DOWN, KC_RIGHT),

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
// LAYER 5: Reserved (Caps Nav moved to Key Overrides)
// Kept as transparent passthrough to preserve layer numbering
// ============================================
[5] = LAYOUT_ansi_84(
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______, _______, _______,
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______,          _______,
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______,          _______,
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______,                   _______,
    _______,           _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______,          _______, _______,
    _______, _______,  _______,                                _______,                               _______, _______,          _______, _______, _______),

// ============================================
// LAYER 6: Space Nav (Hold Space)
// Right hand: YUIO = Home/PgUp/PgDn/End, HJKL = NAV custom (AeroSpace focus/move/join/swap)
// Left hand:  App launchers via MY_HYPR(KC_x) → Hammerspoon toggleApp
//             R = F13 (Raycast), W = F19 (WezTerm dialog), Tab = LCA(Tab) → WS toggle
//             F = MO(7) → display focus sublayer (Space+F+HJKL = Cmd+Alt+HJKL)
//             C = F17 (float cycle), X = F18 (float fullscreen), Z = F20 (slow paste)
// WM:         1-9 = LCA(KC_1-9) → workspace switch
//             G = KC_F16 → window mode modal trigger (Hammerspoon)
// Hyper cluster (top-right physical keys, Layer 0 passthrough):
//             DEL=Hyper+N, HOME=Hyper+M, END=Hyper+,, PGUP=Hyper+., PGDN=Hyper+/
// ============================================
[6] = LAYOUT_ansi_84(
    _______, _______,  _______,  MAC_TASK, _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______, _______, _______,
    _______, LCA(KC_1), LCA(KC_2), LCA(KC_3), LCA(KC_4), LCA(KC_5), LCA(KC_6), LCA(KC_7), LCA(KC_8), LCA(KC_9), _______, _______, _______, _______,          _______,
    LCA(KC_TAB), _______,  KC_F19,   MY_HYPR(KC_E), KC_F13, LCA(KC_T), KC_HOME, KC_PGUP, KC_PGDN,  KC_END,  _______, LCA(KC_LBRC), LCA(KC_RBRC), _______,          _______,
    _______, MY_HYPR(KC_A), MY_HYPR(KC_S), MY_HYPR(KC_D), MO(7), KC_F16, NAV_H, NAV_J, NAV_K, NAV_L, _______, _______, _______,  _______,
    _______,           KC_F20,   KC_F18,   KC_F17,   MY_HYPR(KC_V), _______, _______, _______, _______, _______, _______, _______,          _______, _______,
    _______, _______,  _______,                                _______,                               _______, _______,          _______, _______, _______),

// ============================================
// LAYER 7: Display Focus (Space+F sublayer)
// Space+F+HJKL → Cmd+Alt+HJKL → Hammerspoon display focus
// ============================================
[7] = LAYOUT_ansi_84(
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______, _______, _______,
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______,          _______,
    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______, _______, _______, _______, _______,          _______,
    _______, _______,  _______,  _______,  _______,  _______, LAG(KC_H), LAG(KC_J), LAG(KC_K), LAG(KC_L), _______, _______, _______,              _______,
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
// Excluded on Layer 6 (Space Nav): LCA(KC_H) must send Ctrl+Alt+H (letter),
// not Ctrl+Alt+Left (arrow), so AeroSpace can bind focus to Ctrl+Alt+HJKL.
// Negative mod GUI: suppresses override when Cmd is held (i.e. Hyper combos),
// so Hyper+HJKL sends Hyper+letter (not Hyper+arrow) for AeroSpace move binding.
#define KO_LAYERS (~(1 << 6))
const key_override_t nav_h = ko_make_with_layers_and_negmods(MOD_MASK_CTRL, KC_H, KC_LEFT, KO_LAYERS, MOD_MASK_GUI);
const key_override_t nav_j = ko_make_with_layers_and_negmods(MOD_MASK_CTRL, KC_J, KC_DOWN, KO_LAYERS, MOD_MASK_GUI);
const key_override_t nav_k = ko_make_with_layers_and_negmods(MOD_MASK_CTRL, KC_K, KC_UP, KO_LAYERS, MOD_MASK_GUI);
const key_override_t nav_l = ko_make_with_layers_and_negmods(MOD_MASK_CTRL, KC_L, KC_RGHT, KO_LAYERS, MOD_MASK_GUI);

// Modifier→F-key overrides (float_c_ko, float_f_ko, paste_v_ko) REMOVED.
// They suffered from stale-modifier leaks: physical Ctrl/Cmd/Alt stay held
// when the F-key fires, so macOS sees Ctrl+Cmd+F14 instead of bare F14.
// F14/F15 are also hardware-reserved for brightness on macOS.
//
// Replacement: these actions now live on Layer 6 (Space+C/X/Z → F17/F18/F20).
// Layer-tap keys produce zero leading modifier events — clean single triggers.
const key_override_t *key_overrides[] = {
    &nav_h, &nav_j, &nav_k, &nav_l,
    NULL
};

// ============================================
// NAV RESIZE KEY REPEAT
// ============================================
// QMK-level key repeat for join-with (Shift+Space+HJKL → Ctrl+Shift+HJKL).
// AeroSpace ignores OS key repeat, so we retrigger at firmware level.
static uint16_t nav_repeat_kc = 0;
static uint8_t  nav_repeat_mods = 0;
static uint16_t nav_repeat_timer = 0;
static bool     nav_repeat_started = false;

#define NAV_REPEAT_DELAY 200  // ms before repeat begins
#define NAV_REPEAT_RATE   50  // ms between repeats

// ============================================
// NAV KEY LIFECYCLE TRACKING
// ============================================
// Fixes stuck-modifier bug: when Space (layer-tap) is released before
// a NAV key, Layer 6 deactivates and the release event arrives as
// KC_H/J/K/L (Layer 0) instead of NAV_H/J/K/L (Layer 6). Without
// tracking, the cleanup code in the NAV handler never runs, leaving
// Ctrl+Alt permanently registered in the USB HID report.
//
// Three safety nets, any one of which is sufficient:
// 1. nav_cleanup() on NAV key release (normal path)
// 2. nav_cleanup() on Layer 6 deactivation (layer_state_set_user)
// 3. nav_cleanup() on KC_H/J/K/L release when nav_key_active (ghost release)
static bool     nav_key_active = false;
static uint16_t nav_active_kc = 0;          // KC_H/J/K/L currently registered
static uint8_t  nav_active_saved_mods = 0;  // mods to restore on cleanup

// KC_F16 on Layer 6 G position — same ghost-release class as NAV keys.
// Without tracking, releasing Space before G leaves F16 stuck forever.
static bool     layer6_f16_active = false;

static void nav_cleanup(void) {
    if (!nav_key_active) return;
    unregister_code(nav_active_kc);
    // Delta-based cleanup: only remove mods WE added, leave physically-held
    // mods untouched. Fixes stuck-Ctrl when Caps released before NAV key.
    uint8_t current = get_mods();
    uint8_t nav_added = current & ~nav_active_saved_mods;
    unregister_mods(nav_added);
    nav_key_active = false;
    nav_active_kc = 0;
    nav_repeat_kc = 0;
    nav_repeat_started = false;
}

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
                } else {
                    // Not used as Hyper, send comma
                    tap_code(KC_COMM);
                }
                comm_hyp_held = false;
                comm_hyp_activated = false;
            }
            return false;  // We handle this key completely

        case NAV_SPC:  // Space with nav layer
        case KC_SPC:
            if (record->event.pressed && comm_hyp_held && !comm_hyp_activated) {
                // Special case: comma + space = ", " (not Hyper+Space)
                tap_code(KC_COMM);
                comm_hyp_held = false;
                // Space continues normally (nav layer or regular space)
            }
            return true;

        // ============================================
        // NAV HJKL: Modifier differentiation
        // No mod      → Ctrl+Alt+key (focus)
        // Caps (Ctrl) → Ctrl+Alt+Shift+key (move)
        // Shift       → Ctrl+Shift+key (join-with)
        // Option (Alt)→ Cmd+Shift+Alt+key (swap) — Hyper
        // ============================================
        case NAV_H:
        case NAV_J:
        case NAV_K:
        case NAV_L: {
            uint16_t kc;
            switch (keycode) {
                case NAV_H: kc = KC_H; break;
                case NAV_J: kc = KC_J; break;
                case NAV_K: kc = KC_K; break;
                default:    kc = KC_L; break;
            }
            if (record->event.pressed) {
                nav_cleanup();  // clear any prior stuck state
                nav_active_saved_mods = get_mods();
                clear_mods();
                uint8_t new_mods = MOD_BIT(KC_LCTL) | MOD_BIT(KC_LALT);
                if (nav_active_saved_mods & MOD_MASK_CTRL) {
                    // Caps → Ctrl+Alt+Shift (move)
                    new_mods |= MOD_BIT(KC_LSFT);
                } else if (nav_active_saved_mods & MOD_MASK_SHIFT) {
                    // Shift → Ctrl+Shift (join-with) — drop Alt
                    new_mods = MOD_BIT(KC_LCTL) | MOD_BIT(KC_LSFT);
                } else if (nav_active_saved_mods & MOD_MASK_ALT) {
                    // Option → Cmd+Shift+Alt (swap) — Hyper without Ctrl
                    new_mods = MY_HYPR_MODS;
                }
                // Cmd (physical key) → pass through for monitor move
                if (nav_active_saved_mods & MOD_MASK_GUI) {
                    new_mods |= MOD_BIT(KC_LGUI);
                }
                set_mods(new_mods);
                register_code(kc);
                nav_key_active = true;
                nav_active_kc = kc;
                // Start firmware repeat for resize
                if (nav_active_saved_mods & MOD_MASK_SHIFT && !(nav_active_saved_mods & MOD_MASK_CTRL)) {
                    nav_repeat_kc = kc;
                    nav_repeat_mods = new_mods;
                    nav_repeat_timer = timer_read();
                    nav_repeat_started = false;
                }
            } else {
                // Only cleanup if THIS key is the active one.
                // If another NAV key replaced us, our cleanup already
                // ran when that key was pressed (line 402).
                if (nav_active_kc == kc) {
                    nav_cleanup();
                }
            }
            return false;
        }

        // Ghost release: Space released before HJKL — layer reverted to 0,
        // so the release arrives as KC_H/J/K/L instead of NAV_H/J/K/L.
        // Catch it here and run the cleanup that would have run above.
        case KC_H:
        case KC_J:
        case KC_K:
        case KC_L:
            if (!record->event.pressed && nav_key_active && nav_active_kc == keycode) {
                nav_cleanup();
                return false;  // consumed — don't emit a stray letter
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

    // NAV resize key repeat — retrigger keydown at firmware level
    if (nav_repeat_kc) {
        uint16_t elapsed = timer_elapsed(nav_repeat_timer);
        if (!nav_repeat_started && elapsed > NAV_REPEAT_DELAY) {
            nav_repeat_started = true;
            nav_repeat_timer = timer_read();
        } else if (nav_repeat_started && elapsed > NAV_REPEAT_RATE) {
            unregister_code(nav_repeat_kc);
            register_code(nav_repeat_kc);
            nav_repeat_timer = timer_read();
        }
    }
}

// ============================================
// LAYER CHANGE: Clean up NAV state when Layer 6 deactivates
// ============================================
// Second safety net: if both the ghost release handler AND the NAV
// release handler miss (e.g., key released during the same scan cycle
// as the layer change), this catches it at the layer-state level.
layer_state_t layer_state_set_user(layer_state_t state) {
    if (!layer_state_cmp(state, 6)) {
        if (nav_key_active) {
            nav_cleanup();
        }
        if (layer6_f16_active) {
            unregister_code(KC_F16);
            layer6_f16_active = false;
        }
    }
    return state;
}

// ============================================
// PER-KEY TAPPING TERM
// ============================================
// Space needs a shorter tapping term for responsive typing.
// Caps Nav and other mod-taps keep the default 200ms.
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case NAV_SPC:
            return 150;  // 150ms: faster space tap detection
        default:
            return TAPPING_TERM;  // 200ms for everything else
    }
}

// ============================================
// PER-KEY QUICK TAP TERM
// ============================================
// Disable quick-tap for Space: hold must always enter Layer 6, even
// immediately after a Space tap. Without this, tapping Space then
// holding within 120ms bypasses Layer 6 entirely (QUICK_TAP_TERM
// fires instant tap, no hold detection).
uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case NAV_SPC:
            return 0;   // never skip hold detection for Space
        default:
            return QUICK_TAP_TERM;  // 120ms for everything else
    }
}

// ============================================
// SUSPEND/WAKEUP: Reset all custom state
// ============================================
// QMK's clear_keyboard() clears HID report on suspend but does NOT
// reset user static variables. Without this, state survives USB/BT
// profile switches — nav_repeat fires ghost keypresses on reconnect.
void suspend_wakeup_init_user(void) {
    nav_key_active = false;
    nav_active_kc = 0;
    nav_active_saved_mods = 0;
    layer6_f16_active = false;
    comm_hyp_held = false;
    comm_hyp_activated = false;
    nav_repeat_kc = 0;
    nav_repeat_started = false;
}

// ============================================
// PER-KEY PERMISSIVE HOLD
// ============================================
// Enabled for mod-tap keys (faster modifier activation when rolling).
// Disabled for Space to prevent false nav-layer triggers during typing.
// Caps Nav enabled: deliberate hold should activate layer immediately.
bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case NAV_SPC:
            return false;  // Space: don't activate nav layer on fast rolls
        default:
            return true;   // Caps Nav, punctuation mods: activate on rolls
    }
}
