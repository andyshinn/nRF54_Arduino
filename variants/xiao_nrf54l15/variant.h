/*
  XIAO nRF54L15 variant for Adafruit nRF54L Arduino core

  Pin mapping derived from Seeed Studio XIAO nRF54L15 schematic.
*/

#ifndef _VARIANT_XIAO_NRF54L15_
#define _VARIANT_XIAO_NRF54L15_

/** Master clock frequency */
#define VARIANT_MCK       (128000000ul)

#define USE_LFXO      // Board uses 32khz crystal for LF

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "WVariant.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// Number of pins defined in PinDescription array
#define PINS_COUNT           (24)
#define NUM_DIGITAL_PINS     (24)
#define NUM_ANALOG_INPUTS    (8)
#define NUM_ANALOG_OUTPUTS   (0)

/*
 * LEDs
 * LED_BUILTIN: P2.00 (active low)
 */
#define PIN_LED1             (16)  // P2.00

#define LED_BUILTIN          PIN_LED1
#define LED_RED              PIN_LED1
#define LED_STATE_ON         0

/*
 * Button
 */
#define PIN_BUTTON1          (17)  // P0.00

/*
 * Analog pins — mapped to D0..D7 which correspond to SAADC-capable pins
 * (The XIAO maps A0..A7 to D0..D7)
 */
#define PIN_A0               (0)
#define PIN_A1               (1)
#define PIN_A2               (2)
#define PIN_A3               (3)
#define PIN_A4               (4)
#define PIN_A5               (5)
#define PIN_A6               (6)
#define PIN_A7               (7)

static const uint8_t A0  = PIN_A0;
static const uint8_t A1  = PIN_A1;
static const uint8_t A2  = PIN_A2;
static const uint8_t A3  = PIN_A3;
static const uint8_t A4  = PIN_A4;
static const uint8_t A5  = PIN_A5;
static const uint8_t A6  = PIN_A6;
static const uint8_t A7  = PIN_A7;
#define ADC_RESOLUTION    14

#define PIN_AREF           (0xff)

/*
 * Serial interfaces
 */

// Serial1 — default UART on D6 (TX) / D7 (RX)
#define PIN_SERIAL1_TX      (6)   // P2.08
#define PIN_SERIAL1_RX      (7)   // P2.07

/*
 * SPI Interfaces
 */
#define SPI_INTERFACES_COUNT 1

#define PIN_SPI_MOSI         (10)  // D10 = P2.02
#define PIN_SPI_MISO         (9)   // D9 = P2.04
#define PIN_SPI_SCK          (8)   // D8 = P2.01

static const uint8_t SS   = 2;     // D2 = P1.06
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

/*
 * Wire Interfaces
 *
 * Route Wire to TWIM22, a dedicated TWI controller, instead of the default
 * NRF_TWIM0 alias which lands on SERIAL20 (shared with SPIM20/UARTE20). This
 * matches the NRF54L15-Clean-Arduino-core XIAO convention and keeps I2C free
 * of the SERIALx fabric used by SPI/Serial1 on the P2 header pins.
 */
#define WIRE_INTERFACES_COUNT 1

#define PIN_WIRE_SDA         (4)   // D4 = P1.10
#define PIN_WIRE_SCL         (5)   // D5 = P1.11

#define WIRE_TWIM            NRF_TWIM22
#define WIRE_TWIS            NRF_TWIS22
#define WIRE_IRQN            SERIAL22_IRQn
#define WIRE_IRQ_HANDLER     SERIAL22_IRQHandler

/*
 * Board control pins
 */
#define PIN_SAMD11_RX        (18)  // P1.09 — SAMD11 USB bridge RX (nRF TX)
#define PIN_SAMD11_TX        (19)  // P1.08 — SAMD11 USB bridge TX (nRF RX)
#define PIN_IMU_MIC_PWR      (20)  // P0.01 — Sense IMU+MIC power enable
#define PIN_RF_SW            (21)  // P2.03 — RF switch power enable
#define PIN_RF_SW_CTL        (22)  // P2.05 — RF path select (0=ceramic, 1=ext)
#define PIN_VBAT_EN          (23)  // P1.15 — VBAT divider enable
#define PIN_VBAT_READ        PIN_A7  // P1.14 — VBAT divider sense

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#endif
