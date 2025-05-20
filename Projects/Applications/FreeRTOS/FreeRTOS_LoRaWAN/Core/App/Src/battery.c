/*
 * battery.c
 *
 *  Created on: May 9, 2025
 *      Author: Bjorn Wakker
 */
#include "battery.h"

static uint16_t battery_min_vref = 0; // Battery voltage when empty
static uint16_t battery_max_vref = UINT16_MAX; // Battery voltage when full
static bool vrefs_initialized = false; // Keeps track of if the voltage vrefs have been initialized

Warning Set_Battery_Vref(const uint16_t min_vref, const uint16_t max_vref)
{
	battery_min_vref = min_vref;
	battery_max_vref = max_vref;
	vrefs_initialized = true;

	if (min_vref < VDD_MIN) // Possibly not enough charge for the board
	{
		return MIN_VREF_BELOW_CUTOFF_BOARD;
	}
	else if (min_vref > VDD_MAX) // Inaccuracy in battery level calculation (always 0%)
	{
		return MIN_VREF_ABOVE_MAX_VDD_BOARD;
	}
	else if (max_vref > VDD_MAX) // Inaccuracy in battery level calculation (extended period of 100%, not linearly decreasing)
	{
		return MAX_VREF_ABOVE_MAX_VDD_BOARD;
	}

	return NO_WARNING;
}

bool Vrefs_Initialized(void)
{
	return vrefs_initialized;
}

uint8_t Get_Battery_Level(void)
{
	// Decide what to use for voltage reference
	const uint16_t min = (battery_min_vref < VDD_MIN) ? VDD_MIN : battery_min_vref;
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

	return (((uint32_t)(battery_level_mv - min) * UINT8_MAX) / (max - min));
}
