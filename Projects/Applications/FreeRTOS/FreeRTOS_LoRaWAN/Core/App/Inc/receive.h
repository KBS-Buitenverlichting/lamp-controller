/*
 * receive.h
 *
 *  Created on: Apr 24, 2025
 *      Author: robdr
 */

#pragma once

#include "stm32wlxx_hal.h"
#include "LmHandler.h"
#include "LmHandlerTypes.h"
#include "message_format.h"
#include "battery.h"

void Process_Rx_Data(const LmHandlerAppData_t *const app_data, const LmHandlerRxParams_t *const params);
void Interpret_Message(const uint8_t *const buffer, const uint8_t buffer_size);
void Handle_Lamp_Off_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);
void Handle_Lamp_On_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);
void Handle_Activate_Motion_Sensor_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);
void Handle_Change_Brightness_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);
void Handle_Send_Battery_Status_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);
void Handle_Set_Battery_Vrefs_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);
void Handle_Synchronize_Time_And_Date_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);
void Handle_Set_Timeslot_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);
void Handle_Show_Timetable_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);
void Handle_Remove_Timeslot_Instruction(const uint8_t *const buffer, const uint8_t buffer_size);
void Print_Current_RTC_Time(void);
