/* Unused by the build: adafruit.py sets LDSCRIPT_PATH to
 * variants/<variant>/<board_build.ldscript>, so the copy under variants/ is the
 * one that links. Kept only so this directory holds a full set; any edit here
 * must be mirrored there or the two silently disagree (they already did once --
 * this file still had the RAM length that put __StackTop in unmapped memory).
 * nrf54_common.ld is the only script in here the linker actually reads. */
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

  /* 512 KB RAM in two contiguous 256 KB banks (0x20000000 + 0x20040000),
   * but the top of the range is NOT usable. Measured on a XIAO nRF54LM20A
   * over SWD: 0x2007FEDC is the last readable word, 0x2007FEE0 and above
   * bus-fault for both the debugger and the CPU. (Held in reset the usable
   * top drops further and reads inconsistently, so do not tune close to it.)
   *
   * Ending RAM at a nominal 0x20080000 puts __StackTop (nrf54_common.ld)
   * there, so the very first instruction of SystemInit -- push {r3, lr},
   * writing to 0x2007FFF8 -- locks the core up before main() ever runs.
   * Confirmed by single-stepping that one instruction on hardware: with
   * SP=0x20080000 the push does not retire and PC becomes 0xEFFFFFFE (the
   * ARMv8-M lockup PC); with SP=0x2007FCC0 it retires normally.
   *
   * End at 0x2007FCC0 instead, matching Nordic's own partitioning for this
   * SoC (softdevice_ram 0x0..0x4780 + app_ram in
   * bm_nrf54lm20dk_nrf54lm20a_cpuapp_s145_softdevice.dts, sdk-nrf-bm v2.0.1)
   * and the boot stub / bootloader in nRF54_Bootloader, which both use
   * 0x2007FCC0 as their stack top. That leaves 0x340 unused at the top.
   *
   * Note the flat memory maps (nrfx MDK linker script, pyOCD
   * target_nRF54LM20A.py, TinyUSB's nRF BSP) all claim the full 512 KB. They
   * declare RAM and RAM1 as separate regions, so their __StackTop lands at
   * 0x20040000 and they never touch the unusable top. This script merges the
   * banks, so the limit matters here.
   *
   * The 0x4800 reserved below the app matches the other nRF54L targets;
   * s145 v10.0.1 needs 0x1480 minimum, the rest covers sd_ble_enable(). */
  RAM (rwx) :  ORIGIN = 0x20004800, LENGTH = 0x7B4C0

  /* SVC data (bond info exchange between app and bootloader) */
  NOINIT (rwx) :  ORIGIN = 0x200047FC, LENGTH = 0x04
}

INCLUDE "nrf54_common.ld"
