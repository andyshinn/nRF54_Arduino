/* nRF54L10 Application linker script with S145 v9.0.0 SoftDevice */
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  /* App FLASH ends at 0xC7000, last 28 KB reserved for InternalFS
   * (LittleFS used by BLE bonding etc., see __flash_arduino_start below).
   * Bootloader lives at 0xD0000, 8 KB gap from 0xCE000-0xCFFFF is unused. */
  FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 0xC6000
  RAM (rwx) :  ORIGIN = 0x20004800, LENGTH = 0x2B800

  /* SVC data (bond info exchange between app and bootloader) */
  NOINIT (rwx) :  ORIGIN = 0x200047FC, LENGTH = 0x04
}

/* Base of the SoftDevice image; see __softdevice_addr in nrf54_common.ld. */
PROVIDE(__softdevice_addr = 0x0D8C00);

INCLUDE "nrf54_common.ld"
