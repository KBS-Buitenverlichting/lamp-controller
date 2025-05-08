#include "receive.h"
#include "sys_app.h" // Used for APP_LOG to write output to serial

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
			APP_LOG(TS_OFF, VLEVEL_M, "Lamp off\n");
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
			break;
		case 0x01:
			APP_LOG(TS_OFF, VLEVEL_M, "Lamp on\n");
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
			break;
		case 0x02:
			APP_LOG(TS_OFF, VLEVEL_M, "Select motion sensor!\n");
			break;
		case 0x03:
			if (BufferSize < 3) {
				APP_LOG(TS_OFF, VLEVEL_M,
						"Brightness command does not include brightness\n");
			} else {
				APP_LOG(TS_OFF, VLEVEL_M, "Change brightness to %02X!\n",
						Buffer[2]);
			}
			break;
		case 0x04:
			APP_LOG(TS_OFF, VLEVEL_M, "Report battery state!\n");
			break;
		case 0x05:
			if (BufferSize < 9){
				APP_LOG(TS_OFF, VLEVEL_M, "Time/date command input is to short!\n");
			} else {
				APP_LOG(TS_OFF, VLEVEL_M, "Update time/ date!\n");

				uint8_t hour = Buffer[2];
				uint8_t minute = Buffer[3];
				uint8_t second = Buffer[4];
				uint8_t year = Buffer[5];      // Assume offset from 2000
				uint8_t weekday = Buffer[6];   // 1 = Monday, 7 = Sunday
				uint8_t month = Buffer[7];     // 1–12
				uint8_t day = Buffer[8];       // 1–31

				const char* weekday_names[] = {
				    "Forbidden", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"
				};

				const char* weekday_str = "Unknown";
				if (weekday <= 7) {
				    weekday_str = weekday_names[weekday];
				}

				APP_LOG(TS_OFF, VLEVEL_M, "Time: %02u:%02u:%02u\r\n", hour, minute, second);
				APP_LOG(TS_OFF, VLEVEL_M, "Date: %s %02u-%02u-%04u\r\n", weekday_str, day, month, 2000 + year);
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
}

