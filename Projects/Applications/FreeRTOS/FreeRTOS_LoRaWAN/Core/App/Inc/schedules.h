/*
 * schedules.h
 *
 *  Created on: May 15, 2025
 *      Author: jacco
 */

#pragma once

// #include "lamp_state.h"
#include "rtc.h"

#define MAX_SCHEDULE_LIST_LENGTH 10

typedef struct LampConfig {
	// LampState lamp_state;
	uint8_t brightness;
} LampConfig;

typedef struct Schedule {
	RTC_DateTypeDef start_date;
	RTC_TimeTypeDef start_time;
	RTC_DateTypeDef end_date;
	RTC_TimeTypeDef end_time;
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

void Insert_Schedule_After(ScheduleNode* schedule, ScheduleNode* newSchedule);
void Remove_Schedule(ScheduleNode* schedule);
