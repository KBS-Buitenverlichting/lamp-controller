/*
 * testing.c
 *
 *  Created on: May 6, 2025
 *      Author: Bjorn Wakker
 */
#include "testing.h"

const uint8_t (*test_functions[])(void) = {
	// LoRa transmit tests
	Tx_Clear_Buffer_Test,
	Tx_Set_Buffer_Test,
	Tx_Set_Ack_Test,
	// schedule tests
	Test_Schedules_RTC_DateTime_To_ScheduleTimestamp,
	Test_Schedules_ScheduleTimestamp_To_RTC_DateTime,
	Test_Schedules_Clear_ScheduleList,
	Test_Schedules_Insert_First,
	Test_Schedules_Insert_First_List_Full,
	Test_Schedules_Insert_After,
	Test_Schedules_Remove_From_Empty_List_Error,
	Test_Schedules_Remove_After
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

	APP_LOG(TS_OFF, VLEVEL_M, "Finished unit tests\r\n");
}
