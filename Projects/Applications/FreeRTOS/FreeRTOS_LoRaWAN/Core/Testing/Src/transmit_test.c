/*
 * transmit_test.c
 *
 *  Created on: May 6, 2025
 *      Author: Bjorn Wakker
 */
#include "transmit_test.h"

extern uint8_t tx_buffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];
extern uint8_t tx_buffer_size;

uint8_t Tx_Clear_Buffer_Test(void)
{
	// Test case 1
	tx_buffer_size = 2;
	tx_buffer[0] = 5;
	tx_buffer[1] = 10;

	Tx_Clear_Buffer();

	if (tx_buffer_size != 0)
		return 11;

	if (tx_buffer[0] != 0 || tx_buffer[1] != 0)
		return 12;

	// Test case 2
	tx_buffer_size = 255;
	tx_buffer[0] = 5;
	tx_buffer[90] = 10;
	tx_buffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE - 1] = 15;

	Tx_Clear_Buffer();

	if (tx_buffer_size != 0)
		return 21;

	if (tx_buffer[0] != 0 || tx_buffer[90] != 0 || tx_buffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE - 1] != 0)
		return 22;

	return 0;
}

uint8_t Tx_Set_Buffer_Test(void)
{
	// Test case 1
	Tx_Set_Buffer(0xC0, 0x00, 0, 0);

	if (tx_buffer_size != 2)
		return 11;

	if (tx_buffer[0] != 0xC0 || tx_buffer[1] != 0x00)
		return 12;

	// Test case 2
	const uint8_t param[] = { 0x80 };
	Tx_Set_Buffer(0xC1, 0x00, (const uint8_t* const)&param, 1);

	if (tx_buffer_size != 3)
		return 21;

	if (tx_buffer[0] != 0xC1 || tx_buffer[1] != 0x00 || tx_buffer[2] != 0x80)
		return 22;

	// Test case 3
	const uint8_t params[] = { 0x00, 0x01, 0x02 };
	Tx_Set_Buffer(0xFF, 0x05, (const uint8_t* const)&params, 3);

	if (tx_buffer_size != 5)
		return 31;

	if (tx_buffer[0] != 0xFF || tx_buffer[1] != 0x05 || tx_buffer[2] != 0x00 || tx_buffer[3] != 0x01 || tx_buffer[4] != 0x02)
		return 32;

	return 0;
}

uint8_t Tx_Set_Ack_Test(void)
{
	// Test case 1
	Tx_Set_Ack(LAMP_ON);

	if (tx_buffer_size != 3)
		return 11;

	if (tx_buffer[IDENTIFIER_BYTE] != RESPONSE_OUT || tx_buffer[SUBTYPE_BYTE] != INSTRUCTION_COMPLETED || tx_buffer[PARAMETERS_START_BYTE] != LAMP_ON)
		return 12;

	return 0;
}
