/*********************************************************************
 * @file   transmit.c
 * @brief  File for handling LoRa transmission
 *
 * @author KBS Buitenverlichting
 * @date   25 April 2025
 *********************************************************************/
#include "transmit.h"

uint8_t tx_buffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];
uint8_t tx_buffer_size = 0;
static LmHandlerAppData_t tx_app_data = { LORAWAN_USER_APP_PORT, 0, tx_buffer };

void Tx_Clear_Buffer(void) {
	if (tx_buffer_size > LORAWAN_APP_DATA_BUFFER_MAX_SIZE) {
		tx_buffer_size = LORAWAN_APP_DATA_BUFFER_MAX_SIZE;
	}

	for (uint8_t i = 0; i < tx_buffer_size; i++) {
		tx_buffer[i] = 0;
	}

	tx_buffer_size = 0;
}

void Tx_Set_Buffer(const Identifier identifier, const uint8_t subtype, const uint8_t *const parameters, const uint8_t nr_of_parameter_bytes) {
	// Check if the message is not to long
	if (nr_of_parameter_bytes > LORAWAN_APP_DATA_BUFFER_MAX_SIZE) {
		return;
	}

	// Copy all data into the buffer
	tx_buffer[IDENTIFIER_BYTE] = identifier;
	tx_buffer[SUBTYPE_BYTE] = subtype;

	for (uint8_t i = 0; i < nr_of_parameter_bytes; i++) {
		tx_buffer[PARAMETERS_START_BYTE + i] = parameters[i];
	}

	tx_buffer_size = MESSAGE_MIN_BYTES + nr_of_parameter_bytes;
}

void Tx_Set_Ack(const InstructionSubtype instruction) {
	const uint8_t params[] = { instruction };
	Tx_Set_Buffer(RESPONSE_OUT, INSTRUCTION_COMPLETED, (const uint8_t* const ) &params, 1);
}

void Tx_Transmit_Data(void) {
	tx_app_data.BufferSize = tx_buffer_size;

	// Send the data, clear buffer on successfull send
	if (LmHandlerSend(&tx_app_data, LORAWAN_DEFAULT_CONFIRMED_MSG_STATE, NULL, false) == LORAMAC_HANDLER_SUCCESS) {
		Tx_Clear_Buffer();
	}
}
