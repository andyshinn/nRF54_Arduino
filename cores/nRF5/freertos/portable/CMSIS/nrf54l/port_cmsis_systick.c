/*
 * FreeRTOS Kernel V10.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "nrf_nvic.h"

#define ROUNDED_DIV(A, B) (((A) + ((B) / 2)) / (B))

#ifdef SOFTDEVICE_PRESENT
#include "nrf_soc.h"
#endif

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM33 port.
 * CMSIS compatible layer to manage tick source using GRTC.
 *
 * GRTC SYSCOUNTER runs at 1 MHz (configSYSTICK_CLOCK_HZ = 1000000).
 * We use a compare channel to generate tick interrupts.
 *----------------------------------------------------------*/
#if configUSE_16_BIT_TICKS == 1
#error This port does not support 16 bit ticks.
#endif

/*-----------------------------------------------------------*/

/* Read the low 32 bits of the GRTC SYSCOUNTER.
 * SYSCOUNTER is 52-bit but we only need 32-bit for tick counting. */
static inline uint32_t grtc_counter_get(void)
{
    /* Reading SYSCOUNTERL latches SYSCOUNTERH for coherent 64-bit read,
     * but we only need the low 32 bits. */
    return (uint32_t)(portNRF_GRTC_REG->SYSCOUNTER[0].SYSCOUNTERL);
}

/* Set compare channel value */
static inline void grtc_cc_set(uint32_t cc_channel, uint32_t val)
{
    portNRF_GRTC_REG->CC[cc_channel].CCL = val;
    portNRF_GRTC_REG->CC[cc_channel].CCH = 0;  /* High word = 0 for 32-bit compare */
}

/* Clear compare event */
static inline void grtc_event_compare_clear(uint32_t cc_channel)
{
    portNRF_GRTC_REG->EVENTS_COMPARE[cc_channel] = 0;
    /* Ensure event is cleared before ISR returns (ARM Cortex-M write buffer) */
    volatile uint32_t dummy = portNRF_GRTC_REG->EVENTS_COMPARE[cc_channel];
    (void)dummy;
}

/* Enable compare interrupt for channel */
static inline void grtc_int_compare_enable(uint32_t cc_channel)
{
    portNRF_GRTC_REG->INTENSET0 = (1UL << cc_channel);
}

/* Disable compare interrupt for channel */
static inline void grtc_int_compare_disable(uint32_t cc_channel)
{
    portNRF_GRTC_REG->INTENCLR0 = (1UL << cc_channel);
}

/*-----------------------------------------------------------*/

void xPortSysTickHandler( void )
{
    traceISR_ENTER();

    /* Clear compare event */
    grtc_event_compare_clear(portNRF_GRTC_CC_CH);

    BaseType_t switch_req = pdFALSE;
    uint32_t isrstate = portSET_INTERRUPT_MASK_FROM_ISR();

    uint32_t systick_counter = grtc_counter_get();

    if (configUSE_DISABLE_TICK_AUTO_CORRECTION_DEBUG == 0)
    {
        /* Auto-correct missed ticks.
         * GRTC runs at configSYSTICK_CLOCK_HZ (1 MHz).
         * Each OS tick = portNRF_GRTC_TICKS_PER_SYSTICK GRTC ticks. */
        TickType_t diff;
        uint32_t expected_counter = xTaskGetTickCount() * portNRF_GRTC_TICKS_PER_SYSTICK;
        diff = (systick_counter - expected_counter) / portNRF_GRTC_TICKS_PER_SYSTICK;

        /* At most 1 step if scheduler is suspended */
        if ((diff > 1) && (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING))
        {
            diff = 1;
        }
        while ((diff--) > 0)
        {
            switch_req |= xTaskIncrementTick();
        }
    }
    else
    {
        switch_req = xTaskIncrementTick();
    }

    /* Schedule next compare */
    {
        uint32_t next_cc = grtc_counter_get() + portNRF_GRTC_TICKS_PER_SYSTICK;
        grtc_cc_set(portNRF_GRTC_CC_CH, next_cc);
    }

    /* Increment the RTOS tick as usual which checks if there is a need for rescheduling */
    if ( switch_req != pdFALSE )
    {
        traceISR_EXIT_TO_SCHEDULER();
        /* A context switch is required.  Context switching is performed in
        the PendSV interrupt.  Pend the PendSV interrupt. */
        SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
        __SEV();
    }
    else
    {
        traceISR_EXIT();
    }

    portCLEAR_INTERRUPT_MASK_FROM_ISR( isrstate );
}

/*
 * Setup the GRTC compare channel to generate tick interrupts at the required
 * frequency.
 */
void vPortSetupTimerInterrupt( void )
{
    /* GRTC SYSCOUNTER is already running (started by SystemInit or bootloader).
     * We just need to set up a compare channel for periodic tick interrupts. */

    /* Clear any pending event */
    grtc_event_compare_clear(portNRF_GRTC_CC_CH);

    /* Set first compare value */
    uint32_t now = grtc_counter_get();
    grtc_cc_set(portNRF_GRTC_CC_CH, now + portNRF_GRTC_TICKS_PER_SYSTICK);

    /* Enable compare interrupt */
    grtc_int_compare_enable(portNRF_GRTC_CC_CH);

    NVIC_SetPriority(portNRF_GRTC_IRQn, configKERNEL_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(portNRF_GRTC_IRQn);
}

#if configUSE_TICKLESS_IDLE == 1

void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
    TickType_t enterTime;

    /* Make sure the expected idle time does not overflow the counter. */
    if ( xExpectedIdleTime > portNRF_GRTC_MAXTICKS - configEXPECTED_IDLE_TIME_BEFORE_SLEEP )
    {
        xExpectedIdleTime = portNRF_GRTC_MAXTICKS - configEXPECTED_IDLE_TIME_BEFORE_SLEEP;
    }

    /* Block all the interrupts globally */
#ifdef SOFTDEVICE_PRESENT
    do{
        uint8_t dummy = 0;
        uint32_t err_code = sd_nvic_critical_region_enter(&dummy);
        (void) err_code;
    }while (0);
#else
    __disable_irq();
#endif

    enterTime = grtc_counter_get();

    if ( eTaskConfirmSleepModeStatus() != eAbortSleep )
    {
        TickType_t xModifiableIdleTime;
        /* Convert OS ticks to GRTC ticks for wakeup time */
        uint32_t wakeupTime = (enterTime + xExpectedIdleTime * portNRF_GRTC_TICKS_PER_SYSTICK) & portNRF_GRTC_MAXTICKS;

        /* Disable periodic tick interrupt, use compare for wakeup */
        grtc_int_compare_disable(portNRF_GRTC_CC_CH);

        /* Configure compare for wakeup */
        grtc_cc_set(portNRF_GRTC_CC_CH, wakeupTime);
        grtc_event_compare_clear(portNRF_GRTC_CC_CH);
        grtc_int_compare_enable(portNRF_GRTC_CC_CH);

        __DSB();

        xModifiableIdleTime = xExpectedIdleTime;
        configPRE_SLEEP_PROCESSING( xModifiableIdleTime );
        if ( xModifiableIdleTime > 0 )
        {
#if (__FPU_USED == 1)
            /* Clear FPU flags to prevent it from keeping CPU awake */
            __set_FPSCR(__get_FPSCR() & ~(0x0000009F));
            (void) __get_FPSCR();
  #ifdef FPU_IRQn
            /* nRF52 routes FPU exceptions through a dedicated NVIC line;
             * clear any pending FPU interrupt before entering sleep so it
             * doesn't wake the CPU spuriously. Cortex-M33 (nRF54L) has no
             * FPU_IRQn - FPU exceptions go through UsageFault instead, so
             * this NVIC clear has nothing to do. */
            NVIC_ClearPendingIRQ(FPU_IRQn);
  #endif
#endif

            {
                /* S145 does not provide sd_app_evt_wait(); use WFE directly.
                 * BASEPRI cannot be used because it would prevent WFE from waking up. */
                do{
                    __WFE();
                } while (0 == (NVIC->ISPR[0] | NVIC->ISPR[1] | NVIC->ISPR[2] | NVIC->ISPR[3]
                             | NVIC->ISPR[4] | NVIC->ISPR[5] | NVIC->ISPR[6] | NVIC->ISPR[7]));
            }
        }
        configPOST_SLEEP_PROCESSING( xExpectedIdleTime );

        grtc_int_compare_disable(portNRF_GRTC_CC_CH);
        grtc_event_compare_clear(portNRF_GRTC_CC_CH);

        /* Correct the system ticks */
        {
            TickType_t diff;
            TickType_t exitTime;

            exitTime = grtc_counter_get();
            /* Convert GRTC ticks back to OS ticks */
            diff = ((exitTime - enterTime) & portNRF_GRTC_MAXTICKS) / portNRF_GRTC_TICKS_PER_SYSTICK;

            /* Re-enable periodic tick via compare */
            uint32_t next_cc = grtc_counter_get() + portNRF_GRTC_TICKS_PER_SYSTICK;
            grtc_cc_set(portNRF_GRTC_CC_CH, next_cc);
            grtc_event_compare_clear(portNRF_GRTC_CC_CH);
            grtc_int_compare_enable(portNRF_GRTC_CC_CH);

            /* It is important that we clear pending here so that our corrections are latest and in sync with tick_interrupt handler */
            NVIC_ClearPendingIRQ(portNRF_GRTC_IRQn);

            if ((configUSE_TICKLESS_IDLE_SIMPLE_DEBUG) && (diff > xExpectedIdleTime))
            {
                diff = xExpectedIdleTime;
            }

            BaseType_t switch_req = pdFALSE;

            if (diff > 1)
            {
                vTaskStepTick(diff - 1);

                // If dwt cycle count is enabled, adjust it as well
                if ( (CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk) && (DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) )
                {
                  DWT->CYCCNT += (((diff-1) * 1000000) / configTICK_RATE_HZ) * (SystemCoreClock / 1000000);
                }

                switch_req = xTaskIncrementTick();
            }
            else if (diff == 1)
            {
                switch_req = xTaskIncrementTick();
            }

            if ( switch_req != pdFALSE )
            {
                SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
                __SEV();
            }
        }
    }
#ifdef SOFTDEVICE_PRESENT
    uint32_t err_code = sd_nvic_critical_region_exit(0);
    (void) err_code;
#else
    __enable_irq();
#endif
}

#endif // configUSE_TICKLESS_IDLE
