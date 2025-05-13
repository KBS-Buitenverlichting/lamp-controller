/*
 * battery.h
 *
 *  Created on: May 9, 2025
 *      Author: Bjorn Wakker
 */
#pragma once

#include "stdint.h"
#include "adc_if.h"
#include "transmit.h"

/**
 * @brief Sets the battery voltages references
 *
 * @param min_vref The minimum voltage reference
 * @param max_vref The maximum voltage reference
 *
 * @return A warning may be generated if the voltage references are not within the stm board specs
 */
Warning Set_Battery_Vref(const uint16_t min_vref, const uint16_t max_vref);

/**
 * @brief Gets the battery level
 *
 * @return The battery level, ranging from 0 (empty) to 255 (full)
 */
uint8_t Get_Battery_Level(void);
