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
			APP_LOG(TS_OFF, VLEVEL_M, "Lamp on!\n");
			break;
		case 0x01:
			APP_LOG(TS_OFF, VLEVEL_M, "Lamp off!\n");
			break;
		case 0x02:
			APP_LOG(TS_OFF, VLEVEL_M, "Select motion sensor!\n");
			break;
		case 0x03:
			if (BufferSize < 3) {
				APP_LOG(TS_OFF, VLEVEL_M, "Brightness command does not include brightness\n");
			} else {
				APP_LOG(TS_OFF, VLEVEL_M, "Change brightness to %02X!\n",Buffer[2]);
			}
			break;
		case 0x04:
			APP_LOG(TS_OFF, VLEVEL_M, "Report battery state!\n");
			break;
		default:
			APP_LOG(TS_OFF, VLEVEL_M, "Unknown AB command: %02X\n", Buffer[1]);
		}
	} else {
		APP_LOG(TS_OFF, VLEVEL_M, "Unknown message type: %02X\n", Buffer[0]);
	}
}
