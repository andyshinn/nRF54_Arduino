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
/* For GRTC_SYSCOUNTER / GRTC_INTENSET / GRTC_INTENCLR, which resolve to this
 * domain's registers via GRTC_IRQ_GROUP, and for the register field names. */
#include "nrf_grtc.h"

#define ROUNDED_DIV(A, B) (((A) + ((B) / 2)) / (B))

#ifdef SOFTDEVICE_PRESENT
#include "nrf_soc.h"
#endif

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM33 port.
 * CMSIS compatible layer to manage tick source using GRTC.
 *
 * The GRTC SYSCOUNTER is a 52-bit counter fixed at 1 MHz on nRF54L. CLKCFG
 * does not scale it: CLKFASTDIV divides the CLKOUT pin, and CLKSEL only picks
 * which LFCLK keeps time while the counter is asleep. A compare channel
 * generates the tick.
 *----------------------------------------------------------*/
#if configUSE_16_BIT_TICKS == 1
#error This port does not support 16 bit ticks.
#endif

/* portNRF_GRTC_TICKS_PER_SYSTICK is derived from configSYSTICK_CLOCK_HZ, so a
 * mismatch here would give a tick that runs but runs at the wrong rate --
 * much harder to spot than one that does not run at all. */
#if configSYSTICK_CLOCK_HZ != NRF_GRTC_SYSCOUNTER_MAIN_FREQUENCY_HZ
#error configSYSTICK_CLOCK_HZ does not match the GRTC SYSCOUNTER frequency
#endif

/*-----------------------------------------------------------*/

/* Read this domain's SYSCOUNTER.
 *
 * Reading SYSCOUNTERL latches SYSCOUNTERH, so the pair is coherent, but the
 * result is only usable once BUSY clears -- the counter reports BUSY while it
 * is waking from its low-power state -- and OVERFLOW says the low word wrapped
 * between the two reads. Retry on either, exactly as nrfy_grtc_sys_counter_get
 * does. A single non-retried read is what made the frozen counter look like a
 * legitimate zero. */
static inline uint64_t grtc_counter_get(void)
{
    uint32_t counter_l, counter_h;

    do
    {
        counter_l = portNRF_GRTC_REG->GRTC_SYSCOUNTER.SYSCOUNTERL;
        counter_h = portNRF_GRTC_REG->GRTC_SYSCOUNTER.SYSCOUNTERH;
    } while (counter_h & (GRTC_SYSCOUNTER_SYSCOUNTERH_BUSY_Msk |
                          GRTC_SYSCOUNTER_SYSCOUNTERH_OVERFLOW_Msk));

    return ((uint64_t)(counter_h & GRTC_SYSCOUNTER_SYSCOUNTERH_VALUE_Msk) << 32) | counter_l;
}

/* Tick bookkeeping is 32-bit modular arithmetic on the bottom of the counter. */
static inline uint32_t grtc_counter32_get(void)
{
    return (uint32_t)grtc_counter_get();
}

/* Set compare channel value.
 *
 * The compare is against the full 52-bit SYSCOUNTER, so CCH has to carry the
 * high bits: pinning it to zero would stop the tick dead once the counter
 * passed 2^32 us, about 71 minutes after boot. CCEN gates the channel and is
 * dropped across the update so a stale value cannot match mid-write. */
static inline void grtc_cc_set(uint32_t cc_channel, uint64_t val)
{
    portNRF_GRTC_REG->CC[cc_channel].CCEN = GRTC_CC_CCEN_ACTIVE_Disable;
    portNRF_GRTC_REG->CC[cc_channel].CCL  = (uint32_t)val;
    portNRF_GRTC_REG->CC[cc_channel].CCH  = (uint32_t)(val >> 32) & NRF_GRTC_SYSCOUNTER_CCH_MASK;
    portNRF_GRTC_REG->CC[cc_channel].CCEN = GRTC_CC_CCEN_ACTIVE_Enable;
}

/* Clear compare event */
static inline void grtc_event_compare_clear(uint32_t cc_channel)
{
    portNRF_GRTC_REG->EVENTS_COMPARE[cc_channel] = 0;
    /* Ensure event is cleared before ISR returns (ARM Cortex-M write buffer) */
    volatile uint32_t dummy = portNRF_GRTC_REG->EVENTS_COMPARE[cc_channel];
    (void)dummy;
}

/* Enable compare interrupt for channel.
 * GRTC_INTENSET/GRTC_INTENCLR resolve to this domain's INTENSETn/INTENCLRn. */
static inline void grtc_int_compare_enable(uint32_t cc_channel)
{
    portNRF_GRTC_REG->GRTC_INTENSET = (1UL << cc_channel);
}

/* Disable compare interrupt for channel */
static inline void grtc_int_compare_disable(uint32_t cc_channel)
{
    portNRF_GRTC_REG->GRTC_INTENCLR = (1UL << cc_channel);
}

/* Bring the SYSCOUNTER up.
 *
 * This port used to assume "the SYSCOUNTER is already running, started by
 * SystemInit or the bootloader". Nothing starts it: SystemInit only touches
 * NSACR/CPACR and trim, and the bootloader leaves the GRTC alone. The counter
 * sat at zero with BUSY set forever, so the tick compare could never match and
 * every delay() blocked for good.
 *
 * The sequence follows nrfx_grtc_init() + nrfx_grtc_syscounter_start(): sleep
 * behaviour first, then TASKS_START, then MODE.SYSCOUNTEREN, then wait for the
 * read port to come out of BUSY. */
static void grtc_syscounter_start(void)
{
    if ((portNRF_GRTC_REG->MODE & GRTC_MODE_SYSCOUNTEREN_Msk) == 0)
    {
        /* MODE, TIMEOUT and WAKETIME are global to the peripheral, so only
         * touch them if nobody has started it already. The low-frequency
         * reference is the system LFCLK, which wiring.c's init() starts well
         * before the scheduler. TIMEOUT/WAKETIME are nrfx's defaults. */
        portNRF_GRTC_REG->CLKCFG = (portNRF_GRTC_REG->CLKCFG & ~GRTC_CLKCFG_CLKSEL_Msk) |
                                   (GRTC_CLKCFG_CLKSEL_SystemLFCLK << GRTC_CLKCFG_CLKSEL_Pos);

        portNRF_GRTC_REG->MODE     = (GRTC_MODE_AUTOEN_CpuActive << GRTC_MODE_AUTOEN_Pos);
        portNRF_GRTC_REG->TIMEOUT  = 5;
        portNRF_GRTC_REG->WAKETIME = 4;

        portNRF_GRTC_REG->TASKS_START = 1;
        __DSB();

        portNRF_GRTC_REG->MODE |= (GRTC_MODE_SYSCOUNTEREN_Enabled << GRTC_MODE_SYSCOUNTEREN_Pos);
        __DSB();
    }

    /* Hold this domain's request so the SYSCOUNTER stays awake. It has to:
     * the compare that ends tickless idle is the only thing that wakes us, so
     * letting the counter stop when the CPU sleeps would deadlock the tick.
     * The cost is idle current, and reclaiming it means driving ACTIVE around
     * WFE together with WAKETIME rather than dropping this line. */
    portNRF_GRTC_REG->GRTC_SYSCOUNTER.ACTIVE = GRTC_SYSCOUNTER_ACTIVE_ACTIVE_Active;
    __DSB();

    /* Wait for the read port to produce values. The dummy SYSCOUNTERL read is
     * required: it is what latches SYSCOUNTERH. */
    for ( ;; )
    {
        (void)portNRF_GRTC_REG->GRTC_SYSCOUNTER.SYSCOUNTERL;
        if ((portNRF_GRTC_REG->GRTC_SYSCOUNTER.SYSCOUNTERH &
             GRTC_SYSCOUNTER_SYSCOUNTERH_BUSY_Msk) == 0)
        {
            break;
        }
    }
}

/*-----------------------------------------------------------*/

void xPortSysTickHandler( void )
{
    traceISR_ENTER();

    /* Clear compare event */
    grtc_event_compare_clear(portNRF_GRTC_CC_CH);

    BaseType_t switch_req = pdFALSE;
    uint32_t isrstate = portSET_INTERRUPT_MASK_FROM_ISR();

    uint32_t systick_counter = grtc_counter32_get();

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
        uint64_t next_cc = grtc_counter_get() + portNRF_GRTC_TICKS_PER_SYSTICK;
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
    /* Start the SYSCOUNTER. Nothing else in the system does. */
    grtc_syscounter_start();

    /* Clear any pending event */
    grtc_event_compare_clear(portNRF_GRTC_CC_CH);

    /* Set first compare value */
    uint64_t now = grtc_counter_get();
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

    uint64_t enterTime64 = grtc_counter_get();
    enterTime = (TickType_t)enterTime64;

    if ( eTaskConfirmSleepModeStatus() != eAbortSleep )
    {
        TickType_t xModifiableIdleTime;
        /* Convert OS ticks to GRTC ticks for wakeup time. Full 52-bit value:
         * the compare is against the whole SYSCOUNTER, not just its low word. */
        uint64_t wakeupTime = enterTime64 +
                              (uint64_t)xExpectedIdleTime * portNRF_GRTC_TICKS_PER_SYSTICK;

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

            exitTime = grtc_counter32_get();
            /* Convert GRTC ticks back to OS ticks */
            diff = ((exitTime - enterTime) & portNRF_GRTC_MAXTICKS) / portNRF_GRTC_TICKS_PER_SYSTICK;

            /* Re-enable periodic tick via compare */
            uint64_t next_cc = grtc_counter_get() + portNRF_GRTC_TICKS_PER_SYSTICK;
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
