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


/// @brief test function, should be removed when inserting
/// 	   schedules over LoRa works
void ScheduleList_Fill_With_Test_Schedules() {
	const uint8_t TEST_SCHEDULE_DURATION = 5;
	LampConfig test_configs[SCHEDULE_LIST_MAX_LENGTH] = {
			{ON, 0xFF},
			{ON, 0x40},
			{MOTION_SENSOR, 0x80},
			{OFF, 0x00},
			{ON, 0x01},
			{MOTION_SENSOR, 0x40},
			{ON, 0xFF},
			{ON, 0x40},
			{MOTION_SENSOR, 0xFF},
			{OFF, 0x00},
	};
	ScheduleList_Clear();

	Schedule test_schedule;
	ScheduleTimestamp timestamp = {
		.year = 25,
		.month = 5,
		.weekday = 2,
		.date = 27,
		.hours = 11,
		.minutes = 0,
		.seconds = 30
	};
	test_schedule.time_start = timestamp;
	test_schedule.time_end = timestamp;
	test_schedule.time_end.seconds += TEST_SCHEDULE_DURATION;
	test_schedule.lamp_config = test_configs[0];
	if (test_schedule.time_end.seconds >= 60) {
		test_schedule.time_end.seconds -= 60;
		test_schedule.time_end.minutes++;
	}


	(void)ScheduleList_Insert_First(test_schedule);
	ScheduleNode* test_node = ScheduleList_Get_First_Node();


	// i = 1 because the first config has already been inserted
	for (uint8_t i = 1; i < SCHEDULE_LIST_MAX_LENGTH; i++)
	{
		test_schedule.time_start.seconds += TEST_SCHEDULE_DURATION*2;
		if (test_schedule.time_start.seconds >= 60) {
			test_schedule.time_start.seconds -= 60;
			test_schedule.time_start.minutes++;
		}
		test_schedule.time_end.minutes = test_schedule.time_start.minutes;
		test_schedule.time_end.seconds = test_schedule.time_start.seconds + TEST_SCHEDULE_DURATION;
		if (test_schedule.time_end.seconds >= 60) {
			test_schedule.time_end.seconds -= 60;
			test_schedule.time_end.minutes++;
		}
		test_schedule.lamp_config = test_configs[i];
		(void)ScheduleList_Insert_After(test_node, test_schedule);
		test_node = test_node->next;
	}

	ScheduleNode* first_node = ScheduleList_Get_First_Node();
	RTC_Set_AlarmB_ScheduleTimestamp(first_node->schedule.time_start);
}

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
	osDelay(200); // this is enough time for LoRa to init the RTC before this task configures Alarm B
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
	Load_ScheduleList_From_Flash();
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
	Save_ScheduleList_To_Flash();
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
	Save_ScheduleList_To_Flash();
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
	Save_ScheduleList_To_Flash();
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
	Save_ScheduleList_To_Flash();
	return SCHEDULE_FUNC_OK;
}

bool Save_ScheduleList_To_Flash(void)
{
    FlashScheduleStorage data_to_save;
    data_to_save.size = schedules.size;

    ScheduleNode *current = schedules.first;
    for (uint8_t i = 0; i < data_to_save.size && i < SCHEDULE_LIST_MAX_LENGTH; i++) {
        data_to_save.schedules[i] = current->schedule;
        current = current->next;
    }

    data_to_save.valid_marker = FLASH_SCHEDULE_VALID_MARKER;

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef erase_init;
    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.Page = (FLASH_SCHEDULE_ADDRESS - FLASH_BASE) / FLASH_PAGE_SIZE;
    erase_init.NbPages = 1;

    uint32_t page_error;
    if (HAL_FLASHEx_Erase(&erase_init, &page_error) != HAL_OK) {
        HAL_FLASH_Lock();
        return false;
    }

    uint32_t double_word_count = (sizeof(FlashScheduleStorage) + 7) / 8; // Rounds up the total size to ensure it's evenly divisible into 8-byte chunks
    uint64_t flash_buffer[double_word_count];
    memcpy(flash_buffer, &data_to_save, sizeof(FlashScheduleStorage));

    for (uint32_t i = 0; i < double_word_count; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
                              FLASH_SCHEDULE_ADDRESS + (i * 8), // Calculates the byte offset for each 8-byte chunk
                              flash_buffer[i]) != HAL_OK) {
            HAL_FLASH_Lock();
            return false;
        }
    }

    HAL_FLASH_Lock();
    return true;
}


bool Load_ScheduleList_From_Flash(void) {
    if (FLASH_SCHEDULE_PTR->valid_marker != FLASH_SCHEDULE_VALID_MARKER)
        return false;

    uint8_t stored_size = FLASH_SCHEDULE_PTR->size;

    if (stored_size == 0 || stored_size > SCHEDULE_LIST_MAX_LENGTH)
        return false;

    Schedule temp_array[SCHEDULE_LIST_MAX_LENGTH];

    for (uint8_t i = 0; i < stored_size; i++) {
        memcpy(&temp_array[i], &FLASH_SCHEDULE_PTR->schedules[i], sizeof(Schedule));
    }

    for (int8_t i = stored_size - 1; i >= 0; i--) {
        ScheduleList_Insert_First(temp_array[i]);
    }

    return true;
}
