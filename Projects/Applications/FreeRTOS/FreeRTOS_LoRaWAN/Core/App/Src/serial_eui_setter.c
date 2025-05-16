/*
 * serial_interface.c
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
    	vcom_Trace("Ontvangen: ", 11);
        vcom_Trace_DMA(rx_buffer, rx_buffer_index);
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
