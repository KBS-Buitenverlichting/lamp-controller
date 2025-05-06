/*
 * transmit_test.c
 *
 *  Created on: May 6, 2025
 *      Author: Bjorn Wakker
 */
#include "transmit_test.h"
#include "transmit.h"
#include "LmHandler.h"
#include "lora_app.h"
#include "CayenneLpp.h"

extern uint8_t tx_buffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];
extern uint8_t tx_buffer_idx;

// Initialization function used for all unit tests
void Transmit_Test_Init(void)
{
	tx_buffer_idx = 0;

	for (uint8_t i = 0; i < LORAWAN_APP_DATA_BUFFER_MAX_SIZE; i++)
	{
		tx_buffer[i] = 0;
	}
}

uint8_t Tx_Reset_Buffer_Idx_Test(void)
{
	Transmit_Test_Init();

	// Test case 1, test lower boundary
	tx_buffer_idx = 0;
	Tx_Reset_Buffer_Idx();

	if (tx_buffer_idx != 0)
		return 1;

	// Test case 2, test random value
	tx_buffer_idx = 53;
	Tx_Reset_Buffer_Idx();

	if (tx_buffer_idx != 0)
		return 2;

	// Test case 3, test upper boundary
	tx_buffer_idx = 255;
	Tx_Reset_Buffer_Idx();

	if (tx_buffer_idx != 0)
		return 3;

	return 0;
}

uint8_t Tx_Add_Data_Test(void)
{
	Transmit_Test_Init();
	struct LppData data;

	// Test case 1, test a digital value
	data.sensor_id = 0;
	data.data_type = LPP_DIGITAL_OUTPUT;
	data.data.digital_value = 0xD2;

	if (!Tx_Add_Data(&data))
		return 11; // Err on 1.1

	if (tx_buffer_idx != 3)
		return 12; // Err on 1.2

	if (tx_buffer[2] != data.data.digital_value)
		return 13; // Err on 1.3

	// Test case 2, test an analog value
	data.sensor_id = 0;
	data.data_type = LPP_ANALOG_OUTPUT;
	data.data.analog_value = 0xAFAF;

	if (!Tx_Add_Data(&data))
		return 21;

	if (tx_buffer_idx != 7)
		return 22;

	data.data.analog_value *= 100; // Multiply by 100 for formatting

	if (tx_buffer[5] != ((data.data.analog_value >> 8) & 0xFF) || tx_buffer[6] != (data.data.analog_value & 0xFF))
		return 23;

	// Test case 3, test overwrite
	data.sensor_id = 0;
	data.data_type = LPP_DIGITAL_INPUT;
	data.data.digital_value = 0x0E;

	tx_buffer_idx = 0;

	if (!Tx_Add_Data(&data))
		return 31;

	if (tx_buffer_idx != 3)
		return 32;

	if (tx_buffer[2] != data.data.digital_value)
		return 33;

	// Test case 4, test overflow
	data.sensor_id = 0;
	data.data_type = LPP_DIGITAL_INPUT;
	data.data.digital_value = 0x0E;

	tx_buffer_idx = LORAWAN_APP_DATA_BUFFER_MAX_SIZE - 2;

	if (Tx_Add_Data(&data))
		return 4;

	return 0;
}
