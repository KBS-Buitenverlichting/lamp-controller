/*
 * schedules.h
 *
 *  Created on: May 15, 2025
 *      Author: jacco
 */

#pragma once

#include "rtc.h"
#include "lamp_state.h"
#include "stdbool.h"

#define SCHEDULE_LIST_MAX_LENGTH 10
#define FLASH_SCHEDULE_VALID_MARKER 0xBEEFBEEF
#define FLASH_SCHEDULE_ADDRESS 0x0803F000
#define FLASH_SCHEDULE_PTR ((const FlashScheduleStorage*) FLASH_SCHEDULE_ADDRESS)

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

typedef struct {
	uint32_t valid_marker;
	uint8_t size;
	uint8_t padding[3]; // Padding to ensure the total struct size is a multiple of 8
	Schedule schedules[SCHEDULE_LIST_MAX_LENGTH];
} FlashScheduleStorage;

bool Save_ScheduleList_To_Flash(void);
bool Load_ScheduleList_From_Flash(void);

extern SemaphoreHandle_t sem_process_alarm;

void RTC_Set_AlarmB_ScheduleTimestamp(ScheduleTimestamp ts);

void Start_Process_Schedules_Task(void const *argument);

uint8_t ScheduleList_Get_Size(void);
void ScheduleList_Clear(void);

void ScheduleTimestamp_To_RTC_DateTime(const ScheduleTimestamp* const timestamp, RTC_DateTypeDef* const out_date, RTC_TimeTypeDef* const out_time);
ScheduleTimestamp RTC_DateTime_To_ScheduleTimestamp(const RTC_DateTypeDef* const date, const RTC_TimeTypeDef* const time);
bool ScheduleTimestamp_Equals(const ScheduleTimestamp* const ts1, const ScheduleTimestamp* const ts2);

ScheduleNode* ScheduleList_Get_First_Node(void);
ScheduleFuncStatus ScheduleList_Insert_First(Schedule new_schedule);
ScheduleFuncStatus ScheduleList_Insert_After(ScheduleNode* const schedule_node, Schedule new_schedule);
ScheduleFuncStatus ScheduleList_Remove_First(void);
ScheduleFuncStatus ScheduleList_Remove_After(ScheduleNode* const schedule_node);

void ScheduleList_Fill_With_Test_Schedules(void);

