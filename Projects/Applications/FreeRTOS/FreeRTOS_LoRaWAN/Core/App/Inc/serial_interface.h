#pragma once

#include <stdint.h>
#include <stddef.h>
#include "stm32wlxx_hal.h"
#include "usart.h"
#include "sys_app.h"

#define RX_BUFFER_SIZE 10

extern uint8_t rx_buffer[RX_BUFFER_SIZE];
extern uint8_t rx_buffer_index;

void Print_Rx_Buffer(void);
void Add_To_Rx_Buffer(uint8_t *rxChar);
