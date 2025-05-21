/*
 * serial_eui_setter.c
 *
 *  Created on: May 8, 2025
 *      Author: robdr
 */
#include "serial_eui_setter.h"

// Below are the standard EUIs, note that these will be reverted to when a power cycle occurs
uint8_t devEUI[] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x07, 0x02, 0x97 };
uint8_t joinEUI[] = { 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x02, 0x01 };

uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_buffer_index = 0;

typedef enum {
	CMD_UNKNOWN,
	CMD_DEVEUI,		// !DEVEUI=...
	CMD_JOINEUI,	// !JOINEUI=...
	CMD_JOIN,		// !JOIN
	CMD_PRINT		// !PRINT
} CommandType;

void Print_Rx_Buffer(void) {
	if (rx_buffer_index > 0) {
		vcom_Trace((uint8_t*) "Received: ", 10);
		vcom_Trace(rx_buffer, rx_buffer_index);
		rx_buffer_index = 0;
	}
}

void Add_To_Rx_Buffer(uint8_t *rxChar) {
	if (rx_buffer_index < RX_BUFFER_SIZE) {
		rx_buffer[rx_buffer_index++] = *rxChar;

		if (*rxChar == '\n') {
			if (!Interpret_Rx_Buffer()) {
				Print_Rx_Buffer();
			}
		}
	} else {
		Print_Rx_Buffer();
		rx_buffer[0] = *rxChar;
		rx_buffer_index = 1;
	}
}

void Print_EUI(const char *label, uint8_t *eui) {
	static char buffer[64];
	int len = snprintf(buffer, sizeof(buffer),
			"%s%02X%02X%02X%02X%02X%02X%02X%02X\r\n", label, eui[0], eui[1],
			eui[2], eui[3], eui[4], eui[5], eui[6], eui[7]);

	if (len > 0 && len < sizeof(buffer)) {
		vcom_Trace((uint8_t*) buffer, len);
	} else {
		const char *error = "Print_EUI: format overflow\r\n";
		vcom_Trace((uint8_t*) error, strlen(error));
	}
}

bool Interpret_Rx_Buffer(void) {
	if (rx_buffer_index < 2 || rx_buffer[0] != '!') {
		return false;
	}

	char *cmdStart = (char*) (rx_buffer + 1);

	CommandType cmdType = CMD_UNKNOWN;
	char *hexStr = NULL;

	if (strncmp(cmdStart, "DEVEUI=", 7) == 0) {
		cmdType = CMD_DEVEUI;
		hexStr = cmdStart + 7;
	} else if (strncmp(cmdStart, "JOINEUI=", 8) == 0) {
		cmdType = CMD_JOINEUI;
		hexStr = cmdStart + 8;
	} else if (strncmp(cmdStart, "JOIN", 4) == 0) {
		cmdType = CMD_JOIN;
	} else if (strncmp(cmdStart, "PRINT", 5) == 0) {
		cmdType = CMD_PRINT;
	}

	switch (cmdType) {
	case CMD_DEVEUI:
		Handle_DevEUI_Command(hexStr);
		break;
	case CMD_JOINEUI:
		Handle_JoinEUI_Command(hexStr);
		break;
	case CMD_JOIN:
		Handle_Join_Command();
		break;
	case CMD_PRINT:
		Print_EUIs();
		break;
	default:
		vcom_Trace((uint8_t*) "Unknown command\r\n", 20);
		break;
	}

	rx_buffer_index = 0;
	return true;
}

uint8_t hex2byte(const char *hex) {
	uint8_t high = (isdigit(hex[0]) ? hex[0] - '0' : toupper(hex[0]) - 'A' + 10);
	uint8_t low = (isdigit(hex[1]) ? hex[1] - '0' : toupper(hex[1]) - 'A' + 10);
	return (high << 4) | low;
}

void Handle_DevEUI_Command(char *hexStr) {
	uint8_t newDevEUI[8];
	for (int i = 0; i < 8; i++) {
		newDevEUI[i] = hex2byte(&hexStr[i * 2]);
	}
	LmHandlerStop();
	if (set_devEUI(newDevEUI)) {
		Print_EUI("New DevEUI: ", devEUI);
	} else {
		vcom_Trace((uint8_t *)"Please try again\r\n", 18);
	}
}

void Handle_JoinEUI_Command(char *hexStr) {
	uint8_t newJoinEUI[8];
	for (int i = 0; i < 8; i++) {
		newJoinEUI[i] = hex2byte(&hexStr[i * 2]);
	}
	LmHandlerStop();
	if (set_joinEUI(newJoinEUI)) {
		Print_EUI("New JoinEUI: ", joinEUI);
	} else {
		vcom_Trace((uint8_t *)"Please try again\r\n", 18);
	}
}

void Handle_Join_Command(void) {
	vcom_Trace((uint8_t*) "Trying join\r\n", 13);
	LmHandlerJoin(LORAWAN_DEFAULT_ACTIVATION_TYPE);
}

bool set_devEUI(uint8_t *EUI) {
	uint8_t tempEUI[8];
	int attempts = 0;

	// Retry setting DevEUI until it matches or max_attempts is reached
	vcom_Trace((uint8_t*) "Trying to update DevEUI...\r\n", 28);
	while (attempts++ < MAX_RETRIES) {
		if (LmHandlerSetDevEUI(EUI) == LORAMAC_HANDLER_SUCCESS) {
			LmHandlerGetDevEUI(tempEUI);
			if (memcmp(tempEUI, EUI, 8) == 0) {
				memcpy(devEUI, tempEUI, 8);
				vcom_Trace((uint8_t*) "Successfully updated DevEUI\r\n", 29);
				return true;
			}
		}
		LmHandlerStop();
		HAL_Delay(100);
	}

	vcom_Trace((uint8_t*) "Failed to set DevEUI after multiple attempts\r\n", 47);
	return false;
}

bool set_joinEUI(uint8_t *EUI) {
	uint8_t tempEUI[8];
	int attempts = 0;

	// Retry setting JoinEUI until it matches or max_attempts is reached
	vcom_Trace((uint8_t*) "Trying to update JoinEUI...\r\n", 29);
	while (attempts++ < MAX_RETRIES) {
		if (LmHandlerSetAppEUI(EUI) == LORAMAC_HANDLER_SUCCESS) {
			LmHandlerGetAppEUI(tempEUI);
			if (memcmp(tempEUI, EUI, 8) == 0) {
				memcpy(joinEUI, tempEUI, 8);
				vcom_Trace((uint8_t*) "Successfully updated JoinEUI\r\n", 30);
				return true;
			}
		}
		LmHandlerStop();
		HAL_Delay(100);
	}

	vcom_Trace((uint8_t*) "Failed to set JoinEUI after multiple attempts\r\n", 48);
	return false;
}

void Print_EUIs(void) {
	Print_EUI("DevEUI=", devEUI);
	Print_EUI("JoinEUI=", joinEUI);
}

