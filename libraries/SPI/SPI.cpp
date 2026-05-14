/*
 * SPI Master library for nRF5x.
 * Copyright (c) 2015 Arduino LLC
 * Copyright (c) 2016 Sandeep Mistry All right reserved.
 * Copyright (c) 2019 Ha Thach for Adafruit Industries. All right reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "SPI.h"
#include <Arduino.h>
#include <wiring_private.h>
#include <assert.h>

// Maximum DMA transfer chunk size
#define SPI_DMA_CHUNK  65535

static nrf_spim_mode_t _arduino_to_nrf_mode(uint8_t mode)
{
  switch (mode) {
    default:
    case SPI_MODE0: return NRF_SPIM_MODE_0;
    case SPI_MODE1: return NRF_SPIM_MODE_1;
    case SPI_MODE2: return NRF_SPIM_MODE_2;
    case SPI_MODE3: return NRF_SPIM_MODE_3;
  }
}

static nrf_spim_bit_order_t _arduino_to_nrf_bitorder(uint8_t bitOrder)
{
  return (bitOrder == LSBFIRST) ? NRF_SPIM_BIT_ORDER_LSB_FIRST
                                : NRF_SPIM_BIT_ORDER_MSB_FIRST;
}

SPIClass::SPIClass(NRF_SPIM_Type *p_spi, uint8_t uc_pinMISO, uint8_t uc_pinSCK, uint8_t uc_pinMOSI)
{
  initialized = false;
  assert(p_spi != NULL);

  _p_spim = p_spi;

  _uc_pinMiso = g_ADigitalPinMap[uc_pinMISO];
  _uc_pinSCK = g_ADigitalPinMap[uc_pinSCK];
  _uc_pinMosi = g_ADigitalPinMap[uc_pinMOSI];

  _dataMode = SPI_MODE0;
  _bitOrder = MSBFIRST;
}

void SPIClass::_configure(uint8_t mode, uint8_t bitOrder)
{
  nrf_spim_configure(_p_spim,
                     _arduino_to_nrf_mode(mode),
                     _arduino_to_nrf_bitorder(bitOrder));
}

void SPIClass::_setFrequency(uint32_t freq)
{
  // nRF54L SPIM uses PRESCALER register: SPI_CLK = PCLK / PRESCALER
  // PCLK is typically 128 MHz for nRF54L
  // Prescaler must be even, minimum 2
  uint32_t prescaler;

  if (freq >= 32000000) {
    prescaler = 4;       // 32 MHz
  } else if (freq >= 16000000) {
    prescaler = 8;       // 16 MHz
  } else if (freq >= 8000000) {
    prescaler = 16;      // 8 MHz
  } else if (freq >= 4000000) {
    prescaler = 32;      // 4 MHz
  } else if (freq >= 2000000) {
    prescaler = 64;      // 2 MHz
  } else if (freq >= 1000000) {
    prescaler = 128;     // 1 MHz
  } else if (freq >= 500000) {
    prescaler = 256;     // 500 kHz
  } else if (freq >= 250000) {
    prescaler = 512;     // 250 kHz
  } else {
    prescaler = 1024;    // 125 kHz
  }

  nrf_spim_prescaler_set(_p_spim, prescaler);
}

void SPIClass::begin()
{
  if (initialized) return;
  initialized = true;

  // Configure pins
  nrf_spim_pins_set(_p_spim, _uc_pinSCK, _uc_pinMosi, _uc_pinMiso);

  // CSN not used (managed externally)
  _p_spim->PSEL.CSN = NRF_SPIM_PIN_NOT_CONNECTED;

  // Configure SPI mode and bit order
  _configure(_dataMode, _bitOrder);

  // Default 4 MHz
  _setFrequency(4000000);

  // Over-read character
  nrf_spim_orc_set(_p_spim, 0xFF);

  // Enable
  nrf_spim_enable(_p_spim);

  // High drive for SCK and MOSI
  nrf_gpio_cfg(_uc_pinSCK,
               NRF_GPIO_PIN_DIR_OUTPUT,
               NRF_GPIO_PIN_INPUT_CONNECT,
               NRF_GPIO_PIN_NOPULL,
               NRF_GPIO_PIN_H0H1,
               NRF_GPIO_PIN_NOSENSE);

  nrf_gpio_cfg(_uc_pinMosi,
               NRF_GPIO_PIN_DIR_OUTPUT,
               NRF_GPIO_PIN_INPUT_DISCONNECT,
               NRF_GPIO_PIN_NOPULL,
               NRF_GPIO_PIN_H0H1,
               NRF_GPIO_PIN_NOSENSE);
}

void SPIClass::end()
{
  nrf_spim_disable(_p_spim);
  nrf_spim_pins_set(_p_spim, NRF_SPIM_PIN_NOT_CONNECTED,
                    NRF_SPIM_PIN_NOT_CONNECTED,
                    NRF_SPIM_PIN_NOT_CONNECTED);
  initialized = false;
}

void SPIClass::usingInterrupt(int /*interruptNumber*/)
{
}

void SPIClass::beginTransaction(SPISettings settings)
{
  nrf_spim_disable(_p_spim);

  this->_dataMode = settings.dataMode;
  this->_bitOrder = settings.bitOrder;

  _configure(_dataMode, _bitOrder);
  _setFrequency(settings.clockFreq);

  nrf_spim_enable(_p_spim);
}

void SPIClass::endTransaction(void)
{
  // Nothing to do — keep peripheral enabled between transactions
}

void SPIClass::setPins(uint8_t uc_pinMISO, uint8_t uc_pinSCK, uint8_t uc_pinMOSI)
{
  _uc_pinMiso = g_ADigitalPinMap[uc_pinMISO];
  _uc_pinSCK = g_ADigitalPinMap[uc_pinSCK];
  _uc_pinMosi = g_ADigitalPinMap[uc_pinMOSI];
}

void SPIClass::setBitOrder(BitOrder order)
{
  this->_bitOrder = order;
  _configure(_dataMode, _bitOrder);
}

void SPIClass::setDataMode(uint8_t mode)
{
  this->_dataMode = mode;
  _configure(_dataMode, _bitOrder);
}

void SPIClass::setClockDivider(uint32_t div)
{
  // Convert divider to frequency assuming F_CPU
  uint32_t freq = F_CPU / div;
  _setFrequency(freq);
}

void SPIClass::transfer(const void *tx_buf, void *rx_buf, size_t count)
{
  const uint8_t* tx_buf8 = (const uint8_t*) tx_buf;
  uint8_t* rx_buf8 = (uint8_t*) rx_buf;

  while (count)
  {
    size_t xfer_len = min(count, (size_t) SPI_DMA_CHUNK);

    // Set up DMA buffers via HAL
    nrf_spim_tx_buffer_set(_p_spim, tx_buf8, tx_buf8 ? xfer_len : 0);
    nrf_spim_rx_buffer_set(_p_spim, rx_buf8, rx_buf8 ? xfer_len : 0);

    // Clear event and start
    nrf_spim_event_clear(_p_spim, NRF_SPIM_EVENT_END);
    nrf_spim_task_trigger(_p_spim, NRF_SPIM_TASK_START);

    // Wait for completion
    while (!nrf_spim_event_check(_p_spim, NRF_SPIM_EVENT_END)) { /* spin */ }
    nrf_spim_event_clear(_p_spim, NRF_SPIM_EVENT_END);

    count -= xfer_len;
    if (tx_buf8) tx_buf8 += xfer_len;
    if (rx_buf8) rx_buf8 += xfer_len;
  }
}

void SPIClass::transfer(void *buf, size_t count)
{
  transfer(buf, buf, count);
}

byte SPIClass::transfer(uint8_t data)
{
  transfer(&data, 1);
  return data;
}

uint16_t SPIClass::transfer16(uint16_t data) {

  union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } t;

  t.val = data;

  if (_bitOrder == LSBFIRST) {
    t.lsb = transfer(t.lsb);
    t.msb = transfer(t.msb);
  } else {
    t.msb = transfer(t.msb);
    t.lsb = transfer(t.lsb);
  }

  return t.val;
}

void SPIClass::attachInterrupt() {
  // Should be enableInterrupt()
}

void SPIClass::detachInterrupt() {
  // Should be disableInterrupt()
}

// nRF54L SPI instances: SPIM00 (mapped from NRF_SPIM0 via compat) and SPIM21 (mapped from NRF_SPIM2)
#if SPI_INTERFACES_COUNT >= 1
SPIClass SPI(NRF_SPIM0,  PIN_SPI_MISO,  PIN_SPI_SCK,  PIN_SPI_MOSI);
#endif

#if SPI_INTERFACES_COUNT >= 2
SPIClass SPI1(NRF_SPIM2, PIN_SPI1_MISO, PIN_SPI1_SCK, PIN_SPI1_MOSI);
#endif
