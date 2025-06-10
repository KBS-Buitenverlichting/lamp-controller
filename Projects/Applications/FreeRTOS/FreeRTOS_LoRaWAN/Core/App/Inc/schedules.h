/*********************************************************************
 * @file   schedules.h
 * @brief  File for handling time/ task schedules
 *
 * @author KBS Buitenverlichting
 * @date   15 May 2025
 *********************************************************************/
#pragma once

#include "rtc.h"
#include "lamp_state.h"
#include "stdbool.h"

#define SCHEDULE_LIST_MAX_LENGTH 10
#define FLASH_SCHEDULE_VALID_MARKER 0xBEEFBEEF
#define FLASH_SCHEDULE_ADDRESS 0x0803F000
#define FLASH_SCHEDULE_PTR ((const FlashScheduleStorage*) FLASH_SCHEDULE_ADDRESS)

/**
 * @brief Function return values
 */
typedef enum ScheduleFuncStatus {
	SCHEDULE_FUNC_OK,
	SCHEDULE_FUNC_ERROR
} ScheduleFuncStatus;

/**
 * @brief Timestamp struct, size-efficient combination of RTC TimeTypeDef and DateTypeDef types
 */
typedef struct ScheduleTimestamp {
	uint8_t year;
	uint8_t month;
	uint8_t weekday;
	uint8_t date;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
} ScheduleTimestamp;

/**
 * @brief Schedule struct containing its period and action
 */
typedef struct Schedule {
	ScheduleTimestamp time_start;
	ScheduleTimestamp time_end;
	LampConfig lamp_config;
} Schedule;

/**
 * @brief Node to contain a schedule for linked-list
 */
typedef struct ScheduleNode {
	Schedule schedule;
	struct ScheduleNode* next;
} ScheduleNode;


/**
 * @brief Linked-list of schedules
 */
typedef struct ScheduleList {
	uint8_t size;
	ScheduleNode* first;
} ScheduleList;

/**
 * @brief Struct to manage flash memory usage for saving time schedules
 */
typedef struct {
	uint32_t valid_marker;
	uint8_t size;
	uint8_t padding[3]; // Padding to ensure the total struct size is a multiple of 8
	Schedule schedules[SCHEDULE_LIST_MAX_LENGTH];
} FlashScheduleStorage;

/**
 * @brief Sets alarm B to trigger at a certain time
 *
 * @param[in] ts The timestamp at which to trigger
 */
void RTC_Set_AlarmB_ScheduleTimestamp(ScheduleTimestamp ts);

/**
 * @brief Manage schedules
 *
 * @param[in] argument Any arguments the function might receive
 */
void Start_Process_Schedules_Task(void const *argument);

/**
 * @brief Converts a timestamp to rtc format
 *
 * @param[in]  timestamp	The timestamp to convert
 * @param[out] out_date		The date part of the timestamp
 * @param[out] out_time		The time part of the timestamp
 */
void ScheduleTimestamp_To_RTC_DateTime(const ScheduleTimestamp* const timestamp, RTC_DateTypeDef* const out_date, RTC_TimeTypeDef* const out_time);

/**
 * @brief Converts a rtc formatted time to a timestamp
 *
 * @param[in] data	The rtc date
 * @param[in] time	The rtc time
 *
 * @return The converted timestamp
 */
ScheduleTimestamp RTC_DateTime_To_ScheduleTimestamp(const RTC_DateTypeDef* const date, const RTC_TimeTypeDef* const time);

/**
 * @brief Checks if two timestamps are the same
 *
 * @param[in] ts1	Timestamp 1
 * @param[in] ts2	Timestamp 2
 *
 * @return True if they are the same, else False
 */
bool ScheduleTimestamp_Equals(const ScheduleTimestamp* const ts1, const ScheduleTimestamp* const ts2);

/**
 * @brief Initializes the list
 */
void ScheduleList_Init();

/**
 * @brief Gets the number of schedules in the list
 *
 * @return Number of schedules
 */
uint8_t ScheduleList_Get_Size(void);

/**
 * @brief Clears the list
 */
void ScheduleList_Clear(void);

/**
 * @brief Gets the first node in the list
 *
 * @return First node
 */
ScheduleNode* ScheduleList_Get_First_Node(void);

/**
 * @brief Inserts a schedule at the front
 *
 * @param[in] new_schedule The schedule to insert
 *
 * @return Execution status
 */
ScheduleFuncStatus ScheduleList_Insert_First(Schedule new_schedule);

/**
 * @brief Inserts a schedule after a certain node
 *
 * @param[in] schedule_node	The node after which to insert
 * @param[in] new_schedule	The schedule to insert
 *
 * @return Execution status
 */
ScheduleFuncStatus ScheduleList_Insert_After(ScheduleNode* const schedule_node, Schedule new_schedule);

/**
 * @brief Removes the schedule at the front
 *
 * @return Execution status
 */
ScheduleFuncStatus ScheduleList_Remove_First(void);

/**
 * @brief Removes the schedule after a certain node
 *
 * @param[in] schedule_node The node after which to remove
 *
 * @return Execution status
 */
ScheduleFuncStatus ScheduleList_Remove_After(ScheduleNode* const schedule_node);

/**
 * @brief Saves the schedule list in flash memory
 *
 * @return Execution status
 */
bool Save_ScheduleList_To_Flash(void);

/**
 * @brief Loads the schedule list from flash memory
 *
 * @return Execution status
 */
bool Load_ScheduleList_From_Flash(void);
