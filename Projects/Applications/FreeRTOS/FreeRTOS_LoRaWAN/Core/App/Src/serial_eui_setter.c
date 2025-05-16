/*
 * serial_eui_setter.c
 *
 *  Created on: May 8, 2025
 *      Author: robdr
 */
#include "serial_eui_setter.h"

uint8_t devEUI[] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x07, 0x02, 0x97 };
uint8_t joinEUI[] = { 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x02, 0x01 };

uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_buffer_index = 0;

void set_devEUI(uint8_t *EUI) {
	memcpy(devEUI, EUI, sizeof(devEUI));
}

void set_joinEUI(uint8_t *EUI) {
	memcpy(joinEUI, EUI, sizeof(joinEUI));
}

void Print_Rx_Buffer(void) {
    if (rx_buffer_index > 0) {
    	//vcom_Trace("Ontvangen: ", 11);
        //vcom_Trace_DMA(rx_buffer, rx_buffer_index);
    	Print_EUI("Join", joinEUI);
        rx_buffer_index = 0;
    }
}

void Add_To_Rx_Buffer(uint8_t *rxChar) {
    if (rx_buffer_index < RX_BUFFER_SIZE) {
        rx_buffer[rx_buffer_index++] = *rxChar;

        if (*rxChar == '\n') {
            Print_Rx_Buffer();
        }
    } else {
        Print_Rx_Buffer();
        rx_buffer[0] = *rxChar;
        rx_buffer_index = 1;
    }
}

void Print_EUI(const char *label, uint8_t *eui) {
    char buffer[64];
    int len = snprintf(buffer, sizeof(buffer),
                       "%s: %02X%02X%02X%02X%02X%02X%02X%02X\r\n",
                       label,
                       eui[0], eui[1], eui[2], eui[3],
                       eui[4], eui[5], eui[6], eui[7]);

    if (len > 0 && len < sizeof(buffer)) {
        vcom_Trace_DMA((uint8_t *)buffer, len);
    } else {
        const char *error = "Print_EUI: format overflow\r\n";
        vcom_Trace_DMA((uint8_t *)error, strlen(error));
    }
}

/*
static uint8_t hex2byte(const char *hex) {
    uint8_t high = (isdigit(hex[0]) ? hex[0] - '0' : toupper(hex[0]) - 'A' + 10);
    uint8_t low  = (isdigit(hex[1]) ? hex[1] - '0' : toupper(hex[1]) - 'A' + 10);
    return (high << 4) | low;
}

void Interperet_Rx_Buffer(void) {
    // Check of buffer de prefix DEVEUI= heeft (case sensitive)
    if (rx_buffer_index < 14) return; // minimaal 6 + 16 chars + 1 (newline) ?

    if (strncmp((char *)rx_buffer, "DEVEUI=", 7) == 0) {
        // Daarna moeten 16 hex tekens volgen (8 bytes)
        char *hexStr = (char *)(rx_buffer + 7);

        uint8_t newDevEUI[8];
        for (int i = 0; i < 8; i++) {
            newDevEUI[i] = hex2byte(&hexStr[i * 2]);
        }
        set_devEUI(newDevEUI);
    }

    // reset buffer na interpretatie
    rx_buffer_index = 0;
}
*/
