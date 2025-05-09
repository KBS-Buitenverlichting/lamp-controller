/*
 * battery.c
 *
 *  Created on: May 9, 2025
 *      Author: Bjorn Wakker
 */

#include "battery.h"

Warning Set_Battery_Vref(const uint16_t min_vref, const uint16_t max_vref)
{
	battery_min_vref = min_vref;
	battery_max_vref = max_vref;

	if (min_vref < VDD_MIN)
	{
		return MIN_VREF_BELOW_CUTOFF_BOARD;
	}
	else if (min_vref > VDD_MAX)
	{
		return MIN_VREF_ABOVE_MAX_VDD_BOARD;
	}
	else if (max_vref > VDD_MAX)
	{
		return MAX_VREF_ABOVE_MAX_VDD_BOARD;
	}

	return NO_WARNING;
}

uint8_t Get_Battery_Percentage(void)
{
	const uint16_t min = (battery_min_vref < VDD_MIN) ? VDD_MIN : battery_min_vref
	const uint16_t max = (battery_max_vref > VDD_MAX) ? VDD_MAX : battery_max_vref;
	const uint16_t battery_level_mv = SYS_GetBatteryLevel();

	if (battery_level_mv > max)
	{
		return UINT8_MAX;
	}

	if (battery_level_mv < min)
	{
		return 0;
	}

	return (((uint32_t)(batteryLevelmV - min) * UINT8_MAX) / (max - min));
}
