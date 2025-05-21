/*
 * dac.h
 *
 *  Created on: May 16, 2025
 *      Author: Bjorn Wakker
 */
#pragma once

#include "stdint.h"
#include "stm32wle5xx.h"
#include "stm32wlxx_hal.h"
#include "adc_if.h"
#include "transmit.h"
#include "lamp_state.h"

#define DAC_MAX 4096
#define LED_VMIN 1700 // 1.7V
#define LED_VMAX 2000 // 2.0V

/**
 * @brief Sets the EN1 bit in the DAC_CR
 */
void DAC_Enable();

/**
 * @brief Resets the EN1 bit in the DAC_RC
 */
void DAC_Disable();

/**
 * @brief Initializes the DAC
 */
void DAC_Init(void);

/**
 * @brief Normalizes the input value to the stm voltage range and sets it in the DAC register
 *
 * @param value The voltage (in mv) to set
 *
 * @return A warning may be generated if 'value' is higher than the board its power supply
 */
Warning DAC_Set_Value(const uint16_t value);

/**
 * @brief Normalizes the brightness to the led its voltage range and calls DAC_Set_Value to set the brightness
 *
 * @param brightness The brightness to set, where 255 is 100%
 *
 * @return A warning may be generated in DAC_Set_Value, this will be passed on
 *
 * @see DAC_Set_Value
 */
Warning DAC_Set_Brightness(const uint8_t brightness);
