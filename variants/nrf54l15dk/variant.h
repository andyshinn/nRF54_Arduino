/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.
  Copyright (c) 2016 Sandeep Mistry All right reserved.
  Copyright (c) 2018, Adafruit Industries (adafruit.com)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _VARIANT_NRF54L15DK_
#define _VARIANT_NRF54L15DK_

/** Master clock frequency */
#define VARIANT_MCK       (128000000ul)

#define USE_LFXO      // Board uses 32khz crystal for LF
// define USE_LFRC    // Board uses RC for LF

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "WVariant.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*
 * nRF54L15 DK Pin Map: Arduino pin = physical GPIO number.
 *   P0.n = n, P1.n = 32 + n, P2.n = 64 + n
 *
 * Serial peripherals are bound to a GPIO port on nRF54L:
 *   SERIAL00 (UARTE00/SPIM00) -> P2, SERIAL2x -> P1, SERIAL30 -> P0.
 */

// Number of pins defined in PinDescription array
#define PINS_COUNT           (96)
#define NUM_DIGITAL_PINS     (96)
#define NUM_ANALOG_INPUTS    (8)
#define NUM_ANALOG_OUTPUTS   (0)

// LEDs (active low): LED0 P2.09, LED1 P1.10, LED2 P2.07, LED3 P1.14
#define PIN_LED1             (73)
#define PIN_LED2             (42)
#define PIN_LED3             (71)
#define PIN_LED4             (46)

#define LED_BUILTIN          PIN_LED1
#define LED_CONN             PIN_LED2

#define LED_RED              PIN_LED1
#define LED_BLUE             PIN_LED2

#define LED_STATE_ON         0         // State when LED is lit (active low)

// Buttons (active low): BTN0 P1.13, BTN1 P1.09, BTN2 P1.08, BTN3 P0.04
#define PIN_BUTTON1          (45)
#define PIN_BUTTON2          (41)
#define PIN_BUTTON3          (40)
#define PIN_BUTTON4          (4)

// Analog: AIN0..AIN3 = P1.04..P1.07, AIN4..AIN7 = P1.11..P1.14
#define PIN_A0               (36)
#define PIN_A1               (37)
#define PIN_A2               (38)
#define PIN_A3               (39)
#define PIN_A4               (43)
#define PIN_A5               (44)
#define PIN_A6               (45)
#define PIN_A7               (46)

static const uint8_t A0  = PIN_A0;
static const uint8_t A1  = PIN_A1;
static const uint8_t A2  = PIN_A2;
static const uint8_t A3  = PIN_A3;
static const uint8_t A4  = PIN_A4;
static const uint8_t A5  = PIN_A5;
static const uint8_t A6  = PIN_A6;
static const uint8_t A7  = PIN_A7;
#define ADC_RESOLUTION    14

// Other pins
#define PIN_AREF           (0xff)  // No external AREF on nRF54L

/*
 * Serial interfaces
 */

// Serial1: VCOM0 on the on-board J-Link (UARTE20): TX P1.04, RX P1.05
#define PIN_SERIAL1_RX      (37)
#define PIN_SERIAL1_TX      (36)
#define SERIAL1_UARTE       NRF_UARTE20
#define SERIAL1_IRQN        SERIAL20_IRQn
#define SERIAL1_IRQ_HANDLER SERIAL20_IRQHandler

// Serial2: VCOM1 on the on-board J-Link (UARTE30): TX P0.00, RX P0.01
#define PIN_SERIAL2_RX      (1)
#define PIN_SERIAL2_TX      (0)
#define SERIAL2_UARTE       NRF_UARTE30
#define SERIAL2_IRQN        SERIAL30_IRQn
#define SERIAL2_IRQ_HANDLER SERIAL30_IRQHandler

/*
 * SPI Interfaces (SPIM00, shared by the on-board MX25R64 and the Arduino header)
 */
#define SPI_INTERFACES_COUNT 1

#define PIN_SPI_MISO         (68)  // P2.04
#define PIN_SPI_MOSI         (66)  // P2.02
#define PIN_SPI_SCK          (65)  // P2.01

static const uint8_t SS   = 69;    // P2.05 (MX25R64 CS)
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

/*
 * Wire Interfaces (TWIM22 on the Arduino header)
 */
#define WIRE_INTERFACES_COUNT 1

#define PIN_WIRE_SDA         (43)  // P1.11
#define PIN_WIRE_SCL         (44)  // P1.12

#define WIRE_TWIM            NRF_TWIM22
#define WIRE_TWIS            NRF_TWIS22
#define WIRE_IRQN            SERIAL22_IRQn
#define WIRE_IRQ_HANDLER     SERIAL22_IRQHandler

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#endif
