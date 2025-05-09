/*
 * battery.h
 *
 *  Created on: May 9, 2025
 *      Author: Bjorn Wakker
 */

#include "stdint.h"
#include "adc_if.h"
#include "transmit.h"

static uint16_t battery_min_vref = 0;
static uint16_t battery_max_vref = 0;

Warning Set_Battery_Vref(const uint16_t min_vref, const uint16_t max_vref);
uint8_t Get_Battery_Percentage(void);
