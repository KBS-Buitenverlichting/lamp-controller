#include "receive.h"
#include "lamp_state.h"
#include "sys_app.h" // Used for APP_LOG to write output to serial

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
    APP_LOG(TS_OFF, VLEVEL_M, "Message too short to interpret\n");
    return;
  }

  if (buffer[IDENTIFIER_BYTE] == INSTRUCTION_IN) {
    switch (buffer[SUBTYPE_BYTE]) {
    case LAMP_OFF:
      Send_LampState(OFF);
      break;
    case LAMP_ON:
      Send_LampState(ON);
      break;
    case ACTIVATE_MOTION_SENSOR:
      Send_LampState(MOTION_SENSOR);
      break;
    case CHANGE_BRIGHTNESS:
      Send_Brightness(buffer[PARAMETERS_START_BYTE]);
      break;
      case SEND_BATTERY_STATUS:
			APP_LOG(TS_OFF, VLEVEL_M, "Report battery state!\r\n");
			{
				if (Vrefs_Initialized())
				{
					const uint8_t params[] = { SEND_BATTERY_STATUS, Get_Battery_Level() };
					Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION, (const uint8_t* const)&params, 2);
				}
				else
				{
					const uint8_t params[] = { SEND_BATTERY_STATUS, VREFS_NOT_INITIALIZED };
					Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION_ERROR, (const uint8_t* const)&params, 2);
				}
			}
			break;
		case SET_BATTERY_VREF:
			APP_LOG(TS_OFF, VLEVEL_M, "Setting battery min and max vref\r\n");
			if (buffer_size < MESSAGE_MIN_BYTES + BATTERY_VREF_PARAMS_BYTE_COUNT) {
				const uint8_t params[] = { SET_BATTERY_VREF };
				Tx_Set_Buffer(RESPONSE_OUT, MISSING_DATA, (const uint8_t* const)&params, 1);
			}
			else
			{
				uint16_t min_vref = (buffer[PARAMETERS_START_BYTE] << 8) | buffer[PARAMETERS_START_BYTE + 1];
				uint16_t max_vref = (buffer[PARAMETERS_START_BYTE + 2] << 8) | buffer[PARAMETERS_START_BYTE + 3];
				APP_LOG(TS_OFF, VLEVEL_M, "min: %u    max: %u\r\n", min_vref, max_vref);
				Warning result = Set_Battery_Vref(min_vref, max_vref);
				if (result == NO_WARNING)
				{
					Tx_Set_Ack(SET_BATTERY_VREF);
				}
				else
				{
					const uint8_t params[] = { SET_BATTERY_VREF, result };
					Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION_WARNING, (const uint8_t* const)&params, 2);
				}
			}
			break;
    case SYNCHRONIZE_TIME_AND_DATE:
      if (buffer_size < TIME_DATE_BYTE_COUNT) {
        APP_LOG(TS_OFF, VLEVEL_M, "Time/date command input is to short!\r\n");
      } else {
        APP_LOG(TS_OFF, VLEVEL_M, "Update time/ date!\r\n");

        uint8_t hour = buffer[2];
        uint8_t minute = buffer[3];
        uint8_t second = buffer[4];
        uint8_t year = buffer[5];    // Assume offset from 2000
        uint8_t weekday = buffer[6]; // 1 = Monday, 7 = Sunday
        uint8_t month = buffer[7];   // 1–12
        uint8_t day = buffer[8];     // 1–31

        const char *weekday_names[] = {"Forbidden", "Monday",   "Tuesday",
                                       "Wednesday", "Thursday", "Friday",
                                       "Saturday",  "Sunday"};

        const char *weekday_str = "Unknown";

        if (weekday <= 7) {
          weekday_str = weekday_names[weekday];
        }

        APP_LOG(TS_OFF, VLEVEL_M, "Time: %02u:%02u:%02u\r\n", hour, minute,
                second);
        APP_LOG(TS_OFF, VLEVEL_M, "Date: %s %02u-%02u-%04u\r\n", weekday_str,
                day, month, 2000 + year);
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
      APP_LOG(TS_OFF, VLEVEL_M, "Unknown AB command: %02X\r\n",
              buffer[SUBTYPE_BYTE]);
      break;
    }
  } else {
    APP_LOG(TS_OFF, VLEVEL_M, "Unknown message type: %02X\r\n",
            buffer[IDENTIFIER_BYTE]);
  }
}
