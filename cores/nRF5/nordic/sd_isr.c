#include "sd_isr.h"
#include "nrf.h"
#include <stdbool.h>

#define SD_ISR_MAGIC 0x47F34BC1u

extern uint32_t __softdevice_start__;

// SVCs are always forwarded (sd_softdevice_is_enabled() etc. are used before BLE init), IRQs only once enabled.
uint32_t sd_isr_forward_base = (uint32_t)&__softdevice_start__;
uint32_t sd_isr_forward_enabled = 0;
static bool sd_reset_done = false;

extern void sd_isr_call_reset_handler(void);

// The SoftDevice reset handler must run once before any SVC reaches it; init() calls this at boot.
void sd_isr_boot_init(void)
{
    if (sd_reset_done) return;
    sd_isr_call_reset_handler();
    sd_reset_done = true;
}

void sd_isr_forwarding_enable(uint32_t sd_base)
{
    sd_isr_forward_base = sd_base;

    // Priorities the SoftDevice expects for its interrupts (nRF54L: 3 priority bits).
    NVIC_SetPriority(RADIO_0_IRQn, 0);
    NVIC_SetPriority(TIMER10_IRQn, 0);
    NVIC_SetPriority(GRTC_3_IRQn, 0);
    NVIC_SetPriority(AAR00_CCM00_IRQn, 4);
    NVIC_SetPriority(CLOCK_POWER_IRQn, 4);
    NVIC_SetPriority(ECB00_IRQn, 4);
    NVIC_SetPriority(SWI00_IRQn, 4);
    NVIC_SetPriority(SVCall_IRQn, 4);

    sd_isr_boot_init();
    sd_isr_forward_enabled = SD_ISR_MAGIC;
}

void sd_isr_forwarding_disable(void)
{
    sd_isr_forward_enabled = 0;
}
