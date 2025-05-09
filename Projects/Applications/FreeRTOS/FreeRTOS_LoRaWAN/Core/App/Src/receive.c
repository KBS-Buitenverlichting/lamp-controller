#include "receive.h"
#include "sys_app.h" // Used for APP_LOG to write output to serial
#include "lamp_state.h"

void Interpret_Message(uint8_t *Buffer, uint8_t BufferSize);

void Process_Rx_Data(const LmHandlerAppData_t *const app_data,
		const LmHandlerRxParams_t *const params) {
	APP_LOG(TS_OFF, VLEVEL_M, "Received payload (hex): ");
	for (uint8_t i = 0; i < app_data->BufferSize; i++) {
		APP_LOG(TS_OFF, VLEVEL_M, "%02X ", app_data->Buffer[i]);
	}
	APP_LOG(TS_OFF, VLEVEL_M, "\r\n");

	Interpret_Message(app_data->Buffer, app_data->BufferSize);
}

void Interpret_Message(uint8_t *Buffer, uint8_t BufferSize) {
	if (BufferSize < 2) {
		APP_LOG(TS_OFF, VLEVEL_M, "Message too short to interpret\n");
		return;
	}

	if (Buffer[0] == 0xAB) {
		switch (Buffer[1]) {
		case 0x00:
			APP_LOG(TS_OFF, VLEVEL_M, "Lamp off\r\n");
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
			Set_State_LampState(Off);
			break;
		case 0x01:
			APP_LOG(TS_OFF, VLEVEL_M, "Lamp on\r\n");
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
			Set_State_LampState(On);
			break;
		case 0x02:
			APP_LOG(TS_OFF, VLEVEL_M, "Select motion sensor!\r\n");
			Set_State_LampState(MotionSensor);
			break;
		case 0x03:
			if (BufferSize < 3) {
				APP_LOG(TS_OFF, VLEVEL_M, "Brightness command does not include brightness\n");
			} else {
				APP_LOG(TS_OFF, VLEVEL_M, "Change brightness to %03u!\r\n", Buffer[2]);
				Set_Brightness(Buffer[2]);
			}
			break;
		case 0x04:
			APP_LOG(TS_OFF, VLEVEL_M, "Report battery state!\n");
			break;
		case 0x05:
			if (BufferSize < 9) {
				APP_LOG(TS_OFF, VLEVEL_M, "Time/date command input is to short!\n");
			} else {
				APP_LOG(TS_OFF, VLEVEL_M, "Update time/ date!\n");

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
		case 0x06:
			APP_LOG(TS_OFF, VLEVEL_M, "Set time schedule!\n");
			break;
		case 0x07:
			APP_LOG(TS_OFF, VLEVEL_M, "Present all time schedules!\n");
			break;
		case 0x08:
			APP_LOG(TS_OFF, VLEVEL_M, "Change time schedule!\n");
			break;
		case 0x09:
			APP_LOG(TS_OFF, VLEVEL_M, "Delete time schedule!\n");
			break;
		default:
			APP_LOG(TS_OFF, VLEVEL_M, "Unknown AB command: %02X\n", Buffer[1]);
		}
	} else {
		APP_LOG(TS_OFF, VLEVEL_M, "Unknown message type: %02X\n", Buffer[0]);
	}

	APP_LOG(TS_OFF, VLEVEL_M, "Current lamp state: %s\r\n", LampState_ToString(Get_State_LampState()));
	APP_LOG(TS_OFF, VLEVEL_M, "Current brightness: %03u\r\n", Get_Brightness());
}

