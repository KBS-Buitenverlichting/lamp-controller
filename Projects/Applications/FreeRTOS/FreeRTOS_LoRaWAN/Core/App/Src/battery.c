/*
 * battery.c
 *
 *  Created on: May 9, 2025
 *      Author: Bjorn Wakker
 */
#include "battery.h"

SemaphoreHandle_t sem_start_battery_read;

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
