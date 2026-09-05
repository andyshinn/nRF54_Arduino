/* nRF54LM20A Application linker script with S145 v10.0.1 SoftDevice */
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  /* RRAM is 0x1FD000 (2036 KB), not a round 2 MB.
   * App FLASH ends at 0x1C7000, last 28 KB reserved for InternalFS
   * (see __flash_arduino_start in nrf54_common.ld). Bootloader lives at
   * 0x1D0000, 8 KB gap from 0x1CE000-0x1CFFFF is unused.
   * s145 v10.0.1 occupies 0x1DA800-0x1FCC00 (0x22400), leaving the
   * bootloader 0x1D0000-0x1DA800 (42 KB). */
  FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 0x1C6000

  /* 512 KB RAM in two contiguous 256 KB banks (0x20000000 + 0x20040000).
   * The 0x4800 reserved below the app matches the other nRF54L targets;
   * s145 v10.0.1 needs 0x1480 minimum, the rest covers sd_ble_enable(). */
  RAM (rwx) :  ORIGIN = 0x20004800, LENGTH = 0x7B800

  /* SVC data (bond info exchange between app and bootloader) */
  NOINIT (rwx) :  ORIGIN = 0x200047FC, LENGTH = 0x04
}

INCLUDE "nrf54_common.ld"
