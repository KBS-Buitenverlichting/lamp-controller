#pragma once

#include <stdint.h>
#include <stddef.h>
#include "stm32wlxx_hal.h"
#include "usart.h"

void Send_Serial_Data(const uint8_t* data, size_t length);
uint8_t* Receive_Serial_Data(uint8_t* buffer, size_t max_length, size_t* received_length);
