/* Common linker script for nRF54L Arduino applications */

OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
ENTRY(Reset_Handler)

/* InternalFS (LittleFS) region: 28 KB carved out just above the app FLASH,
 * below the bootloader. Single source of truth for InternalFileSystem.cpp
 * and flash_nrf5x.c; per-chip values are determined by FLASH length in the
 * including chip linker script:
 *   nRF54L05: 0x47000-0x4DFFF
 *   nRF54L10: 0xC7000-0xCDFFF
 *   nRF54L15: 0x147000-0x14DFFF
 *   nRF54LM20A: 0x1C7000-0x1CDFFF
 *
 * The 8 KB gap between __flash_arduino_end and __bootloader_addr is the
 * same on all four chips (page alignment for the bootloader entry). */
PROVIDE(__flash_arduino_start = ORIGIN(FLASH) + LENGTH(FLASH));
PROVIDE(__flash_arduino_end   = __flash_arduino_start + 0x7000);
PROVIDE(__bootloader_addr     = __flash_arduino_end + 0x2000);

/* __softdevice_addr: base of the SoftDevice image in RRAM. It cannot be
 * derived from FLASH/__bootloader_addr -- the SoftDevice sits above the
 * bootloader at a chip-specific offset (0x8C00 on L05/L10/L15 with s145
 * v9.0.0, 0xA800 on LM20A with s145 v10.0.1) -- so each chip script sets it
 * explicitly. nRF52 code that reads the SD info struct at MBR_SIZE is wrong
 * here: on nRF54L that address is application code.
 *   nRF54L05: 0x058C00   nRF54L10: 0x0D8C00
 *   nRF54L15: 0x158C00   nRF54LM20A: 0x1DA800
 * Consumed by utilities.c (SD_ID_GET / SD_VERSION_GET). */

SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector))
        *(.text*)
        KEEP(*(.init))
        KEEP(*(.fini))
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)
        *(.rodata*)
        KEEP(*(.eh_frame*))
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;

    __etext = .;
    _sidata = __etext;

    /* SoftDevice SVC data */
    .svc_data :
    {
        PROVIDE(__start_svc_data = .);
        KEEP(*(svc_data))
        PROVIDE(__stop_svc_data = .);
    } > NOINIT

    /* Filesystem data */
    .fs_data :
    {
        PROVIDE(__start_fs_data = .);
        KEEP(*(fs_data))
        PROVIDE(__stop_fs_data = .);
    } > RAM

    .data : AT (__etext)
    {
        __data_start__ = .;
        _sdata = .;
        *(vtable)
        *(.data*)
        . = ALIGN(4);
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        . = ALIGN(4);
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);
        . = ALIGN(4);
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
        KEEP(*(.jcr*))
        . = ALIGN(4);
        __data_end__ = .;
        _edata = .;
    } > RAM

    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
        _ebss = .;
    } > RAM

    .noinit (NOLOAD) :
    {
        PROVIDE(__start_noinit_data = .);
        KEEP(*(.noinit))
        PROVIDE(__stop_noinit_data = .);
    } > RAM

    /* Heap and stack are sized here, not by input sections. The Nordic MDK
     * startup files reserve .heap/.stack with .space directives; the
     * hand-written startup_nrf54l15.S / startup_nrf54lm20a.S in this core do
     * not, and nothing else in the tree emits those sections either. Deriving
     * the sizes from KEEP(*(.heap*)) / SIZEOF(.stack_dummy) therefore made
     * both of them zero: __HeapLimit landed on __HeapBase, the first _sbrk()
     * (new.cpp) returned -1, and malloc/new returned NULL for the life of the
     * program -- including during static construction, before setup() runs.
     *
     * MSP only has to carry the startup path, the SoftDevice's handlers and
     * whatever ISRs nest on them; FreeRTOS tasks get their stacks from the
     * heap. 8 KB is what Adafruit's nRF52 core reserves for the same job.
     * Everything between the end of .bss and the stack is heap. */
    __stack_size = 0x2000;

    .heap (COPY):
    {
        . = ALIGN(8);
        __HeapBase = .;
        __end__ = .;
        PROVIDE(end = .);
    } > RAM

    __StackTop   = ORIGIN(RAM) + LENGTH(RAM);
    __StackLimit = __StackTop - __stack_size;
    __HeapLimit  = __StackLimit;
    PROVIDE(__stack = __StackTop);

    ASSERT(__HeapLimit > __HeapBase, "region RAM overflowed: no room left for the heap")
    DataInitFlashUsed = __bss_start__ - __data_start__;
    CodeFlashUsed = __etext - ORIGIN(FLASH);
    TotalFlashUsed = CodeFlashUsed + DataInitFlashUsed;
    ASSERT(TotalFlashUsed <= LENGTH(FLASH), "region FLASH overflowed with .data and user data")
}
