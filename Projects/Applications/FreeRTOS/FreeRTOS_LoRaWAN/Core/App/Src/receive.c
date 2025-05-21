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
		case SET_BATTERY_VREFS:
			Handle_Set_Battery_Vrefs_Instruction(buffer, buffer_size);
			break;
		case SYNCHRONIZE_TIME_AND_DATE:
			Handle_Synchronize_Time_And_Date_Instruction(buffer, buffer_size);
			break;
		case SET_TIMESLOT:
			Handle_Set_Timeslot_Instruction(buffer, buffer_size);
			break;
		case SHOW_TIMETABLE:
			Handle_Show_Timetable_Instruction(buffer, buffer_size);
			break;
		case REMOVE_TIMESLOT:
			Handle_Remove_Timeslot_Instruction(buffer, buffer_size);
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
	if (buffer_size < MESSAGE_MIN_BYTES + BRIGHTNESS_PARAMS_BYTE_COUNT) {
		APP_LOG(TS_OFF, VLEVEL_M, "Brightness command does not include brightness\n");
	} else {
		APP_LOG(TS_OFF, VLEVEL_M, "Change brightness to %02X!\n", buffer[PARAMETERS_START_BYTE]);
		Send_Brightness(buffer[PARAMETERS_START_BYTE]);
	}
}

void Handle_Send_Battery_Status_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	APP_LOG(TS_OFF, VLEVEL_M, "Report battery state\r\n");

	if (Vrefs_Initialized())
	{
		const uint8_t params[] = { SEND_BATTERY_STATUS, Get_Battery_Level() };
		Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION, (const uint8_t* const)&params, sizeof(params));
	}
	else
	{
		const uint8_t params[] = { SEND_BATTERY_STATUS, VREFS_NOT_INITIALIZED };
		Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION_ERROR, (const uint8_t* const)&params, sizeof(params));
	}
}

void Handle_Set_Battery_Vrefs_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	APP_LOG(TS_OFF, VLEVEL_M, "Setting battery min and max vref\r\n");

	if (buffer_size < MESSAGE_MIN_BYTES + BATTERY_VREF_PARAMS_BYTE_COUNT)
	{
		const uint8_t params[] = { SET_BATTERY_VREFS };
		Tx_Set_Buffer(RESPONSE_OUT, MISSING_DATA, (const uint8_t* const)&params, sizeof(params));
		return;
	}

	uint16_t min_vref = (buffer[PARAMETERS_START_BYTE] << 8) | buffer[PARAMETERS_START_BYTE + 1];
	uint16_t max_vref = (buffer[PARAMETERS_START_BYTE + 2] << 8) | buffer[PARAMETERS_START_BYTE + 3];

	APP_LOG(TS_OFF, VLEVEL_M, "min: %u    max: %u\r\n", min_vref, max_vref);

	Warning result = Set_Battery_Vref(min_vref, max_vref);

	if (result != NO_WARNING)
	{
		const uint8_t params[] = { SET_BATTERY_VREFS, result };
		Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION_WARNING, (const uint8_t* const)&params, sizeof(params));
		return;
	}

	Tx_Set_Ack(SET_BATTERY_VREFS);
}

void Handle_Synchronize_Time_And_Date_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	APP_LOG(TS_OFF, VLEVEL_M, "Sync time and date\r\n");

	if (buffer_size < TIME_DATE_BYTE_COUNT) {
		APP_LOG(TS_OFF, VLEVEL_M, "Time/date command input is too short!\r\n");
		Tx_Set_Ack(MISSING_DATA);
	} else if ( !IS_RTC_YEAR(buffer[2]) ||               // Year (0–99)
				!IS_RTC_MONTH(buffer[3]) ||              // Month (1–12)
				!IS_RTC_WEEKDAY(buffer[4]) ||            // Weekday (1–7)
				!IS_RTC_DATE(buffer[5]) ||               // Day (1–31)
				!IS_RTC_HOUR24(buffer[6]) ||             // Hours (0–23)
				!IS_RTC_MINUTES(buffer[7])||             // Minutes (0–59)
				!IS_RTC_SECONDS(buffer[8]))              // Seconds (0–59)
				{
					APP_LOG(TS_OFF, VLEVEL_M, "Invalid date/time value received!\r\n");
					Tx_Set_Ack(INVALID_DATA);
				}
	else {
		APP_LOG(TS_OFF, VLEVEL_M, "Update time/date!\r\n");

		RTC_TimeTypeDef sTime = {0};
		RTC_DateTypeDef sDate = {0};

		sDate.Year = buffer[2];
		sDate.Month = buffer[3];
		sDate.WeekDay = buffer[4];
		sDate.Date = buffer[5];
		sTime.Hours = buffer[6];
		sTime.Minutes = buffer[7];
		sTime.Seconds = buffer[8];
		sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		sTime.StoreOperation = RTC_STOREOPERATION_RESET;


		if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
			APP_LOG(TS_OFF, VLEVEL_M, "Failed to set RTC time!\r\n");
			const uint8_t params[] = { SYNCHRONIZE_TIME_AND_DATE, FAILED_TO_SET_RTC};
			Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION_ERROR, (const uint8_t* const)&params, sizeof(params));
		}

		if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
			APP_LOG(TS_OFF, VLEVEL_M, "Failed to set RTC date!\r\n");
			const uint8_t params[] = { SYNCHRONIZE_TIME_AND_DATE, FAILED_TO_SET_RTC};
			Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION_ERROR, (const uint8_t* const)&params, sizeof(params));
		}

			Print_Current_RTC_Time();
			Tx_Set_Ack(SYNCHRONIZE_TIME_AND_DATE);
	}
}

void Handle_Set_Timeslot_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	APP_LOG(TS_OFF, VLEVEL_M, "Set new timeslot\r\n");
}

void Handle_Show_Timetable_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	APP_LOG(TS_OFF, VLEVEL_M, "Show timetable\r\n");
}

void Handle_Remove_Timeslot_Instruction(const uint8_t *const buffer, const uint8_t buffer_size)
{
	APP_LOG(TS_OFF, VLEVEL_M, "Remove timeslot\r\n");
}

void Print_Current_RTC_Time(void)
{
	RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    const char *weekday_names[] = {"Forbidden", "Monday",   "Tuesday",
    								   "Wednesday", "Thursday", "Friday",
    								   "Saturday",  "Sunday"};
    const char *weekday_str = (sDate.WeekDay <= 7) ? weekday_names[sDate.WeekDay] : "Unknown";

    APP_LOG(TS_OFF, VLEVEL_M, "RTC Now: %02u:%02u:%02u on %02u-%02u-%04u (Weekday %s)\r\n",
            sTime.Hours,
            sTime.Minutes,
            sTime.Seconds,
            sDate.Date,
            sDate.Month,
            2000 + sDate.Year,
            weekday_str);
}
