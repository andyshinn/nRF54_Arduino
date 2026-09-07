/* nRF54L15 Application linker script with S145 v9.0.0 SoftDevice */
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  /* App FLASH ends at 0x147000, last 28 KB reserved for InternalFS
   * (LittleFS used by BLE bonding etc., see __flash_arduino_start below).
   * Bootloader lives at 0x150000, 8 KB gap from 0x14E000-0x14FFFF is unused. */
  FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 0x146000
  RAM (rwx) :  ORIGIN = 0x20004800, LENGTH = 0x3B800

  /* SVC data (bond info exchange between app and bootloader) */
  NOINIT (rwx) :  ORIGIN = 0x200047FC, LENGTH = 0x04
}

/* Base of the SoftDevice image; see __softdevice_addr in nrf54_common.ld. */
PROVIDE(__softdevice_addr = 0x158C00);

INCLUDE "nrf54_common.ld"
