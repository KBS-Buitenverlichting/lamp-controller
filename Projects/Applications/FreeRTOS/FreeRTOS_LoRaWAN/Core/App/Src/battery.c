/*
 * battery.c
 *
 *  Created on: May 9, 2025
 *      Author: Bjorn Wakker
 */
#include <stdio.h>
#include "battery.h"

SemaphoreHandle_t sem_start_battery_read;

static uint16_t battery_min_vref = 0; // Battery voltage when empty
static uint16_t battery_max_vref = UINT16_MAX; // Battery voltage when full
static bool vrefs_initialized = false; // Keeps track of if the voltage vrefs have been initialized

void Start_Get_Battery_Level_Task(void const *argument) {
	const uint8_t MAX17048_READ_ADDR = 0x6C;
	const uint8_t MAX17048_REG_SOC = 0x04;
	const uint8_t BUFFER_SIZE = 2;

	uint8_t rx_buffer[BUFFER_SIZE];
	sem_start_battery_read = xSemaphoreCreateBinary();
	for(;;) {
		// wait for signal to start read
		if(xSemaphoreTake(sem_start_battery_read, portMAX_DELAY) != pdPASS) {
			Error_Handler();
		}
		// send read request for address
		if (HAL_I2C_Mem_Read(&hi2c2, MAX17048_READ_ADDR, MAX17048_REG_SOC, I2C_MEMADD_SIZE_8BIT, (uint8_t*) rx_buffer, BUFFER_SIZE, 100) != HAL_OK) {
			Error_Handler();
		}
		//HAL_I2C_Master_Receive(&hi2c2, MAX17048_READ_ADDR, (uint8_t *)rx_buffer, BUFFER_SIZE,1000); //Receiving in Blocking mode
		const uint8_t params[] = { SEND_BATTERY_STATUS, rx_buffer[0], rx_buffer[1]};
		Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION, (const uint8_t* const)&params, sizeof(params));
	}
}


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
