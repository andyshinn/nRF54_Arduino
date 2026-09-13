#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Route the SoftDevice-owned interrupts to the s145 image at sd_base; call before sd_softdevice_enable().
void sd_isr_forwarding_enable(uint32_t sd_base);
void sd_isr_forwarding_disable(void);
void sd_isr_boot_init(void);

#ifdef __cplusplus
}
#endif
