/*
 * nRF52 -> nRF54L compatibility mappings
 *
 * Force-included via -include in build flags.
 * Maps legacy nRF52 peripheral names used by Adafruit core code
 * to nRF54L equivalents.
 *
 * Note: Nordic ships mdk/nrf54l15_name_change.h as an empty placeholder
 * (see cores/nRF5/nordic/nrfx/mdk/nrf54l15_name_change.h - just the
 * BSD-3 boilerplate, no #defines). Every nRF52->nRF54L alias has to
 * live in this file; there is nothing to dedup against upstream.
 *
 * Scope: symbol-name aliases ONLY (e.g. NRF_TWIM0 -> NRF_TWIM20).
 * Register-shape changes (split-bank INTENSETn, split DRIVE0/DRIVE1
 * fields) cannot be aliased portably because the nrfx HAL uses
 * #if defined(...) checks on the legacy macros as chip-detection
 * markers - aliasing fools the HAL into taking the wrong branch.
 * Rewrite the source in those cases (see WInterrupts.c port and
 * wiring_digital.c port for examples).
 */

#ifndef NRF54L_COMPAT_H__
#define NRF54L_COMPAT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Peripheral instance remapping ---- */

/* PWM: nRF52 PWM0-2 -> nRF54L PWM20-22 */
#ifndef NRF_PWM0
#define NRF_PWM0            NRF_PWM20
#endif
#ifndef NRF_PWM1
#define NRF_PWM1            NRF_PWM21
#endif
#ifndef NRF_PWM2
#define NRF_PWM2            NRF_PWM22
#endif

/* SPI: nRF52 SPIM0/2 -> nRF54L SPIM00/21 */
#ifndef NRF_SPIM0
#define NRF_SPIM0           NRF_SPIM00
#endif
#ifndef NRF_SPIM2
#define NRF_SPIM2           NRF_SPIM21
#endif

/* I2C: nRF52 TWIM0/1 -> nRF54L TWIM20/22 */
#ifndef NRF_TWIM0
#define NRF_TWIM0           NRF_TWIM20
#endif
#ifndef NRF_TWIM1
#define NRF_TWIM1           NRF_TWIM22
#endif

/* GPIOTE */
#ifndef NRF_GPIOTE
#define NRF_GPIOTE          NRF_GPIOTE20
#endif

/* QDEC: nRF52 has NRF_QDEC, nRF54L has NRF_QDEC20/NRF_QDEC21 */
#ifndef NRF_QDEC
#define NRF_QDEC            NRF_QDEC20
#endif

/* ---- IRQ name remapping ---- */

/* GPIOTE_IRQn / GPIOTE_IRQHandler follow GPIOTE_IRQ_GROUP, see WInterrupts.c */

/* I2C / Wire: nRF52 combined IRQ -> nRF54L SERIAL IRQ */
#ifndef SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn
#define SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn       SERIAL20_IRQn
#endif
#ifndef SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler
#define SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler SERIAL20_IRQHandler
#endif
#ifndef SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn
#define SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn       SERIAL22_IRQn
#endif
#ifndef SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler
#define SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler SERIAL22_IRQHandler
#endif

/* TWIS: nRF52 TWIS0/1 -> nRF54L TWIS20/22 */
#ifndef NRF_TWIS0
#define NRF_TWIS0           NRF_TWIS20
#endif
#ifndef NRF_TWIS1
#define NRF_TWIS1           NRF_TWIS22
#endif

/* CRYPTOCELL does not exist on nRF54L (uses CRACEN instead) */
#ifdef NRF_CRYPTOCELL
#undef NRF_CRYPTOCELL
#endif

/* QDEC IRQ */
#ifndef QDEC_IRQn
#define QDEC_IRQn           QDEC20_IRQn
#endif
#ifndef QDEC_IRQHandler
#define QDEC_IRQHandler     QDEC20_IRQHandler
#endif

#ifdef __cplusplus
}
#endif

#endif /* NRF54L_COMPAT_H__ */
