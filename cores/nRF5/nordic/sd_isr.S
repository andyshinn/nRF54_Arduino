/* SoftDevice interrupt forwarding for s145 on nRF54L.
 * The application owns the vector table; the SoftDevice publishes its handler addresses at
 * its base address (nrf_sd_isr.h) and the application jumps there while forwarding is enabled.
 * Modelled on sdk-nrf-bm subsys/softdevice_handler/irq_forward.s. */

    .syntax unified
    .arch armv8-m.main
    .thumb

    .extern sd_isr_forward_base
    .extern sd_isr_forward_enabled
    .extern vPortSVCHandler

    .equ SD_ISR_MAGIC,                  0x47F34BC1
    .equ NRF_SD_ISR_OFFSET_RESET,       0x0000
    .equ NRF_SD_ISR_OFFSET_SVC,         0x0008
    .equ NRF_SD_ISR_OFFSET_SWI00,       0x000c
    .equ NRF_SD_ISR_OFFSET_AAR00_CCM00, 0x0010
    .equ NRF_SD_ISR_OFFSET_ECB00,       0x0014
    .equ NRF_SD_ISR_OFFSET_TIMER10,     0x0018
    .equ NRF_SD_ISR_OFFSET_RADIO_0,     0x001c
    .equ NRF_SD_ISR_OFFSET_GRTC_3,      0x0020
    .equ NRF_SD_ISR_OFFSET_CLOCK_POWER, 0x0024
    .equ SD_SVC_BASE,                   0x10

    .section .text.sd_isr, "ax"

/* r0 = SoftDevice vector offset. Forwards when enabled, otherwise returns from the exception. */
    .thumb_func
    .align 2
sd_isr_forward:
    ldr     r2, =sd_isr_forward_enabled
    ldr     r2, [r2]
    ldr     r1, =SD_ISR_MAGIC
    cmp     r2, r1
    bne     1f
    ldr     r1, =sd_isr_forward_base
    ldr     r1, [r1]
    ldr     r1, [r1, r0]
    bx      r1
1:  bx      lr

/* void sd_isr_call_reset_handler(void): must run once before forwarding is enabled. */
    .thumb_func
    .align 2
    .globl  sd_isr_call_reset_handler
    .type   sd_isr_call_reset_handler, %function
sd_isr_call_reset_handler:
    push    {lr}
    ldr     r1, =sd_isr_forward_base
    ldr     r1, [r1]
    ldr     r1, [r1, #NRF_SD_ISR_OFFSET_RESET]
    blx     r1
    pop     {pc}
    .size   sd_isr_call_reset_handler, . - sd_isr_call_reset_handler

/* SVC 0x00-0x0F belong to the application (FreeRTOS uses 0), the rest to the SoftDevice. */
    .thumb_func
    .align 2
    .globl  SVC_Handler
    .type   SVC_Handler, %function
SVC_Handler:
    tst     lr, #4
    ite     eq
    mrseq   r0, msp
    mrsne   r0, psp
    ldr     r1, [r0, #24]
    ldrb    r1, [r1, #-2]
    cmp     r1, #SD_SVC_BASE
    bhs     1f
    ldr     r2, =vPortSVCHandler
    bx      r2
1:  ldr     r2, =sd_isr_forward_base
    ldr     r2, [r2]
    ldr     r2, [r2, #NRF_SD_ISR_OFFSET_SVC]
    bx      r2
    .size   SVC_Handler, . - SVC_Handler

    .macro sd_vector name, offset
    .thumb_func
    .align 2
    .globl  \name
    .type   \name, %function
\name:
    ldr     r0, =\offset
    b       sd_isr_forward
    .size   \name, . - \name
    .endm

    sd_vector SWI00_IRQHandler,       NRF_SD_ISR_OFFSET_SWI00
    sd_vector AAR00_CCM00_IRQHandler, NRF_SD_ISR_OFFSET_AAR00_CCM00
    sd_vector ECB00_IRQHandler,       NRF_SD_ISR_OFFSET_ECB00
    sd_vector TIMER10_IRQHandler,     NRF_SD_ISR_OFFSET_TIMER10
    sd_vector RADIO_0_IRQHandler,     NRF_SD_ISR_OFFSET_RADIO_0
    sd_vector GRTC_3_IRQHandler,      NRF_SD_ISR_OFFSET_GRTC_3
    sd_vector CLOCK_POWER_IRQHandler, NRF_SD_ISR_OFFSET_CLOCK_POWER

    .end
