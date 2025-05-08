#pragma once

#include <stdint.h>
#include <stddef.h>
#include "stm32wlxx_hal.h"
#include "usart.h"
#include "sys_app.h"

void Send_Serial_Data(const uint8_t* data);
uint8_t* Receive_Serial_Data(uint8_t* buffer, size_t max_length, size_t* received_length);
