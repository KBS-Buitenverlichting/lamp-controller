#include "receive.h"
#include "message_format.h"
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
	if (BufferSize < MESSAGE_MIN_BYTES) {
		APP_LOG(TS_OFF, VLEVEL_M, "Message too short to interpret\n");
		return;
	}

	if (Buffer[IDENTIFIER_BYTE] == INSTRUCTION_IN) {
		switch (Buffer[SUBTYPE_BYTE]) {
		case LAMP_OFF:
			APP_LOG(TS_OFF, VLEVEL_M, "Lamp off\n");
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
			break;
		case LAMP_ON:
			APP_LOG(TS_OFF, VLEVEL_M, "Lamp on\n");
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
			break;
		case ACTIVATE_MOTION_SENSOR:
			APP_LOG(TS_OFF, VLEVEL_M, "Select motion sensor!\n");
			break;
		case CHANGE_BRIGHTNESS:
			if (BufferSize <= MESSAGE_MIN_BYTES) {
				APP_LOG(TS_OFF, VLEVEL_M,
						"Brightness command does not include brightness\n");
			} else {
				APP_LOG(TS_OFF, VLEVEL_M, "Change brightness to %02X!\n",
						Buffer[PARAMETERS_START_BYTE]);
			}
			break;
		case SEND_BATTERY_STATUS:
			APP_LOG(TS_OFF, VLEVEL_M, "Report battery state!\n");
			break;
		case SYNCHRONIZE_TIME_AND_DATE:
			break;
		case SET_TIMESLOT:
			break;
		case SHOW_TIMETABLE:
			break;
		case CHANGE_TIMESLOT:
			break;
		case REMOVE_TIMESLOT:
			break;
		default:
			APP_LOG(TS_OFF, VLEVEL_M, "Unknown AB command: %02X\n", Buffer[SUBTYPE_BYTE]);
			break;
		}
	} else {
		APP_LOG(TS_OFF, VLEVEL_M, "Unknown message type: %02X\n", Buffer[IDENTIFIER_BYTE]);
	}
}
