/*
 * transmit_test.h
 *
 *  Created on: Apr 25, 2025
 *      Author: Bjorn Wakker
 */
#include "transmit.h"

uint8_t tx_buffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];
uint8_t tx_buffer_size = 0;
static LmHandlerAppData_t tx_app_data = { LORAWAN_USER_APP_PORT, 0, tx_buffer };

void Tx_Set_Buffer(const uint8_t identifier, const uint8_t subtype, const uint8_t* const parameters, const uint8_t nr_of_parameter_bytes)
{
	// Check if the message is not to long
	if (nr_of_parameter_bytes > LORAWAN_APP_DATA_BUFFER_MAX_SIZE)
	{
		return;
	}

	// Copy all data into the buffer
	tx_buffer[0] = identifier;
	tx_buffer[1] = subtype;

	for (uint8_t i = 0; i < nr_of_parameter_bytes; i++)
	{
		tx_buffer[2 + i] = parameters[i];
	}

	tx_buffer_size = 2 + nr_of_parameter_bytes;
}

void Tx_Transmit_Data(void)
{
	// Update the size
	tx_app_data.BufferSize = tx_buffer_size;

	// Send the data
	UTIL_TIMER_Time_t next_tx_in = 0;
	if (LmHandlerSend(&tx_app_data, LORAWAN_DEFAULT_CONFIRMED_MSG_STATE, &next_tx_in, false) != LORAMAC_HANDLER_SUCCESS)
	{
	    return;
	}
}
