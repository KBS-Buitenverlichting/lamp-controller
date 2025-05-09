/*
 * testing.c
 *
 *  Created on: May 6, 2025
 *      Author: Bjorn Wakker
 */
#include "testing.h"

const uint8_t (*test_functions[])(void) = {
	Tx_Clear_Buffer_Test,
	Tx_Set_Buffer_Test,
	Tx_Set_Ack_Test
};

void Main_Test(void)
{
	APP_LOG(TS_OFF, VLEVEL_M, "Starting unit tests\r\n");

	// Go through all unit tests and report their status
	for (uint8_t i = 0; i < sizeof(test_functions) / sizeof(test_functions[0]); i++)
	{
		const uint8_t result = test_functions[i]();

		if (result == 0)
		{
			APP_LOG(TS_OFF, VLEVEL_M, "Test %d succeeded\r\n", i);
		}
		else
		{
			APP_LOG(TS_OFF, VLEVEL_M, "Test %d failed on test case %d\r\n", i, result);
		}
	}
}
