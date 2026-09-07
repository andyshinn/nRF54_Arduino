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

/* The SoftDevice's RAM. It sits below the application's, from the bottom of
 * RAM up to ORIGIN(RAM), and sd_ble_enable() is handed __data_start__ as the
 * boundary. Everything in it belongs to the SoftDevice except the top two
 * words, which are how the application and the bootloader talk across a
 * reset: the bootloader's double-reset detector at ORIGIN(RAM)-8 and the SVC
 * data word at ORIGIN(RAM)-4 (the NOINIT region in the chip scripts).
 *
 * Reset_Handler clears this range at every boot. The SoftDevice keeps its
 * "am I enabled" state here, and nothing else clears it -- startup only zeroes
 * the application's .bss, which is above ORIGIN(RAM). See the comment at the
 * clearing loop in startup_nrf54*.S for why that matters. */
PROVIDE(__softdevice_ram_start = 0x20000000);
PROVIDE(__softdevice_ram_end   = ORIGIN(RAM) - 8);

/* __bootloader_peer_data: 0x40 bytes reserved at the very bottom of the
 * application's RAM, before .data, holding the bond information handed to the
 * bootloader across a soft reset into DFU (BLEDfu.cpp writes it,
 * dfu_ble_svc.c in nRF54_Bootloader reads it).
 *
 * Both images place it at their own ORIGIN(RAM), which is the same address on
 * every nRF54L target here, so this is a contract between the two and not a
 * free choice. It has to be a hole in the memory map rather than an ordinary
 * variable: nothing may be loaded or zeroed over it, or the data would not
 * survive the reset that is the entire point.
 *
 * dfu_ble_peer_data_t is 60 bytes plus a 16-bit CRC = 62; 0x40 rounds that up
 * and keeps .data 8-byte aligned. */

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

    /* Bond handoff window -- see __bootloader_peer_data above.
     *
     * Must be the first output section allocated in RAM, ahead of .fs_data as
     * well as .data, or its address moves the moment anything contributes an
     * fs_data input section. NOLOAD so nothing is written over it at startup.
     * The ASSERT below is the contract with the bootloader: if this ever stops
     * landing on ORIGIN(RAM), the link fails rather than shipping a DFU that
     * silently loses its bond. */
    .peer_data (NOLOAD) :
    {
        __bootloader_peer_data = .;
        . = . + 0x40;
    } > RAM

    ASSERT(__bootloader_peer_data == ORIGIN(RAM),
           "bond handoff window must start at ORIGIN(RAM)")

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
