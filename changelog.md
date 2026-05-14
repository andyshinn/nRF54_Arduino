# Changelog

## 0.1.0 — 2026-05-12

Initial fork from
[Adafruit_nRF52_Arduino](https://github.com/adafruit/Adafruit_nRF52_Arduino),
reworked for Nordic nRF54L05/L10/L15 with s145 SoftDevice 9.0.0.
PlatformIO-only consumption via
[caveman99/platform-nordicnrf54](https://github.com/caveman99/platform-nordicnrf54).

Highlights:

- nRF54L MDK headers, s145 9.0.0 SoftDevice API and SoftDevice hex
- Linker scripts for L05/L10/L15 reserving 28 KB for InternalFS
- FreeRTOS port running off the GRTC peripheral (nRF54L has no SysTick)
- `nrf54l_compat.h` aliasing nRF52 peripheral names to nRF54L
  equivalents so most Adafruit core code compiles unmodified
- `wiring_analog_nRF54L.c` — fresh SAADC implementation accounting for
  nRF54L's 0.9 V internal reference, multiplier gain enum, microsecond
  TACQ and byte-count MAXCNT
- InternalFileSystem and `flash_nrf5x.c` consume `__flash_arduino_start`
  / `__bootloader_addr` linker symbols (no per-chip ifdefs in code)
- Bluefruit54Lib: HID detached from TinyUSB; data-length update reply
  capped by configured BANDWIDTH
- Five board variants: `xiao_nrf54l15`, `xiao_nrf54l15_sense`,
  `nrf54l15dk`, `nrf54l10dk`, `nrf54l05dk`
- XIAO variants route `Wire` to TWIM22 (dedicated TWI controller),
  avoiding the SERIAL00/SERIAL20 fabric shared with SPI/UARTE
- nRF52-only submodules (`Adafruit_TinyUSB_Arduino`,
  `Adafruit_nRFCrypto`) removed — nRF54L has no USB and uses CRACEN
  instead of CC310
- Bootloader hex sourced from the separate
  [nRF54_Bootloader](https://github.com/caveman99/nRF54_Bootloader)
  PlatformIO package (this repo ships only the SoftDevice hex)
