#include "receive.h"
#include "lamp_state.h"
#include "sys_app.h" // Used for APP_LOG to write output to serial
#include "rtc.h"
#include "stm32wlxx_hal_rtc.h"

/// Brief: Handles incoming LoRaWAN data and calls message interpreter.
void Process_Rx_Data(const LmHandlerAppData_t *const app_data,
                     const LmHandlerRxParams_t *const params) {
  APP_LOG(TS_OFF, VLEVEL_M, "Received payload (hex): ");
  for (uint8_t i = 0; i < app_data->BufferSize; i++) {
    APP_LOG(TS_OFF, VLEVEL_M, "%02X ", app_data->Buffer[i]);
  }
  APP_LOG(TS_OFF, VLEVEL_M, "\r\n");

  Interpret_Message(app_data->Buffer, app_data->BufferSize);
}

/// Brief: Interprets received message and executes corresponding command.
void Interpret_Message(const uint8_t *const buffer, const uint8_t buffer_size) {
	if (buffer_size < MESSAGE_MIN_BYTES) {
		APP_LOG(TS_OFF, VLEVEL_M, "Message too short to interpret\r\n");
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
			APP_LOG(TS_OFF, VLEVEL_M, "Unknown AB command: %02X\r\n", buffer[SUBTYPE_BYTE]);
			break;
		}
	} else {
		APP_LOG(TS_OFF, VLEVEL_M, "Unknown message type: %02X\r\n", buffer[IDENTIFIER_BYTE]);
	}
}

void Handle_Lamp_Off_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	APP_LOG(TS_OFF, VLEVEL_M, "Lamp off\r\n");
	Send_LampState(OFF);
	Tx_Set_Ack(LAMP_OFF);
}

void Handle_Lamp_On_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	APP_LOG(TS_OFF, VLEVEL_M, "Lamp on\r\n");
	Send_LampState(ON);
	Tx_Set_Ack(LAMP_ON);
}

void Handle_Activate_Motion_Sensor_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	APP_LOG(TS_OFF, VLEVEL_M, "Select motion sensor\r\n");
	Send_LampState(MOTION_SENSOR);
	Tx_Set_Ack(ACTIVATE_MOTION_SENSOR);
}

void Handle_Change_Brightness_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	APP_LOG(TS_OFF, VLEVEL_M, "Setting brightness\r\n");

	if (buffer_size < MESSAGE_MIN_BYTES + BRIGHTNESS_PARAMS_BYTE_COUNT)
	{
		const uint8_t params[] = { CHANGE_BRIGHTNESS };
		Tx_Set_Buffer(RESPONSE_OUT, MISSING_DATA, (const uint8_t* const)&params, sizeof(params));
		return;
	}

	APP_LOG(TS_OFF, VLEVEL_M, "brightness: %u\r\n", buffer[PARAMETERS_START_BYTE]);

	Send_Brightness(buffer[PARAMETERS_START_BYTE]);
	Tx_Set_Ack(CHANGE_BRIGHTNESS);
}

void Handle_Send_Battery_Status_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	APP_LOG(TS_OFF, VLEVEL_M, "Report battery state\r\n");
	// battery task will perform the actual read, to not block the program
	xSemaphoreGive(sem_start_battery_read);
}

void Handle_Synchronize_Time_And_Date_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	APP_LOG(TS_OFF, VLEVEL_M, "Sync time and date\r\n");

	if (buffer_size < TIME_DATE_BYTE_COUNT) {
		APP_LOG(TS_OFF, VLEVEL_M, "Time/date command input is too short!\r\n");
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
					APP_LOG(TS_OFF, VLEVEL_M, "Invalid date/time value received!\r\n");
					const uint8_t params[] = { SYNCHRONIZE_TIME_AND_DATE };
					Tx_Set_Buffer(RESPONSE_OUT, INVALID_DATA, (const uint8_t* const)&params, sizeof(params));
				}
	else {
		APP_LOG(TS_OFF, VLEVEL_M, "Update time/date!\r\n");

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
			APP_LOG(TS_OFF, VLEVEL_M, "Failed to set RTC time!\r\n");
			const uint8_t params[] = { SYNCHRONIZE_TIME_AND_DATE, FAILED_TO_SET_RTC};
			Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION_ERROR, (const uint8_t* const)&params, sizeof(params));
		}

		if (HAL_RTC_SetDate(&hrtc, &current_date, RTC_FORMAT_BIN) != HAL_OK) {
			APP_LOG(TS_OFF, VLEVEL_M, "Failed to set RTC date!\r\n");
			const uint8_t params[] = { SYNCHRONIZE_TIME_AND_DATE, FAILED_TO_SET_RTC};
			Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION_ERROR, (const uint8_t* const)&params, sizeof(params));
		}
			Print_Current_RTC_Time();
//			An acknowledge is not possible because when it updates the time, it also resets the subseconds, TTN uses this to base their messaging timing on,
//			and then you have to wait however long the microcontroller has been on, which could take a while.
//			And during this time you can't use the LoRa. A reset fixes this, but then you won't get an acknowledge, so for this reason we don't have one.
//			Tx_Set_Ack(SYNCHRONIZE_TIME_AND_DATE);
			NVIC_SystemReset();
	}
}

void Handle_Set_Timeschedule_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	APP_LOG(TS_OFF, VLEVEL_M, "Set new timeslot\r\n");
}

void Handle_Show_Timetable_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	// Below is for testing purposes
	ScheduleList_Clear();

	Schedule test_schedule;
	ScheduleTimestamp timestamp = {
		.year = 25,
		.month = 5,
		.weekday = 3,
		.date = 16,
		.hours = 11,
		.minutes = 30,
		.seconds = 0
	};
	test_schedule.lamp_config = (LampConfig) {
		.state = ON,
		.brightness = 255
	};
	test_schedule.time_start = timestamp;
	test_schedule.time_end = timestamp;
	test_schedule.time_end.hours++;
	(void)ScheduleList_Insert_First(test_schedule);

	ScheduleNode* test_node = ScheduleList_Get_First_Node();


	for (uint8_t i = 0; i < 9; i++)
	{
		(void)ScheduleList_Insert_After(test_node, test_schedule);
		test_node = test_node->next;
	}
	// Above is for testing purposes

	APP_LOG(TS_OFF, VLEVEL_M, "Show timetable\r\n");

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

	// Below is for testing purposes
	ScheduleList_Clear();
	// Above is for testing purposes
}

void Handle_Remove_Timeschedule_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	// Below is for testing purposes
	ScheduleList_Clear();

	Schedule test_schedule;
	ScheduleTimestamp timestamp = {
		.year = 25,
		.month = 5,
		.weekday = 3,
		.date = 16,
		.hours = 11,
		.minutes = 30,
		.seconds = 0
	};
	test_schedule.lamp_config = (LampConfig) {
		.state = ON,
		.brightness = 255
	};
	test_schedule.time_start = timestamp;
	test_schedule.time_end = timestamp;
	test_schedule.time_end.hours++;
	(void)ScheduleList_Insert_First(test_schedule);

	ScheduleNode* test_node = ScheduleList_Get_First_Node();


	for (uint8_t i = 0; i < 9; i++)
	{
		test_schedule.time_start.year++;
		test_schedule.time_end.year++;
		(void)ScheduleList_Insert_After(test_node, test_schedule);
		test_node = test_node->next;
	}

	uint8_t node_counter = 0;
	// Above is for testing purposes

	APP_LOG(TS_OFF, VLEVEL_M, "Remove timeschedule\r\n");

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
		// Below is for testing purposes
        APP_LOG(TS_OFF, VLEVEL_M, "Removed first\r\n");
    	ScheduleList_Clear();
		// Above is for testing purposes
        return;
	}
	else // Check all other nodes
	{
		ScheduleNode* previous_node = node;
		node = node->next;

		while (node)
		{
			// Below is for testing purposes
			node_counter++;
			// Above is for testing purposes
			if (ScheduleTimestamp_Equals(&(node->schedule.time_start), &start_time))
			{
				ScheduleList_Remove_After(previous_node);
				Tx_Set_Ack(REMOVE_TIMESCHEDULE);
				// Below is for testing purposes
		        APP_LOG(TS_OFF, VLEVEL_M, "Removed %u\r\n", node_counter);
		    	ScheduleList_Clear();
				// Above is for testing purposes
				return;
			}
			previous_node = node;
			node = node->next;
		}
	}

	const uint8_t params[] = { REMOVE_TIMESCHEDULE, SCHEDULE_NOT_FOUND };
	Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION_ERROR, (const uint8_t* const)&params, sizeof(params));
}

void Print_Current_RTC_Time(void)
{
	RTC_TimeTypeDef current_time;
    RTC_DateTypeDef current_date;

    HAL_RTC_GetTime(&hrtc, &current_time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &current_date, RTC_FORMAT_BIN);

    const char *weekday_names[] = {"Forbidden", "Monday",   "Tuesday",
    								   "Wednesday", "Thursday", "Friday",
    								   "Saturday",  "Sunday"};
    const char *weekday_str = (current_date.WeekDay <= 7) ? weekday_names[current_date.WeekDay] : "Unknown";

    APP_LOG(TS_OFF, VLEVEL_M, "RTC Now: %02u:%02u:%02u on %02u-%02u-%04u (Weekday %s)\r\n",
            current_time.Hours,
            current_time.Minutes,
            current_time.Seconds,
            current_date.Date,
            current_date.Month,
            2000 + current_date.Year,	// adding 2000 years to get the full year =)
            weekday_str);
}
