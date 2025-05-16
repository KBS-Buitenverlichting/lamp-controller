#pragma once

#include <stdint.h>
#include <stddef.h>
#include "stm32wlxx_hal.h"
#include "usart.h"
#include "sys_app.h"
#include "variable_credentials.h"

#define RX_BUFFER_SIZE 10

void Print_Rx_Buffer(void);
void Add_To_Rx_Buffer(uint8_t *rxChar);
