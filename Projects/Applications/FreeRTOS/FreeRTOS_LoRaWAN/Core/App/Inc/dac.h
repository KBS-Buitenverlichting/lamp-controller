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
#include "main.h"


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
 */
void DAC_Set_Value(const uint16_t value);

/**
 * @brief Normalizes the brightness to the led its voltage range and calls DAC_Set_Value to set the brightness
 *
 * @see DAC_Set_Value
 */
void DAC_Set_Brightness(const uint8_t brightness);
