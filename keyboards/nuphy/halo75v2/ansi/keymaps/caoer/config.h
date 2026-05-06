// Custom config for caoer - No home row mods
#pragma once

// ============================================
// TAPPING TERM - Core timing for tap vs hold
// ============================================
#undef TAPPING_TERM
#define TAPPING_TERM 200  // Default: 200ms for Caps Lock (Ctrl/Esc) and punctuation mods

// Per-key tapping term: Space gets a shorter window for faster typing
#define TAPPING_TERM_PER_KEY

// QUICK_TAP_TERM: After tapping a key, pressing it again within this window
// instantly registers as a tap (no hold-detection delay). Makes repeated
// spaces feel instant when typing "word.  Next sentence".
#define QUICK_TAP_TERM 120

// ============================================
// REMAINING TAP-HOLD KEYS
// ============================================
// - Caps Lock: Ctrl (hold) / Esc (tap) — HJKL arrows via Key Overrides
// - Left Ctrl: Hyper (hold only)
// - Space: Nav Layer 6 (hold) / Space (tap) — YUIO page nav + Ctrl+arrows + app launchers
// - F3/F5/F10: Tap dance keys

// RETRO_TAPPING: If you hold and release WITHOUT pressing another key,
// it sends the tap action. Useful for Caps Lock -> Esc.
#define RETRO_TAPPING

// PERMISSIVE_HOLD per-key: For mod-tap keys (Caps-Ctrl, punctuation mods),
// if another key is pressed AND released while held, treat as hold immediately.
// Disabled for Space to avoid false nav-layer activations during fast typing.
#define PERMISSIVE_HOLD_PER_KEY

// ============================================
// KEEP ORIGINAL DEFAULTS (override only what we need)
// ============================================
// Inherits all other settings from parent config.h
