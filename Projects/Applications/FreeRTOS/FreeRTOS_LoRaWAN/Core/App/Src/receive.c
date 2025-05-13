#include "receive.h"
#include "message_format.h"
#include "sys_app.h" // Used for APP_LOG to write output to serial
#include "lamp_state.h"

#define TIME_DATE_BUFFERSIZE 9

void Interpret_Message(uint8_t *Buffer, uint8_t BufferSize);

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
void Interpret_Message(uint8_t *Buffer, uint8_t BufferSize) {
	if (BufferSize < MESSAGE_MIN_BYTES) {
		APP_LOG(TS_OFF, VLEVEL_M, "Message too short to interpret\n");
		return;
	}

	if (Buffer[IDENTIFIER_BYTE] == INSTRUCTION_IN) {
		switch (Buffer[SUBTYPE_BYTE]) {
		case LAMP_OFF:
			Send_LampState(Off);
			break;
		case LAMP_ON:
			Send_LampState(On);
			break;
		case ACTIVATE_MOTION_SENSOR:
			Send_LampState(MotionSensor);
			break;
		case CHANGE_BRIGHTNESS:
		    Send_Brightness(Buffer[PARAMETERS_START_BYTE]);
		    break;
		case SEND_BATTERY_STATUS:
			APP_LOG(TS_OFF, VLEVEL_M, "Report battery state!\r\n");
			break;
		case SYNCHRONIZE_TIME_AND_DATE:
			if (BufferSize < TIME_DATE_BUFFERSIZE) {
				APP_LOG(TS_OFF, VLEVEL_M, "Time/date command input is to short!\r\n");
			} else {
				APP_LOG(TS_OFF, VLEVEL_M, "Update time/ date!\r\n");

				uint8_t Hour = Buffer[2];
				uint8_t Minute = Buffer[3];
				uint8_t Second = Buffer[4];
				uint8_t Year = Buffer[5];      // Assume offset from 2000
				uint8_t Weekday = Buffer[6];   // 1 = Monday, 7 = Sunday
				uint8_t Month = Buffer[7];     // 1–12
				uint8_t Day = Buffer[8];       // 1–31

				const char *Weekday_Names[] = { "Forbidden", "Monday",
						"Tuesday", "Wednesday", "Thursday", "Friday",
						"Saturday", "Sunday" };

				const char *Weekday_Str = "Unknown";

				if (Weekday <= 7) {
					Weekday_Str = Weekday_Names[Weekday];
				}

				APP_LOG(TS_OFF, VLEVEL_M, "Time: %02u:%02u:%02u\r\n", Hour, Minute, Second);
				APP_LOG(TS_OFF, VLEVEL_M, "Date: %s %02u-%02u-%04u\r\n", Weekday_Str, Day, Month, 2000 + Year);
			}
			break;
		case SET_TIMESLOT:
			APP_LOG(TS_OFF, VLEVEL_M, "Set time schedule!\r\n");
			break;
		case SHOW_TIMETABLE:
			APP_LOG(TS_OFF, VLEVEL_M, "Show time table!\r\n");
			break;
		case CHANGE_TIMESLOT:
			APP_LOG(TS_OFF, VLEVEL_M, "Change timeslot!\r\n");
			break;
		case REMOVE_TIMESLOT:
			APP_LOG(TS_OFF, VLEVEL_M, "Remove timeslot!\r\n");
			break;
		default:
			APP_LOG(TS_OFF, VLEVEL_M, "Unknown AB command: %02X\r\n", Buffer[SUBTYPE_BYTE]);
			break;
		}
	} else {
		APP_LOG(TS_OFF, VLEVEL_M, "Unknown message type: %02X\r\n", Buffer[IDENTIFIER_BYTE]);
	}
}

