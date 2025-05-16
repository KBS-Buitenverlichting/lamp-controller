/*
 * schedules_test.h
 *
 *  Created on: May 16, 2025
 *      Author: jacco
 */

#pragma once

#include "schedules.h"


uint8_t Test_Schedules_ScheduleTimestamp_To_RTC_DateTime(void);
uint8_t Test_Schedules_RTC_DateTime_To_ScheduleTimestamp(void);
uint8_t Test_Schedules_Clear_ScheduleList(void);
uint8_t Test_Schedules_Insert_First(void);
uint8_t Test_Schedules_Insert_First_List_Full(void);
uint8_t Test_Schedules_Insert_After(void);
uint8_t Test_Schedules_Remove_From_Empty_List_Error(void);
uint8_t Test_Schedules_Remove_After(void);
