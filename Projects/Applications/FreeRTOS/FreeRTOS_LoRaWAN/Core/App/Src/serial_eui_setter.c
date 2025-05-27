/*
 * serial_eui_setter.c
 *
 *  Created on: May 8, 2025
 *      Author: robdr
 */
#include "serial_eui_setter.h"

static void Print_EUIs(void);
static void Print_Rx_Buffer(void);
static void Print_EUI(const char* const label, const uint8_t* const eui);
static bool Interpret_Rx_Buffer(void);
static uint8_t Hex_To_Byte(const char* const hex);
static void Handle_DevEUI_Command(const char* const hex_str);
static void Handle_JoinEUI_Command(const char* const hex_str);
static void Handle_Join_Command(void);
static bool Set_DevEUI(uint8_t *EUI);
static bool Set_JoinEUI(uint8_t *EUI);

// Below are the standard EUIs, note that these will be reverted to when a power cycle occurs
uint8_t devEUI[] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x07, 0x02, 0x96 };
uint8_t joinEUI[] = { 0x01, 0x0A, 0x02, 0x0B, 0x03, 0x0C, 0x04, 0x0D };

uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_buffer_index = 0;

typedef enum CommandTypes {
	CMD_UNKNOWN,
	CMD_DEVEUI,		// !DEVEUI=...
	CMD_JOINEUI,	// !JOINEUI=...
	CMD_JOIN,		// !JOIN
	CMD_PRINT		// !PRINT
} CommandType;

void Add_To_Rx_Buffer(const uint8_t* const rx_char) {
	if (rx_buffer_index < RX_BUFFER_SIZE) {
		rx_buffer[rx_buffer_index++] = *rx_char;

		if (*rx_char == '\n') {
			if (!Interpret_Rx_Buffer()) {
				Print_Rx_Buffer();
			}
		}
	} else {
		Print_Rx_Buffer();
		rx_buffer[0] = *rx_char;
		rx_buffer_index = 1;
	}
}

static void Print_EUIs(void) {
	Print_EUI("DevEUI=", devEUI);
	Print_EUI("JoinEUI=", joinEUI);
}

static void Print_Rx_Buffer(void) {
	if (rx_buffer_index > 0) {
		vcom_Trace((uint8_t*) "Received: ", 10);
		vcom_Trace(rx_buffer, rx_buffer_index);
		rx_buffer_index = 0;
	}
}

static void Print_EUI(const char* const label, const uint8_t* const eui) {
	static char buffer[PRINT_BUFFER_SIZE];
	uint8_t len = snprintf(buffer, sizeof(buffer),
			"%s%02X%02X%02X%02X%02X%02X%02X%02X\r\n", label, eui[0], eui[1],
			eui[2], eui[3], eui[4], eui[5], eui[6], eui[7]);

	if (len > 0 && len < sizeof(buffer)) {
		vcom_Trace((uint8_t*) buffer, len);
	} else {
		const char *error = "Print_EUI: format overflow\r\n";
		vcom_Trace((uint8_t*) error, strlen(error));
	}
}

static bool Interpret_Rx_Buffer(void) {
	if (rx_buffer_index < 2 || rx_buffer[0] != '!') {
		return false;
	}

	char *cmd_start = (char*) (rx_buffer + 1);

	CommandType cmd_type = CMD_UNKNOWN;
	char *hex_str = NULL;

	if (strncmp(cmd_start, "DEVEUI=", 7) == 0) {
		cmd_type = CMD_DEVEUI;
		hex_str = cmd_start + 7;
	} else if (strncmp(cmd_start, "JOINEUI=", 8) == 0) {
		cmd_type = CMD_JOINEUI;
		hex_str = cmd_start + 8;
	} else if (strncmp(cmd_start, "JOIN", 4) == 0) {
		cmd_type = CMD_JOIN;
	} else if (strncmp(cmd_start, "PRINT", 5) == 0) {
		cmd_type = CMD_PRINT;
	}

	switch (cmd_type) {
	case CMD_DEVEUI:
		Handle_DevEUI_Command(hex_str);
		break;
	case CMD_JOINEUI:
		Handle_JoinEUI_Command(hex_str);
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

static uint8_t Hex_To_Byte(const char* const hex) {
	uint8_t high = (isdigit((uint8_t)hex[0]) ? hex[0] - '0' : toupper(hex[0]) - 'A' + 10);
	uint8_t low = (isdigit((uint8_t)hex[1]) ? hex[1] - '0' : toupper(hex[1]) - 'A' + 10);
	return (high << 4) | low;
}

static void Handle_DevEUI_Command(const char* const hex_str) {
	uint8_t new_devEUI[EUI_SIZE];
	for (uint8_t i = 0; i < EUI_SIZE; i++) {
		new_devEUI[i] = Hex_To_Byte(&hex_str[i * 2]);
	}
	LmHandlerStop();
	if (Set_DevEUI(new_devEUI)) {
		Print_EUI("New DevEUI: ", devEUI);
	} else {
		vcom_Trace((uint8_t *)"Please try again\r\n", 18);
	}
}

static void Handle_JoinEUI_Command(const char* const hex_str) {
	uint8_t new_joinEUI[EUI_SIZE];
	for (uint8_t i = 0; i < EUI_SIZE; i++) {
		new_joinEUI[i] = Hex_To_Byte(&hex_str[i * 2]);
	}
	LmHandlerStop();
	if (Set_JoinEUI(new_joinEUI)) {
		Print_EUI("New JoinEUI: ", joinEUI);
	} else {
		vcom_Trace((uint8_t *)"Please try again\r\n", 18);
	}
}

static void Handle_Join_Command(void) {
	vcom_Trace((uint8_t*) "Trying join...\r\n", 16);
	LmHandlerJoin(LORAWAN_DEFAULT_ACTIVATION_TYPE);
}

static bool Set_DevEUI(uint8_t *EUI) {
	uint8_t tempEUI[EUI_SIZE];
	uint8_t attempts = 0;

	// Retry setting DevEUI until it matches or max_attempts is reached
	vcom_Trace((uint8_t*) "Trying to update DevEUI...\r\n", 28);
	while (attempts++ < MAX_RETRIES) {
		if (LmHandlerSetDevEUI(EUI) == LORAMAC_HANDLER_SUCCESS) {
			LmHandlerGetDevEUI(tempEUI);
			if (memcmp(tempEUI, EUI, EUI_SIZE) == 0) {
				memcpy(devEUI, tempEUI, EUI_SIZE);
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

static bool Set_JoinEUI(uint8_t *EUI) {
	uint8_t tempEUI[EUI_SIZE];
	uint8_t attempts = 0;

	// Retry setting JoinEUI until it matches or max_attempts is reached
	vcom_Trace((uint8_t*) "Trying to update JoinEUI...\r\n", 29);
	while (attempts++ < MAX_RETRIES) {
		if (LmHandlerSetAppEUI(EUI) == LORAMAC_HANDLER_SUCCESS) {
			LmHandlerGetAppEUI(tempEUI);
			if (memcmp(tempEUI, EUI, EUI_SIZE) == 0) {
				memcpy(joinEUI, tempEUI, EUI_SIZE);
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
