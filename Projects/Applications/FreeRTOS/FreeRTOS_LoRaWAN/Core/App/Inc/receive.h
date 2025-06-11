/*********************************************************************
 * @file   receive.h
 * @brief  File for handling received LoRa data
 *
 * @author KBS Buitenverlichting
 * @date   24 April 2025
 *********************************************************************/
#pragma once

#include <stdint.h>
#include "LmHandlerTypes.h"
#include "LmHandler.h"
#include "schedules.h"

/**
 * @brief Return codes for schedule insertion check
 */
typedef enum {
    SCHEDULE_VALID_INSERT,
	SCHEDULE_INVALID_DATA,
    SCHEDULE_INVALID_OVERLAP_PREVIOUS,
    SCHEDULE_INVALID_OVERLAP_NEXT,
    SCHEDULE_LIST_FULL,
	SCHEDULE_BEFORE_RTC_TIME
} ScheduleValidationResult;

/**
 * @brief Handles incoming LoRaWAN data and calls message interpreter
 */
void Process_Rx_Data(const LmHandlerAppData_t *const app_data, const LmHandlerRxParams_t *const params);

/**
 * @brief Interprets received message and executes corresponding command
 */
void Interpret_Message(const uint8_t *const buffer, const uint8_t buffer_size);

/**
 * @brief Turns the lamp off
 */
void Handle_Lamp_Off_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);

/**
 * @brief Turns the lamp on
 */
void Handle_Lamp_On_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);

/**
 * @brief Changes the lamp to run on the motion sensor
 */
void Handle_Activate_Motion_Sensor_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);

/**
 * @brief Changes the brightness
 */
void Handle_Change_Brightness_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);

/**
 * @brief Sends the battery percentage
 */
void Handle_Send_Battery_Status_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);

/**
 * @brief Synchronizes the module its time
 */
void Handle_Synchronize_Time_And_Date_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);

/**
 * @brief Sets a time schedule
 */
void Handle_Set_Timeschedule_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);

/**
 * @brief Sends the time table containing all time schedules
 */
void Handle_Show_Timetable_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);

/**
 * @brief Removes a time schedule
 */
void Handle_Remove_Timeschedule_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);

/**
 * @brief Checks if a time schedule can be inserted
 */
ScheduleValidationResult ScheduleList_Can_Insert(const Schedule* new_schedule, ScheduleNode** out_insert_after);
