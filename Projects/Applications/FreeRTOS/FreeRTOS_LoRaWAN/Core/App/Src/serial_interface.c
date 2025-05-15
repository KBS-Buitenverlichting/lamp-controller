/*
 * serial_interface.c
 *
 *  Created on: May 8, 2025
 *      Author: robdr
 */
#include "serial_interface.h"

uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_buffer_index = 0;

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
