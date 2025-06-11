#include "receive.h"
#include "lamp_state.h"
#include "sys_app.h" // Used for APP_LOG to write output to serial
#include "rtc.h"
#include "stm32wlxx_hal_rtc.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "transmit.h"

extern SemaphoreHandle_t sem_start_battery_read;

/// Brief: Handles incoming LoRaWAN data and calls message interpreter.
void Process_Rx_Data(const LmHandlerAppData_t *const app_data,
                     const LmHandlerRxParams_t *const params) {
  Interpret_Message(app_data->Buffer, app_data->BufferSize);
}

/// Brief: Interprets received message and executes corresponding command.
void Interpret_Message(const uint8_t *const buffer, const uint8_t buffer_size) {
	if (buffer_size < MESSAGE_MIN_BYTES) {
		Tx_Set_Buffer(RESPONSE_OUT, MISSING_DATA, NULL, 0);
		return;
	}

	if (buffer[IDENTIFIER_BYTE] == INSTRUCTION_IN) {
		switch (buffer[SUBTYPE_BYTE]) {
		case LAMP_OFF:
			Handle_Lamp_Off_Instruction(buffer, buffer_size);
			break;
		case LAMP_ON:
			Handle_Lamp_On_Instruction(buffer, buffer_size);
			break;
		case ACTIVATE_MOTION_SENSOR:
			Handle_Activate_Motion_Sensor_Instruction(buffer, buffer_size);
			break;
		case CHANGE_BRIGHTNESS:
			Handle_Change_Brightness_Instruction(buffer, buffer_size);
			break;
		case SEND_BATTERY_STATUS:
			Handle_Send_Battery_Status_Instruction(buffer, buffer_size);
			break;
		case SYNCHRONIZE_TIME_AND_DATE:
			Handle_Synchronize_Time_And_Date_Instruction(buffer, buffer_size);
			break;
		case SET_TIMESCHEDULE:
			Handle_Set_Timeschedule_Instruction(buffer, buffer_size);
			break;
		case SHOW_TIMETABLE:
			Handle_Show_Timetable_Instruction(buffer, buffer_size);
			break;
		case REMOVE_TIMESCHEDULE:
			Handle_Remove_Timeschedule_Instruction(buffer, buffer_size);
			break;
		default:
			Tx_Set_Buffer(RESPONSE_OUT, INVALID_DATA, NULL, 0);
			break;
		}
	} else {
		Tx_Set_Buffer(RESPONSE_OUT, INVALID_DATA, NULL, 0);
	}
}

void Handle_Lamp_Off_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	Send_LampState(OFF);
	Tx_Set_Ack(LAMP_OFF);
}

void Handle_Lamp_On_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	Send_LampState(ON);
	Tx_Set_Ack(LAMP_ON);
}

void Handle_Activate_Motion_Sensor_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	Send_LampState(MOTION_SENSOR);
	Tx_Set_Ack(ACTIVATE_MOTION_SENSOR);
}

void Handle_Change_Brightness_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	if (buffer_size < MESSAGE_MIN_BYTES + BRIGHTNESS_PARAMS_BYTE_COUNT)
	{
		const uint8_t params[] = { CHANGE_BRIGHTNESS };
		Tx_Set_Buffer(RESPONSE_OUT, MISSING_DATA, (const uint8_t* const)&params, sizeof(params));
		return;
	}

	Send_Brightness(buffer[PARAMETERS_START_BYTE]);
	Tx_Set_Ack(CHANGE_BRIGHTNESS);
}

void Handle_Send_Battery_Status_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	// battery task will perform the actual read, to not block the program
	xSemaphoreGive(sem_start_battery_read);
}

void Handle_Synchronize_Time_And_Date_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	if (buffer_size < TIME_DATE_BYTE_COUNT) {
		const uint8_t params[] = { SYNCHRONIZE_TIME_AND_DATE };
		Tx_Set_Buffer(RESPONSE_OUT, MISSING_DATA, (const uint8_t* const)&params, sizeof(params));
	} else if ( !IS_RTC_YEAR(buffer[2]) ||               // Year (0–99)
				!IS_RTC_MONTH(buffer[3]) ||              // Month (1–12)
				!IS_RTC_WEEKDAY(buffer[4]) ||            // Weekday (1–7)
				!IS_RTC_DATE(buffer[5]) ||               // Day (1–31)
				!IS_RTC_HOUR24(buffer[6]) ||             // Hours (0–23)
				!IS_RTC_MINUTES(buffer[7])||             // Minutes (0–59)
				!IS_RTC_SECONDS(buffer[8]))              // Seconds (0–59)
				{
					const uint8_t params[] = { SYNCHRONIZE_TIME_AND_DATE };
					Tx_Set_Buffer(RESPONSE_OUT, INVALID_DATA, (const uint8_t* const)&params, sizeof(params));
				}
	else {
		RTC_TimeTypeDef current_time = {0};
		RTC_DateTypeDef current_date = {0};

		current_date.Year = buffer[2];
		current_date.Month = buffer[3];
		current_date.WeekDay = buffer[4];
		current_date.Date = buffer[5];
		current_time.Hours = buffer[6];
		current_time.Minutes = buffer[7];
		current_time.Seconds = buffer[8];
		current_time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		current_time.StoreOperation = RTC_STOREOPERATION_RESET;


		if (HAL_RTC_SetTime(&hrtc, &current_time, RTC_FORMAT_BIN) != HAL_OK) {
			const uint8_t params[] = { SYNCHRONIZE_TIME_AND_DATE, FAILED_TO_SET_RTC};
			Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION_ERROR, (const uint8_t* const)&params, sizeof(params));
		}

		if (HAL_RTC_SetDate(&hrtc, &current_date, RTC_FORMAT_BIN) != HAL_OK) {
			const uint8_t params[] = { SYNCHRONIZE_TIME_AND_DATE, FAILED_TO_SET_RTC};
			Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION_ERROR, (const uint8_t* const)&params, sizeof(params));
		}
//			An acknowledge is not possible because when it updates the time, it also resets the subseconds, TTN uses this to base their messaging timing on,
//			and then you have to wait however long the microcontroller has been on, which could take a while.
//			And during this time you can't use the LoRa. A reset fixes this, but then you won't get an acknowledge, so for this reason we don't have one.
//			Tx_Set_Ack(SYNCHRONIZE_TIME_AND_DATE);
			NVIC_SystemReset();
	}
}

void Handle_Set_Timeschedule_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	// Below is for testing purposes
	//ScheduleList_Fill_With_Test_Schedules();
	APP_LOG(TS_OFF, VLEVEL_M, "Set new timeslot\r\n");
	// Above is for testing purposes
}

void Handle_Show_Timetable_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	const ScheduleNode* node = ScheduleList_Get_First_Node();
	uint8_t params[IDENTIFIER_BYTE_COUNT + (ScheduleList_Get_Size() * sizeof(Schedule))];
	params[IDENTIFIER_BYTE] = SHOW_TIMETABLE;

	for (uint8_t i = 0; i < ScheduleList_Get_Size() && node; i++)
	{
		const uint8_t index = IDENTIFIER_BYTE_COUNT + (i * sizeof(Schedule));

		params[index] = node->schedule.time_start.year;
		params[index + 1] = node->schedule.time_start.month;
		params[index + 2] = node->schedule.time_start.weekday;
		params[index + 3] = node->schedule.time_start.date;
		params[index + 4] = node->schedule.time_start.hours;
		params[index + 5] = node->schedule.time_start.minutes;
		params[index + 6] = node->schedule.time_start.seconds;

		params[index + 7] = node->schedule.time_end.year;
		params[index + 8] = node->schedule.time_end.month;
		params[index + 9] = node->schedule.time_end.weekday;
		params[index + 10] = node->schedule.time_end.date;
		params[index + 11] = node->schedule.time_end.hours;
		params[index + 12] = node->schedule.time_end.minutes;
		params[index + 13] = node->schedule.time_end.seconds;

		params[index + 14] = node->schedule.lamp_config.state;
		params[index + 15] = node->schedule.lamp_config.brightness;

		node = node->next;
	}

	Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION, (const uint8_t* const)&params, sizeof(params));
}

void Handle_Remove_Timeschedule_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	if (buffer_size < TIME_DATE_BYTE_COUNT) {
		const uint8_t params[] = { REMOVE_TIMESCHEDULE };
		Tx_Set_Buffer(RESPONSE_OUT, MISSING_DATA, (const uint8_t* const)&params, sizeof(params));
		return;
	}

	const ScheduleTimestamp start_time = {
			.year = buffer[2],
			.month = buffer[3],
			.weekday = buffer[4],
			.date = buffer[5],
			.hours = buffer[6],
			.minutes = buffer[7],
			.seconds = buffer[8]
	};

	ScheduleNode* node = ScheduleList_Get_First_Node();

	// Check the first node
	if (ScheduleTimestamp_Equals(&(node->schedule.time_start), &start_time))
	{
		ScheduleList_Remove_First();
		Tx_Set_Ack(REMOVE_TIMESCHEDULE);
        return;
	}
	else // Check all other nodes
	{
		ScheduleNode* previous_node = node;
		node = node->next;

		while (node)
		{
			if (ScheduleTimestamp_Equals(&(node->schedule.time_start), &start_time))
			{
				ScheduleList_Remove_After(previous_node);
				Tx_Set_Ack(REMOVE_TIMESCHEDULE);
				return;
			}
			previous_node = node;
			node = node->next;
		}
	}

	const uint8_t params[] = { REMOVE_TIMESCHEDULE, SCHEDULE_NOT_FOUND };
	Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION_ERROR, (const uint8_t* const)&params, sizeof(params));
}
