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

void Start_Process_Schedules_Task(void const *argument) {
	ScheduleList_Init();
	for(;;) {
		if(xSemaphoreTake(sem_process_alarm, portMAX_DELAY) != pdPASS) {
			Error_Handler();
		}
		// check if a schedule is active
		// 	if yes: deactivate and delete it, restore the previous lamp config, set alarm to next schedule in the list
		//  if no: save current lamp config to previous, set the first schedule in the list as active, set alarm to end time of current schedule
		if (schedule_active) {

		} else {

		}

	}
}

void ScheduleList_Init() {
	ScheduleList_Clear();
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

bool ScheduleTimestamp_Equals(const ScheduleTimestamp* const ts1, const ScheduleTimestamp* const ts2)
{
	return (ts1->year == ts2->year &&
			ts1->month == ts2->month &&
			ts1->weekday == ts2->weekday &&
			ts1->date == ts2->date &&
			ts1->hours == ts2->hours &&
			ts1->minutes == ts2->minutes &&
			ts1->seconds == ts2->seconds);
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
