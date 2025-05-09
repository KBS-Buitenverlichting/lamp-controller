/*
 * transmit_test.c
 *
 *  Created on: May 6, 2025
 *      Author: Bjorn Wakker
 */
#include "transmit_test.h"

extern uint8_t tx_buffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];
extern uint8_t tx_buffer_size;

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
	Tx_Set_Buffer(0xC1, 0x00, &param, 1);

	if (tx_buffer_size != 3)
		return 21;

	if (tx_buffer[0] != 0xC1 || tx_buffer[1] != 0x00 || tx_buffer[2] != 0x80)
		return 22;

	// Test case 3
	const uint8_t params[] = { 0x00, 0x01, 0x02 };
	Tx_Set_Buffer(0xFF, 0x05, &params, 3);

	if (tx_buffer_size != 5)
		return 31;

	if (tx_buffer[0] != 0xFF || tx_buffer[1] != 0x05 || tx_buffer[2] != 0x00 || tx_buffer[3] != 0x01 || tx_buffer[4] != 0x02)
		return 32;

	return 0;
}

