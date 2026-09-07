/*
 * SoftDevice interrupt forwarding for nRF54L.
 *
 * nRF52 put an MBR at address 0 that owned the vector table and forwarded
 * every exception the SoftDevice needed. nRF54L has no MBR, so VTOR points at
 * the application's own table (see the VTOR write at the top of Reset_Handler
 * in startup_nrf54*.S) and nothing forwards anything. The SoftDevice states
 * the requirement itself, in nrf_sd_isr.h:
 *
 *   "The SoftDevice interrupt vector table contains only the addresses of the
 *    interrupt handlers required by the SoftDevice. The table is located at
 *    the SoftDevice base address. When the SoftDevice is enabled, the
 *    application must forward the interrupts corresponding to the defined
 *    offsets to the SoftDevice."
 *
 * That table is ten words at the SoftDevice base. Verified present in all four
 * blobs shipped under bootloader/s145 -- every entry has the Thumb bit set and
 * points inside the SoftDevice image.
 *
 * Without this file sd_softdevice_enable() cannot work at all: it is an SVC,
 * and an unforwarded SVC never reaches the SoftDevice.
 *
 * Deliberately NOT forwarded: RESET and HARDFAULT, the other two entries in
 * the SoftDevice's table. RESET belongs to the application, which is what
 * boots. HARDFAULT is a judgement call rather than an omission -- the nRF52
 * MBR did forward it, but the application's own handler is the only fault
 * diagnostic this port has, and the SoftDevice's is there to reset cleanly
 * rather than to keep the radio alive. Revisit if faults start disappearing
 * into the stack instead of being reported.
 *
 * Also deliberately NOT forwarded: SWI01 and SWI02. Those are the two the
 * SoftDevice raises *for* the application -- SD_EVT_IRQn and
 * RADIO_NOTIFICATION_IRQn (nrf_soc.h) -- and bluefruit.cpp owns SWI01. They
 * appear in SD_SWI_USED because the application may not use them for its own
 * purposes, not because they need forwarding.
 *
 * The SoftDevice base comes from __softdevice_addr, which the per-chip linker
 * scripts set and which was taken from the SoftDevice info struct in each
 * shipped hex, so a wrong base is a link-time concern rather than a silent
 * runtime jump into erased RRAM.
 */

/* Pure #define header; its extern "C" guards vanish under the assembler's
 * preprocessor, so the offsets come from the vendor header rather than being
 * copied here. */
#include "nrf_sd_isr.h"

/* First SVC number claimed by the SoftDevice: SDM_SVC_BASE in nrf_sdm.h.
 * 0x00..0x0F stay with the application, which is where FreeRTOS's svc 0 lives.
 * utilities.c carries a _Static_assert tying this constant to the header. */
#define SD_SVC_FIRST    0x10

    .syntax unified
    .cpu cortex-m33
    .thumb
    .fpu fpv5-sp-d16

    .section .text.nrf54l_sd_isr, "ax"

/* ---------------------------------------------------------------------------
 * SVC
 *
 * Split by SVC immediate: the application keeps 0x00..0x0F and everything at
 * or above SDM_SVC_BASE goes to the SoftDevice.
 *
 * The stack the exception frame was pushed on is selected by EXC_RETURN bit 2,
 * not by comparing LR against a literal 0xFFFFFFFD -- Armv8-M defines several
 * EXC_RETURN values (secure/non-secure, standard/extended frame) and only that
 * bit is the SPSEL indication.
 *
 * Branching rather than calling into vPortSVCHandler keeps LR as EXC_RETURN,
 * which is what that handler returns through.
 * ------------------------------------------------------------------------ */
    .thumb_func
    .global SVC_Handler
    .type   SVC_Handler, %function
SVC_Handler:
    tst     lr, #4                  /* EXC_RETURN[2]: 0 = MSP, 1 = PSP */
    ite     eq
    mrseq   r0, msp
    mrsne   r0, psp
    ldr     r1, [r0, #24]           /* stacked PC (frame: r0 r1 r2 r3 r12 lr pc xpsr) */
    ldrb    r1, [r1, #-2]           /* the imm8 of the svc that got us here */
    cmp     r1, #SD_SVC_FIRST
    blo     vPortSVCHandler
    ldr     r1, =__softdevice_addr
    ldr     r1, [r1, #NRF_SD_ISR_OFFSET_SVC]
    bx      r1
    .size   SVC_Handler, .-SVC_Handler

/* ---------------------------------------------------------------------------
 * IRQ forwarding
 *
 * Tail branch straight to the SoftDevice's handler, leaving the exception
 * frame and EXC_RETURN untouched so it returns for us. r0 is scratch across
 * an exception entry, and this is the shape nrf_sd_isr.h documents.
 *
 * These override the weak stubs in the startup files, so the vector table
 * entries stay chip-specific while the contract lives in one place.
 * ------------------------------------------------------------------------ */
    .macro SD_FORWARD name, offset
    .thumb_func
    .global \name
    .type   \name, %function
\name:
    ldr     r0, =__softdevice_addr
    ldr     r0, [r0, #\offset]
    bx      r0
    .size   \name, .-\name
    .endm

    SD_FORWARD SWI00_IRQHandler,       NRF_SD_ISR_OFFSET_SWI00
    SD_FORWARD AAR00_CCM00_IRQHandler, NRF_SD_ISR_OFFSET_AAR00_CCM00
    SD_FORWARD ECB00_IRQHandler,       NRF_SD_ISR_OFFSET_ECB00
    SD_FORWARD TIMER10_IRQHandler,     NRF_SD_ISR_OFFSET_TIMER10
    SD_FORWARD RADIO_0_IRQHandler,     NRF_SD_ISR_OFFSET_RADIO_0
    SD_FORWARD GRTC_3_IRQHandler,      NRF_SD_ISR_OFFSET_GRTC_3
    SD_FORWARD CLOCK_POWER_IRQHandler, NRF_SD_ISR_OFFSET_CLOCK_POWER
