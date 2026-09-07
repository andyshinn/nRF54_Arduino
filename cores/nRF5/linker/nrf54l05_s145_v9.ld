/* Unused by the build: adafruit.py sets LDSCRIPT_PATH to
 * variants/<variant>/<board_build.ldscript>, so the copy under variants/ is the
 * one that links. Kept only so this directory holds a full set; any edit here
 * must be mirrored there or the two silently disagree (they already did once --
 * this file still had the RAM length that put __StackTop in unmapped memory).
 * nrf54_common.ld is the only script in here the linker actually reads. */
/* nRF54L05 Application linker script with S145 v9.0.0 SoftDevice */
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  /* App FLASH ends at 0x47000, last 28 KB reserved for InternalFS
   * (LittleFS used by BLE bonding etc., see __flash_arduino_start below).
   * Bootloader lives at 0x50000, 8 KB gap from 0x4E000-0x4FFFF is unused. */
  FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 0x46000
  RAM (rwx) :  ORIGIN = 0x20004800, LENGTH = 0x13800

  /* SVC data (bond info exchange between app and bootloader) */
  NOINIT (rwx) :  ORIGIN = 0x200047FC, LENGTH = 0x04
}

INCLUDE "nrf54_common.ld"
