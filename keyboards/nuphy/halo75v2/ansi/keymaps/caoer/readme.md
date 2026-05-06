# NuPhy Halo75 V2 - caoer Custom Keymap

Custom QMK keymap with **Home Row Mods** and optimized **TAPPING_TERM**.

## Features

### Home Row Mods (Both Hands)
```
Left hand:                    Right hand:
A = Ctrl (hold) / a (tap)     J = Shift (hold) / j (tap)
S = Alt  (hold) / s (tap)     K = Cmd   (hold) / k (tap)
D = Cmd  (hold) / d (tap)     L = Alt   (hold) / l (tap)
F = Shift(hold) / f (tap)     ; = Ctrl  (hold) / ; (tap)
```

### Timing Settings
- **TAPPING_TERM**: 100ms (default 200ms) - faster tap detection
- **PERMISSIVE_HOLD**: Enabled for home row mods
- **HOLD_ON_OTHER_KEY_PRESS**: Enabled for home row mods
- **QUICK_TAP_TERM**: 120ms - double-tap-hold for repeat

## Build Instructions

### 1. Install QMK Toolchain (one-time setup)

```bash
# Install Homebrew if not installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install QMK CLI
brew install qmk/qmk/qmk

# Set up QMK (will clone qmk_firmware to ~/qmk_firmware)
qmk setup

# Install ARM toolchain for STM32
brew install arm-none-eabi-gcc
```

### 2. Build This Firmware

```bash
# Navigate to this repo
cd /Users/Shared/projects/OpenSourceProjects/nuphy-qmk-firmware

# Compile (outputs .bin file)
make nuphy/halo75v2/ansi:caoer

# Or use qmk command
qmk compile -kb nuphy/halo75v2/ansi -km caoer
```

Output file: `nuphy_halo75v2_ansi_caoer.bin`

### 3. Flash the Firmware

#### Option A: QMK Toolbox (GUI - Recommended for First Time)
1. Download [QMK Toolbox](https://github.com/qmk/qmk_toolbox/releases)
2. Open QMK Toolbox
3. Load the `.bin` file
4. Put keyboard in DFU mode:
   - **Method 1**: Fn + Tab (SLEEP_MODE), then hold Fn + \ (BAT_SHOW) while reconnecting USB
   - **Method 2**: Press reset button on back of keyboard (if accessible)
5. Click "Flash" in QMK Toolbox

#### Option B: Command Line
```bash
# After building, flash directly
make nuphy/halo75v2/ansi:caoer:flash

# Or
qmk flash -kb nuphy/halo75v2/ansi -km caoer
```

### 4. Test Your Changes

After flashing:
1. Reconnect keyboard via USB
2. Test home row mods:
   - Hold `A` + press `C` → should copy (Ctrl+C)
   - Hold `D` + press `C` → should copy (Cmd+C on Mac)
   - Hold `F` + press letters → should be UPPERCASE (Shift)
3. Test tap:
   - Tap `A` quickly → should type 'a'

## Troubleshooting

### Accidental Mods While Typing Fast
If you're getting unintended modifier triggers:
1. Increase `TAPPING_TERM` in `config.h` (try 120, 150, or 175)
2. Adjust per-key timing in `get_tapping_term()` function

### Mods Not Triggering
If holds aren't registering as modifiers:
1. Decrease `TAPPING_TERM` (but not below 80ms)
2. Ensure you're holding long enough

### Recovery if Bricked
1. Disconnect keyboard
2. Hold Esc key while plugging in USB
3. Keyboard enters DFU mode
4. Flash stock firmware from [NuPhy](https://nuphy.com/pages/qmk-firmwares)

## Customization

### Adjust Timing
Edit `config.h`:
```c
#define TAPPING_TERM 150  // increase if getting accidental mods
#define QUICK_TAP_TERM 100  // decrease for faster double-tap repeat
```

### Change Mod Order
Edit `keymap.c` - modify the `HRM_*` definitions:
```c
// For Windows-style layout (Ctrl-Alt-Win instead of Ctrl-Alt-Cmd):
#define HRM_D LALT_T(KC_D)   // D = Alt when held
#define HRM_S LGUI_T(KC_S)   // S = Win when held
```

### Add Combos (Advanced)
Uncomment in `rules.mk`:
```make
COMBO_ENABLE = yes
```
Then define combos in `keymap.c`.

## Layer Map

| Layer | Name      | Access      |
|-------|-----------|-------------|
| 0     | Mac Base  | Default     |
| 1     | Mac Fn    | Hold Fn key |
| 2     | Win Base  | Mode switch |
| 3     | Win Fn    | Hold Fn key |
| 4     | Side RGB  | Fn + M      |

## Credits

- Base firmware: [ryodeushii/qmk-firmware](https://github.com/ryodeushii/qmk-firmware)
- Home row mods concept: [precondition's guide](https://precondition.github.io/home-row-mods)
