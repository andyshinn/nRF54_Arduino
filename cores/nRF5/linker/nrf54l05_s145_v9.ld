/* nRF54L05 application with the s145 v9.0.0 SoftDevice.
 * RRAM 0x0-0x8000 is the bootloader (it owns reset), the SoftDevice sits at the top of RRAM.
 * The SoftDevice base is derived from the L15 image size and unverified on hardware. */
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00008000, LENGTH = 0x3f000
  RAM (rwx) :  ORIGIN = 0x20004800, LENGTH = 0x3B800

  NOINIT (rwx) :  ORIGIN = 0x200047FC, LENGTH = 0x04
}

__softdevice_start__ = 0x00058C00;

INCLUDE "nrf54_common.ld"
