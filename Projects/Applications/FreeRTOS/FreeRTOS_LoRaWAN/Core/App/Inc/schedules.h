/*
 * schedules.h
 *
 *  Created on: May 15, 2025
 *      Author: jacco
 */

#pragma once

#include "lamp_state.h"
#include "rtc.h"

#define SCHEDULE_LIST_MAX_LENGTH 10

typedef enum ScheduleFuncStatus {
	SCHEDULE_FUNC_OK,
	SCHEDULE_FUNC_ERROR
} ScheduleFuncStatus;

// more size-efficient than the RTC TimeTypeDef and DateTypeDef types
typedef struct ScheduleTimestamp {
	uint8_t year;
	uint8_t month;
	uint8_t weekday;
	uint8_t date;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
} ScheduleTimestamp;

typedef struct LampConfig {
	LampState lamp_state;
	uint8_t brightness;
} LampConfig;

typedef struct Schedule {
	ScheduleTimestamp time_start;
	ScheduleTimestamp time_end;
	LampConfig lamp_config;
} Schedule;

typedef struct ScheduleNode {
	Schedule schedule;
	struct ScheduleNode* next;
} ScheduleNode;


// linked list of schedules
typedef struct ScheduleList {
	uint8_t size;
	ScheduleNode* first;
} ScheduleList;


void ScheduleTimestamp_To_RTC_DateTime(const ScheduleTimestamp* const timestamp, RTC_DateTypeDef* const out_date, RTC_TimeTypeDef* const out_time);
ScheduleTimestamp RTC_DateTime_To_ScheduleTimestamp(const RTC_DateTypeDef* const date, const RTC_TimeTypeDef* const time);


Schedule Schedule_Get_First(void);
ScheduleFuncStatus Schedule_Insert_First(Schedule new_schedule);
ScheduleFuncStatus Schedule_Insert_After(ScheduleNode* schedule, ScheduleNode* newSchedule);
ScheduleFuncStatus Schedule_Remove_First(void);
ScheduleFuncStatus Schedule_Remove_After(ScheduleNode* schedule_node);
