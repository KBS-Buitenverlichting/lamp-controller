/*
 * schedules_test.c
 *
 *  Created on: May 16, 2025
 *      Author: jacco
 */

#include "schedules_test.h"

/// @brief create a basic schedule to test with
Schedule Create_Test_Schedule(void) {
	Schedule schedule = {0};
	ScheduleTimestamp timestamp = {
		.year = 25,
		.month = 5,
		.weekday = 3,
		.date = 16,
		.hours = 11,
		.minutes = 30,
		.seconds = 0
	};
	schedule.lamp_config = (LampConfig) {
		.state = ON,
		.brightness = 255
	};
	schedule.time_start = timestamp;
	schedule.time_end = timestamp;
	schedule.time_end.hours++;

	return schedule;
}

uint8_t Test_Schedules_ScheduleTimestamp_To_RTC_DateTime(void) {
	ScheduleTimestamp timestamp = {
		.year = 25,
		.month = 5,
		.weekday = 3,
		.date = 16,
		.hours = 11,
		.minutes = 30,
		.seconds = 0
	};
	RTC_DateTypeDef test_date;
	RTC_TimeTypeDef test_time;
	ScheduleTimestamp_To_RTC_DateTime(&timestamp, &test_date, &test_time);
	if (
		test_date.Year != timestamp.year ||
		test_date.Month != timestamp.month ||
		test_date.WeekDay != timestamp.weekday ||
		test_date.Date != timestamp.date ||
		test_time.Hours != timestamp.hours ||
		test_time.Minutes != timestamp.minutes ||
		test_time.Seconds != timestamp.seconds
	) {
		return 11; // fail on test case 1-1
	}
	return 0;
}

uint8_t Test_Schedules_RTC_DateTime_To_ScheduleTimestamp(void) {
	RTC_DateTypeDef test_date = {
		.Year = 25,
		.Month = 5,
		.WeekDay = 3,
		.Date = 16,
	};
	RTC_TimeTypeDef test_time = {
		.Hours = 11,
		.Minutes = 30,
		.Seconds = 0,
	};

	ScheduleTimestamp timestamp = RTC_DateTime_To_ScheduleTimestamp(&test_date, &test_time);

	if (
		test_date.Year != timestamp.year ||
		test_date.Month != timestamp.month ||
		test_date.WeekDay != timestamp.weekday ||
		test_date.Date != timestamp.date ||
		test_time.Hours != timestamp.hours ||
		test_time.Minutes != timestamp.minutes ||
		test_time.Seconds != timestamp.seconds
	) {
		return 11; // fail on test case 1-1
	}
	return 0;
}

uint8_t Test_Schedules_Clear_ScheduleList(void) {
	// test case 1
	ScheduleList_Clear();
	if (ScheduleList_Get_Size() == 0) {
		return 0;
	} else {
		return 1; // fail on test case 1
	}
}


uint8_t Test_Schedules_Insert_First(void) {
	// setup

	ScheduleList_Clear();
	Schedule schedule = Create_Test_Schedule();
	// test
	if (ScheduleList_Insert_First(schedule) != SCHEDULE_FUNC_OK) {
		return 11; // fail on test case 1-1
	}
	if (ScheduleList_Get_Size() != 1) {
		return 12; // fail on test case 1-2
	}
	return 0;
}

uint8_t Test_Schedules_Insert_First_List_Full(void) {
	// setup
	ScheduleList_Clear();
	Schedule schedule = Create_Test_Schedule();

	// test
	for(uint8_t i = 0; i < SCHEDULE_LIST_MAX_LENGTH; i++) {
		ScheduleList_Insert_First(schedule);
		schedule.time_end.hours++;
	}

	// list is full, it should no longer be possible to add
	if(ScheduleList_Insert_First(schedule) != SCHEDULE_FUNC_ERROR) {
		return 11; // fail on test case 1-1
	}

	if(ScheduleList_Get_Size() != SCHEDULE_LIST_MAX_LENGTH) {
		return 12; // fail on test case 1-2
	}
	return 0;
}

uint8_t Test_Schedules_Insert_After(void) {
	// setup
	ScheduleList_Clear();
	Schedule schedule = Create_Test_Schedule();
	ScheduleList_Insert_First(schedule);
	schedule.time_end.hours++;
	// test
	if (ScheduleList_Insert_After(ScheduleList_Get_First_Node(), schedule) != SCHEDULE_FUNC_OK) {
		return 11; // fail on test case 1-1
	}
	if (ScheduleList_Get_Size() != 2) {
		return 12; // fail on test case 1-2
	}
	return 0;
}

uint8_t Test_Schedules_Remove_From_Empty_List_Error(void) {
	ScheduleList_Clear();
	if (ScheduleList_Remove_First() != SCHEDULE_FUNC_ERROR) {
		return 11; // fail on test case 1-1
	}
	return 0;
}

uint8_t Test_Schedules_Remove_After(void) {
	ScheduleList_Clear();
	Schedule schedule = Create_Test_Schedule();

	ScheduleList_Insert_First(schedule);
	schedule.time_end.hours++;
	ScheduleList_Insert_First(schedule);

	ScheduleNode* first_schedule_node = ScheduleList_Get_First_Node();
	ScheduleList_Remove_After(first_schedule_node);
	if (ScheduleList_Get_Size() != 1) {
		return 11; // fail on test case 1-1
	}
	return 0;
}
