/*
 * Startup File for nRF54L15 (Cortex-M33)
 *
 * This file contains:
 * - Interrupt vector table
 * - Reset handler (copies .data, zeroes .bss, calls main)
 * - Default exception/interrupt handlers
 *
 * Licensed under the Apache License 2.0
 */

/* The platform builds all of cores/nRF5 as one library, so every startup
 * file here is assembled for every target. Only one may contribute a
 * vector table. */
#if defined(NRF54L05_XXAA) || defined(NRF54L10_XXAA) || defined(NRF54L15_XXAA)

    .syntax unified
    .cpu cortex-m33
    .thumb
    .fpu fpv5-sp-d16

    .global g_pfnVectors
    .global __isr_vector
    .extern __libc_init_array
    .extern __StackTop

/* *****************************************************************************
 * Vector Table
 * *****************************************************************************/

    .section .isr_vector, "a"
    .align 4
    .type g_pfnVectors, %object

__isr_vector:
g_pfnVectors:
    .word   __StackTop              /* 0: Initial Stack Pointer */
    .word   Reset_Handler           /* 1: Reset Handler */
    .word   NMI_Handler             /* 2: NMI Handler */
    .word   HardFault_Handler       /* 3: Hard Fault Handler */
    .word   MemManage_Handler       /* 4: MPU Fault Handler */
    .word   BusFault_Handler        /* 5: Bus Fault Handler */
    .word   UsageFault_Handler      /* 6: Usage Fault Handler */
    .word   SecureFault_Handler     /* 7: Secure Fault Handler */
    .word   0                       /* 8: Reserved */
    .word   0                       /* 9: Reserved */
    .word   0                       /* 10: Reserved */
    .word   SVC_Handler             /* 11: SVCall Handler */
    .word   DebugMon_Handler        /* 12: Debug Monitor Handler */
    .word   0                       /* 13: Reserved */
    .word   PendSV_Handler          /* 14: PendSV Handler */
    .word   SysTick_Handler         /* 15: SysTick Handler */

    /* External Interrupts (nRF54L15 specific). */
    /* IRQ 0..69 */
    .rept 70
    .word   Default_Handler
    .endr
    /* IRQ 70..71: AAR00/CCM00 and ECB00. */
    .word   AAR00_CCM00_IRQHandler
    .word   ECB00_IRQHandler
    /* IRQ 72..73 */
    .rept 2
    .word   Default_Handler
    .endr
    /* IRQ 74: SERIAL00 (UARTE00/SPIM00/TWIM00) */
    .word   SERIAL00_IRQHandler
    /* IRQ 75..148 */
    .rept 74
    .word   Default_Handler
    .endr
    /* IRQ 149..150: TWIM/TWIS instances used by Wire target IRQ mode. */
    .word   TWIM20_IRQHandler
    .word   TWIM21_IRQHandler
    /* IRQ 151..217 */
    .rept 67
    .word   Default_Handler
    .endr
    /* IRQ 218..219: GPIOTE20 IRQ lines used by attachInterrupt(). */
    .word   GPIOTE20_0_IRQHandler
    .word   GPIOTE20_1_IRQHandler
    /* IRQ 220 */
    .word   Default_Handler
    /* IRQ 221: I2S20 */
    .word   I2S20_IRQHandler
    /* IRQ 222..223 */
    .rept 2
    .word   Default_Handler
    .endr
    /* IRQ 224..225: QDEC20, QDEC21 */
    .word   QDEC20_IRQHandler
    .word   QDEC21_IRQHandler
    /* IRQ 226..228: GRTC interrupt groups. */
    .word   GRTC_0_IRQHandler
    .word   GRTC_1_IRQHandler
    .word   GRTC_2_IRQHandler
    /* IRQ 229..239 */
    .rept 11
    .word   Default_Handler
    .endr

    .size g_pfnVectors, .-g_pfnVectors

/* *****************************************************************************
 * Reset Handler
 * *****************************************************************************/

    .section .text.Reset_Handler
    .global Reset_Handler
    .type Reset_Handler, %function
Reset_Handler:
    /* Disable interrupts */
    cpsid i

    /* Claim the vector table.
     *
     * On nRF52 the MBR lives at address 0 and forwards every exception, so an
     * application entered from a bootloader can leave VTOR alone. nRF54L has
     * no MBR. nRF54_Bootloader's boot stub points VTOR at the bootloader's own
     * table and its app-start path never moves it, so an application that does
     * not claim VTOR here keeps running the bootloader's handlers -- which
     * kills FreeRTOS at the first context switch, because vPortStartFirstTask's
     * SVC and the following PendSV/SysTick dispatch into the wrong image.
     *
     * Note this image's table is at the BOTTOM of RRAM (FLASH origin 0x1000)
     * and the SoftDevice sits at the top, the inverse of the nRF52 layout.
     * Do not "correct" this toward nRF52's SoftDevice-at-MBR_SIZE convention.
     *
     * SystemInit is not the place for it: the weak stub at the end of this
     * file is overridden by the MDK's system_nrf54l.c, which never touches
     * VTOR. */
    ldr r0, =__isr_vector
    ldr r1, =0xE000ED08          /* SCB->VTOR */
    str r0, [r1]
    dsb
    isb

    /* Copy .data section from FLASH to RAM. */
    ldr r0, =__etext         /* SRC: .data load address in FLASH */
    ldr r1, =__data_start__  /* DST: .data start in RAM */
    ldr r2, =__data_end__    /* DST: .data end in RAM */

LoopCopyData:
    cmp r1, r2
    bcs LoopCopyDataDone
    ldr r3, [r0], #4
    str r3, [r1], #4
    b LoopCopyData

LoopCopyDataDone:
    /* Zero out .bss section. */
    ldr r0, =__bss_start__
    ldr r1, =__bss_end__
    mov r2, #0

LoopClearBss:
    cmp r0, r1
    bcs RunMain
    str r2, [r0], #4
    b LoopClearBss

RunMain:
    /* Configure clocks/system state if overridden by the core. */
    bl SystemInit

    /* Run C/C++ runtime init so global constructors are executed. */
    bl __libc_init_array

    /* Enable interrupts */
    cpsie i

    /* Call main() */
    bl main

    /* main() should never return, but if it does, loop forever */
InfiniteLoop:
    b InfiniteLoop

    .size Reset_Handler, .-Reset_Handler

/* *****************************************************************************
 * Exception Handlers
 * *****************************************************************************/

    .weak NMI_Handler
    .type NMI_Handler, %function
NMI_Handler:
    b .
    .size NMI_Handler, .-NMI_Handler

    .weak HardFault_Handler
    .type HardFault_Handler, %function
HardFault_Handler:
    b .
    .size HardFault_Handler, .-HardFault_Handler

    .weak MemManage_Handler
    .type MemManage_Handler, %function
MemManage_Handler:
    b .
    .size MemManage_Handler, .-MemManage_Handler

    .weak BusFault_Handler
    .type BusFault_Handler, %function
BusFault_Handler:
    b .
    .size BusFault_Handler, .-BusFault_Handler

    .weak UsageFault_Handler
    .type UsageFault_Handler, %function
UsageFault_Handler:
    b .
    .size UsageFault_Handler, .-UsageFault_Handler

    .weak SecureFault_Handler
    .type SecureFault_Handler, %function
SecureFault_Handler:
    b .
    .size SecureFault_Handler, .-SecureFault_Handler

    .weak SVC_Handler
    .type SVC_Handler, %function
SVC_Handler:
    b .
    .size SVC_Handler, .-SVC_Handler

    .weak DebugMon_Handler
    .type DebugMon_Handler, %function
DebugMon_Handler:
    b .
    .size DebugMon_Handler, .-DebugMon_Handler

    .weak PendSV_Handler
    .type PendSV_Handler, %function
PendSV_Handler:
    b .
    .size PendSV_Handler, .-PendSV_Handler

    /* FreeRTOS uses the GRTC tickless port on nRF54L, not SysTick, so
     * SysTick_Handler is unused. Define a weak no-op so the vector slot
     * at offset 0x3C resolves; any code that does want a SysTick handler
     * (e.g. a bare-metal user sketch) can override this. */
    .weak SysTick_Handler
    .type SysTick_Handler, %function
SysTick_Handler:
    bx lr
    .size SysTick_Handler, .-SysTick_Handler

/* *****************************************************************************
 * Default Interrupt Handler
 * *****************************************************************************/

    .weak AAR00_CCM00_IRQHandler
    .type AAR00_CCM00_IRQHandler, %function
AAR00_CCM00_IRQHandler:
    bx lr
    .size AAR00_CCM00_IRQHandler, .-AAR00_CCM00_IRQHandler

    .weak ECB00_IRQHandler
    .type ECB00_IRQHandler, %function
ECB00_IRQHandler:
    bx lr
    .size ECB00_IRQHandler, .-ECB00_IRQHandler

    .weak TWIM20_IRQHandler
    .type TWIM20_IRQHandler, %function
TWIM20_IRQHandler:
    b .
    .size TWIM20_IRQHandler, .-TWIM20_IRQHandler

    .weak TWIM21_IRQHandler
    .type TWIM21_IRQHandler, %function
TWIM21_IRQHandler:
    b .
    .size TWIM21_IRQHandler, .-TWIM21_IRQHandler

    .weak SERIAL00_IRQHandler
    .type SERIAL00_IRQHandler, %function
SERIAL00_IRQHandler:
    bx lr
    .size SERIAL00_IRQHandler, .-SERIAL00_IRQHandler

    .weak GRTC_0_IRQHandler
    .type GRTC_0_IRQHandler, %function
GRTC_0_IRQHandler:
    b .
    .size GRTC_0_IRQHandler, .-GRTC_0_IRQHandler

    .weak QDEC20_IRQHandler
    .type QDEC20_IRQHandler, %function
QDEC20_IRQHandler:
    bx lr
    .size QDEC20_IRQHandler, .-QDEC20_IRQHandler

    .weak QDEC21_IRQHandler
    .type QDEC21_IRQHandler, %function
QDEC21_IRQHandler:
    bx lr
    .size QDEC21_IRQHandler, .-QDEC21_IRQHandler

    .weak I2S20_IRQHandler
    .type I2S20_IRQHandler, %function
I2S20_IRQHandler:
    b .
    .size I2S20_IRQHandler, .-I2S20_IRQHandler

    .weak GRTC_1_IRQHandler
    .type GRTC_1_IRQHandler, %function
GRTC_1_IRQHandler:
    b .
    .size GRTC_1_IRQHandler, .-GRTC_1_IRQHandler

    .weak GRTC_2_IRQHandler
    .type GRTC_2_IRQHandler, %function
GRTC_2_IRQHandler:
    b .
    .size GRTC_2_IRQHandler, .-GRTC_2_IRQHandler

    /* GPIOTE20_0_IRQHandler is provided by WInterrupts.c (via the
     * GPIOTE_IRQHandler alias in nrf54l_compat.h). GPIOTE20_1 is the
     * second IRQ line of the same controller; attachInterrupt() doesn't
     * route through it today, but the vector slot still has to resolve. */
    .weak GPIOTE20_1_IRQHandler
    .type GPIOTE20_1_IRQHandler, %function
GPIOTE20_1_IRQHandler:
    b .
    .size GPIOTE20_1_IRQHandler, .-GPIOTE20_1_IRQHandler

    .weak Default_Handler
    .type Default_Handler, %function
Default_Handler:
    b .
    .size Default_Handler, .-Default_Handler

/* *****************************************************************************
 * System Init (optional - can be overridden)
 * *****************************************************************************/

    .weak SystemInit
    .type SystemInit, %function
SystemInit:
    bx lr
    .size SystemInit, .-SystemInit

#endif
