/* nRF54L10 application with the s145 v9.0.0 SoftDevice.
 * RRAM 0x0-0x8000 is the bootloader (it owns reset), the SoftDevice sits at the top of RRAM.
 * The SoftDevice base is derived from the L15 image size and unverified on hardware. */
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00008000, LENGTH = 0xbf000
  RAM (rwx) :  ORIGIN = 0x20004800, LENGTH = 0x3B780

  /* Bootloader peer data and double-reset marker, must stay untouched by the app */
  NOINIT (rwx) :  ORIGIN = 0x2003FF80, LENGTH = 0x78
}

/* InternalFS size; see __flash_arduino_size in nrf54_common.ld. */
__flash_arduino_size = 0x7000;

__softdevice_start__ = 0x000D8C00;

INCLUDE "nrf54_common.ld"
