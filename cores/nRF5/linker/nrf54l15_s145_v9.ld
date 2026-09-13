/* nRF54L15 application with the s145 v9.0.0 SoftDevice.
 * RRAM 0x0-0x8000 is the bootloader (it owns reset), the SoftDevice sits at the top of RRAM. */
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  /* App FLASH ends at 0x147000, last 28 KB reserved for InternalFS (see __flash_arduino_start). */
  FLASH (rx) : ORIGIN = 0x00008000, LENGTH = 0x13F000
  RAM (rwx) :  ORIGIN = 0x20004800, LENGTH = 0x3B800

  /* SVC data (bond info exchange between app and bootloader) */
  NOINIT (rwx) :  ORIGIN = 0x200047FC, LENGTH = 0x04
}

__softdevice_start__ = 0x00158C00;

INCLUDE "nrf54_common.ld"
