# Agent guide for this repo

## What this repo is

Arduino BSP for **Nordic nRF54L05 / nRF54L10 / nRF54L15** with the
**s145 SoftDevice v9.0.0**. Derived from `Adafruit_nRF52_Arduino` and
reworked for nRF54L peripherals.

**PlatformIO only.** Consumed exclusively through
[caveman99/platform-nordicnrf54](https://github.com/caveman99/platform-nordicnrf54).
There is no `platform.txt`, `boards.txt`, or Arduino IDE / Board
Manager support. Do not propose adding them.

## Companion repos

| Repo | Role |
|---|---|
| [`caveman99/platform-nordicnrf54`](https://github.com/caveman99/platform-nordicnrf54) | PlatformIO platform: build flags, board JSONs, upload tooling, examples |
| [`caveman99/nRF54_Bootloader`](https://github.com/caveman99/nRF54_Bootloader) | DFU bootloader source + per-board hex. PIO package `framework-arduinoadafruitnrf54-bootloader` |

## Project conventions

### Commit messages

- **Never add `Co-Authored-By: Claude` (or any AI attribution) trailers.**
- Imperative subject, terse but specific. Reference SHAs of related
  prior commits where helpful.
- Multi-paragraph body for non-trivial changes: state the symptom, the
  root cause, and what the fix does.

### Code style

- Default to writing no comments. Add one only when the *why* is
  non-obvious — a hidden constraint, a workaround, a behavior that
  would surprise a reader. Don't explain *what* the code does.
- Don't add backwards-compat shims, dead branches, or speculative
  abstractions. Trust framework guarantees.
- C++17 is the standard; no RTTI, no exceptions (set by the platform).

### Cross-repo dependencies

Some fixes belong in `platform-nordicnrf54`
(`builder/frameworks/arduino/adafruit.py`), not here:

- Build flags (`-mcpu`, `-mfpu`, include paths, CPPDEFINES)
- Board JSON metadata
- Upload tooling
- LDF / library resolution

Some fixes belong here (the framework):

- C/C++ source code, headers, linker scripts, variants
- `library.properties` for bundled libraries
- `nrfx_config.h` and per-driver guards

When in doubt: if the change affects how the compiler is invoked, it's
platform-side. If it affects what gets compiled, it's framework-side.

## Architectural fixed points (don't accidentally break these)

### `nrf54l_compat.h` is force-included by the platform

The platform's
[`builder/frameworks/arduino/adafruit.py`](https://github.com/caveman99/platform-nordicnrf54/blob/master/builder/frameworks/arduino/adafruit.py)
adds `-include cores/nRF5/nordic/nrf54l_compat.h` to CCFLAGS. This
aliases nRF52 peripheral names (`NRF_PWM0..2`, `NRF_TWIM0/1`,
`NRF_SPIM0/2`, `NRF_UARTE0/1`, `NRF_GPIOTE`, `NRF_TIMER2`, etc.) to
their nRF54L equivalents. **Most of the Adafruit code compiles
unmodified because of this header.**

Scope rule for `nrf54l_compat.h`: **symbol-name aliases only**.
Register-shape changes (split-bank `INTENSETn`, split
`DRIVE0`/`DRIVE1` fields) must NOT be aliased — the nrfx HAL uses
`#if defined(...)` checks on the legacy macros as chip-detection
markers, and aliasing them makes the HAL take the wrong branch.
Rewrite the consuming source in those cases (see `WInterrupts.c`
and `wiring_digital.c` for two existing precedents).

### nrfx driver gating is test-driven

Every driver `.c` file in `cores/nRF5/nordic/nrfx/drivers/src/` is
wrapped in `#if NRFX_CHECK(NRFX_<X>_ENABLED) ... #endif`, and
`cores/nRF5/nordic/nrfx_config.h` defaults every driver to
`_ENABLED = 1`. **Drivers are disabled only after the compiler flags
a specific error.** Current disable list (with the error that prompted
each):

- `NRFX_ADC_ENABLED = 0` — `NRF_ADC / ADC_IRQn` undeclared (nRF51 legacy ADC)
- `NRFX_BELLBOARD_ENABLED = 0` — `NRF_BELLBOARD` undeclared (nRF54H IPC); also the source of cryptic `nrfx_utils.h:517` errors via `NRFX_OFFSETOF`
- `NRFX_CLOCK_LFCLK_ENABLED = 0` — `NRFX_CLOCK_CONFIG_LF_SRC` undefined; LFCLK is started by `cores/nRF5/wiring.c` directly
- `NRFX_IPCT_ENABLED = 0` — IPCT is nRF54H DPPI cross-domain routing; the helper in `nrfx/helpers/internal/nrfx_gppiv1_ipct.c` `#errors` "Platform not supported" otherwise
- `NRFX_VEVIF_ENABLED = 0` — VPR-coprocessor-side driver; the app-core MDK has `VPR00_IRQHandler` but not the `VPRCLIC_*_IRQn` vectors `nrfx_vevif.c` needs
- `NRFX_USBD_ENABLED = NRFX_USBREG_ENABLED = 0` — no USB peripheral on nRF54L

When a new driver fails to compile, add it to the disabled list in
`nrfx_config.h` with a one-line comment naming the error. Do not
preemptively disable drivers just because the framework doesn't link
against them — the linker eliminates unused code via `--gc-sections`.

### `wiring_analog_nRF52.c` is `#if NRF52`-gated

The nRF54L SAADC implementation lives in
`cores/nRF5/wiring_analog_nRF54L.c`. nRF54L's SAADC differs from nRF52
in five ways: 0.9 V internal reference (not 0.6 V), multiplier gain
enum (`Gain2_8` = 2/8, not `Gain1_6` = 1/6), microsecond TACQ count
(not enum), byte-count `MAXCNT` (not sample count), structured PSELP
(`CONNECT | PORT | PIN`, not `AnalogInput0..7`). Don't try to merge
the two — they're genuinely different peripherals.

### Linker symbols pin the memory map

`cores/nRF5/linker/nrf54_common.ld` defines `__flash_arduino_start`
and `__bootloader_addr` so InternalFS / `flash_nrf5x.c` work
chip-independently. Per-chip values:

| Chip | App FLASH end | Bootloader |
|---|---|---|
| nRF54L05 | `0x47000` | `0x50000` |
| nRF54L10 | `0xC7000` | `0xD0000` |
| nRF54L15 | `0x147000` | `0x150000` |

28 KB carved off the top of FLASH for LittleFS, 8 KB gap to bootloader.
Confirmed against the bootloader repo's `linker/` scripts.

The system area (SoftDevice + bootloader) sits at the **top** of RRAM
on nRF54L — the application starts at `0x1000` (right after the MBR)
and grows up toward the bootloader. This is the inverse of nRF52
(SoftDevice at low addresses, app grows up toward bootloader at top).
Reasoning by analogy with nRF52 layout will get linker addresses wrong.

### XIAO Wire routes to TWIM22

The default `Wire` instance (`NRF_TWIM0` → `NRF_TWIM20` via compat) sits
on the SERIAL20 fabric, which on XIAO is also where SPIM lands. The
XIAO variants override `WIRE_TWIM` / `WIRE_TWIS` / `WIRE_IRQN` /
`WIRE_IRQ_HANDLER` in their `variant.h` to point at TWIM22 (a
dedicated TWI controller). Don't remove the override without
verifying the routing on actual hardware.

### `Serial` aliases to `Serial1`

nRF54L has no USB peripheral. `Uart.h` defines `Serial` as
`Serial1` so portable Arduino sketches compile. On XIAO boards
`Serial1` runs through the on-board SAMD11 USB-CDC bridge.

## What's in the tree (orientation)

```
cores/nRF5/                  - core sources (Arduino API + FreeRTOS port + nrfx tree)
  Arduino.h                  - main include
  Uart.{cpp,h}               - UARTE-backed HardwareSerial (Serial1 = primary)
  Wire/SPI/HardwarePWM       - register-direct via HAL inlines
  WInterrupts.c              - attachInterrupt; uses HAL nrf_gpiote_int_*
  wiring_analog_nRF54L.c     - SAADC
  freertos/                  - FreeRTOS kernel (capital-S Source/) + GRTC port
  linker/                    - .ld scripts (chip + common)
  nordic/
    nrf54l_compat.h          - force-included; aliases nRF52 names → nRF54L
    nrfx/                    - upstream Nordic nrfx (mostly dead via guards)
      drivers/src/*.c        - all wrapped in #if NRFX_<X>_ENABLED
    nrfx_config.h            - one place to flip drivers on/off
    softdevice/s145_nrf54l_9.0.0_API/  - SoftDevice headers
libraries/                   - PIO LDF-discovered bundled libs
  Bluefruit54Lib             - BLE API on top of s145
  nRF54Crypto                - CRACEN + TinyCrypt for LESC (replaces CC310)
  Adafruit_LittleFS          - filesystem
  InternalFileSytem          - LittleFS-on-sd_flash for bonding
  Wire/SPI/Servo/PDM/SoftwareSerial/RotaryEncoder
variants/                    - per-board pin tables + linker scripts
  xiao_nrf54l15(_sense)
  nrf54l05dk / nrf54l10dk / nrf54l15dk
bootloader/s145/9.0.0/       - SoftDevice hex (bootloader hex lives in nRF54_Bootloader)
tests/blink/                 - CI smoke test
.github/workflows/githubci.yml  - PIO matrix build for all 5 variants
```
