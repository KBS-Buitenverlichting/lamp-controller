#include "receive.h"
#include "sys_app.h" // Used for APP_LOG to write output to serial

void Process_Rx_Data(const LmHandlerAppData_t *const app_data,
		const LmHandlerRxParams_t *const params) {
	APP_LOG(TS_OFF, VLEVEL_M, "Received payload (hex): ");
	for (uint8_t i = 0; i < app_data->BufferSize; i++) {
		APP_LOG(TS_OFF, VLEVEL_M, "%02X ", app_data->Buffer[i]);
	}
	APP_LOG(TS_OFF, VLEVEL_M, "\r\n");

	Interpret_Message(app_data->Buffer, app_data->BufferSize);
}

void Interpret_Message(const uint8_t *const buffer, const uint8_t buffer_size) {
	if (buffer_size < MESSAGE_MIN_BYTES) {
		APP_LOG(TS_OFF, VLEVEL_M, "Message too short to interpret\n");
		return;
	}

	if (buffer[IDENTIFIER_BYTE] == INSTRUCTION_IN) {
		switch (buffer[SUBTYPE_BYTE]) {
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
			if (buffer_size < MESSAGE_MIN_BYTES + BRIGHTNESS_PARAMS_BYTE_COUNT) {
				APP_LOG(TS_OFF, VLEVEL_M,
						"Brightness command does not include brightness\n");
			} else {
				APP_LOG(TS_OFF, VLEVEL_M, "Change brightness to %02X!\n",
						buffer[PARAMETERS_START_BYTE]);
			}
			break;
		case SEND_BATTERY_STATUS:
			APP_LOG(TS_OFF, VLEVEL_M, "Report battery state!\r\n");
			{
				const uint8_t params[] = { SEND_BATTERY_STATUS, Get_Battery_Level() };
				Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION, &params, 2);
			}
			break;
		case SET_BATTERY_VREF:
			APP_LOG(TS_OFF, VLEVEL_M, "Setting battery min and max vref\r\n");
			if (buffer_size < MESSAGE_MIN_BYTES + BATTERY_VREF_PARAMS_BYTE_COUNT) {
				const uint8_t params[] = { SET_BATTERY_VREF };
				Tx_Set_Buffer(RESPONSE_OUT, MISSING_DATA, &params, 1);
			}
			else
			{
				uint16_t min_vref = (buffer[PARAMETERS_START_BYTE] << 8) | buffer[PARAMETERS_START_BYTE + 1];
				uint16_t max_vref = (buffer[PARAMETERS_START_BYTE + 2] << 8) | buffer[PARAMETERS_START_BYTE + 3];
				Warning result = Set_Battery_Vref(min_vref, max_vref);
				if (result == NO_WARNING)
				{
					Tx_Set_Ack(SET_BATTERY_VREF);
				}
				else
				{
					const uint8_t params[] = { SET_BATTERY_VREF, result };
					Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION_WARNING, &params, 2);
				}
			}
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
			APP_LOG(TS_OFF, VLEVEL_M, "Unknown AB command: %02X\n", buffer[SUBTYPE_BYTE]);
			break;
		}
	} else {
		APP_LOG(TS_OFF, VLEVEL_M, "Unknown message type: %02X\n", buffer[IDENTIFIER_BYTE]);
	}
}
