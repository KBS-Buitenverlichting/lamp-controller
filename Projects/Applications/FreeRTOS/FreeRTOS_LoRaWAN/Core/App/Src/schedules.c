/*
 * schedules.c
 *
 *  Created on: May 15, 2025
 *      Author: jacco
 */

#include <stdlib.h>
#include <stdbool.h>
#include "schedules.h"

SemaphoreHandle_t sem_process_alarm;
static ScheduleList schedules = { 0 };
static bool schedule_active = false;

void ScheduleList_Init();

void RTC_Set_AlarmB_ScheduleTimestamp(ScheduleTimestamp ts) {
	RTC_AlarmTypeDef alarm_b = {0};

	HAL_RTC_GetAlarm(&hrtc, &alarm_b, RTC_ALARM_B, FORMAT_BIN);

	alarm_b.AlarmMask = RTC_ALARMMASK_NONE;
	alarm_b.AlarmTime.Hours = ts.hours;
	alarm_b.AlarmTime.Minutes = ts.minutes;
	alarm_b.AlarmTime.Seconds = ts.seconds;
	alarm_b.AlarmDateWeekDay = ts.date;
	alarm_b.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;

	if (HAL_RTC_SetAlarm_IT(&hrtc, &alarm_b, FORMAT_BIN) != HAL_OK) {
		Error_Handler();
	}
}

void Start_Process_Schedules_Task(void const *argument) {
	ScheduleList_Init();
	for(;;) {
		if(xSemaphoreTake(sem_process_alarm, portMAX_DELAY) != pdPASS) {
			Error_Handler();
		}
		if (ScheduleList_Get_Size() <= 0) {
			continue; // false alarm, there is no schedule to process
		}
		if (schedule_active) {
			vcom_Trace((uint8_t *)"SCHEDULE END\r\n", 14);
			// schedule has reached end time, remove it
			ScheduleList_Remove_First();

			
			// restore saved lamp config
			Send_LampState(previous_lamp_config.state);
			Send_Brightness(previous_lamp_config.brightness);
			
			ScheduleNode* next_alarm_schedule_node = ScheduleList_Get_First_Node();
			// queue up the next schedule alarm
			if (next_alarm_schedule_node != NULL) {
				Schedule next_alarm_schedule = next_alarm_schedule_node->schedule;
				RTC_Set_AlarmB_ScheduleTimestamp(next_alarm_schedule.time_start);
			}
			schedule_active = false;
		} else {
			vcom_Trace((uint8_t *)"SCHEDULE START\r\n", 16);
			Schedule current_schedule = ScheduleList_Get_First_Node()->schedule;

			// save current lamp config
			previous_lamp_config.state = Get_State_LampState();
			previous_lamp_config.brightness = Get_Brightness();

			// configure lamp based on the new schedule
			Send_LampState(current_schedule.lamp_config.state);
			Send_Brightness(current_schedule.lamp_config.brightness);

			// set next alarm to end of current schedule
			RTC_Set_AlarmB_ScheduleTimestamp(current_schedule.time_end);
			schedule_active = true;
		}

	}
}

void ScheduleList_Init() {
	sem_process_alarm = xSemaphoreCreateBinary();
}

uint8_t ScheduleList_Get_Size(void) {
	return schedules.size;
}

void ScheduleList_Clear(void) {
	while (schedules.first != NULL) {
		ScheduleList_Remove_First();
	}
}

void ScheduleTimestamp_To_RTC_DateTime(const ScheduleTimestamp *const timestamp,
		RTC_DateTypeDef *const out_date, RTC_TimeTypeDef *const out_time) {
	out_date->Year = timestamp->year;
	out_date->Month = timestamp->month;
	out_date->WeekDay = timestamp->weekday;
	out_date->Date = timestamp->date;

	out_time->Hours = timestamp->hours;
	out_time->Minutes = timestamp->minutes;
	out_time->Seconds = timestamp->seconds;
}

ScheduleTimestamp RTC_DateTime_To_ScheduleTimestamp(
		const RTC_DateTypeDef *const date, const RTC_TimeTypeDef *const time) {
	ScheduleTimestamp timestamp = { .year = date->Year, .month = date->Month,
			.weekday = date->WeekDay, .date = date->Date, .hours = time->Hours,
			.minutes = time->Minutes, .seconds = time->Seconds };
	return timestamp;
}

ScheduleNode* ScheduleList_Get_First_Node(void) {
	return schedules.first;
}

ScheduleFuncStatus ScheduleList_Insert_First(Schedule new_schedule) {
	if (schedules.size == SCHEDULE_LIST_MAX_LENGTH) {
		return SCHEDULE_FUNC_ERROR;
	}
	ScheduleNode *new_schedule_node = (ScheduleNode*) malloc(
			sizeof(ScheduleNode));
	new_schedule_node->schedule = new_schedule;
	new_schedule_node->next = schedules.first;
	schedules.first = new_schedule_node;
	schedules.size++;
	return SCHEDULE_FUNC_OK;
}

ScheduleFuncStatus ScheduleList_Insert_After(ScheduleNode * const schedule_node,
		Schedule new_schedule) {
	if (schedules.size == SCHEDULE_LIST_MAX_LENGTH) {
		return SCHEDULE_FUNC_ERROR;
	}
	ScheduleNode *new_schedule_node = (ScheduleNode*) malloc(
			sizeof(ScheduleNode));
	new_schedule_node->schedule = new_schedule;
	new_schedule_node->next = schedule_node->next;
	schedule_node->next = new_schedule_node;
	schedules.size++;
	return SCHEDULE_FUNC_OK;
}

ScheduleFuncStatus ScheduleList_Remove_First(void) {
	if (schedules.first == NULL) {
		return SCHEDULE_FUNC_ERROR;
	}
	ScheduleNode *node_to_remove = schedules.first;
	schedules.first = schedules.first->next;
	free(node_to_remove);
	schedules.size--;
	return SCHEDULE_FUNC_OK;
}

ScheduleFuncStatus ScheduleList_Remove_After(ScheduleNode * const schedule_node) {
	ScheduleNode *node_to_remove = schedule_node->next;
	if (node_to_remove == NULL) {
		return SCHEDULE_FUNC_ERROR;
	}
	schedule_node->next = node_to_remove->next;
	free(node_to_remove);
	schedules.size--;
	return SCHEDULE_FUNC_OK;
}

uint8_t ScheduleTimestamp_Compare(const ScheduleTimestamp* a, const ScheduleTimestamp* b)
{
    if (a->year != b->year)       return a->year - b->year;
    if (a->month != b->month)     return a->month - b->month;
    if (a->date != b->date)       return a->date - b->date;
    if (a->hours != b->hours)     return a->hours - b->hours;
    if (a->minutes != b->minutes) return a->minutes - b->minutes;
    if (a->seconds != b->seconds) return a->seconds - b->seconds;

    return 0; // Equal
}

