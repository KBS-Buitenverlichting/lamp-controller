/*
 * serial_interface.c
 *
 *  Created on: May 8, 2025
 *      Author: robdr
 */
#include "serial_interface.h"

void Send_Serial_Data(const uint8_t* data) {
    APP_PRINTF(data);
}

uint8_t* Receive_Serial_Data(uint8_t* buffer, size_t max_length, size_t* received_length) {
    if (HAL_UART_Receive(&huart2, buffer, max_length, 1000) == HAL_OK) {
        *received_length = max_length;
    } else {
        *received_length = 0;
    }
    return buffer;
}
