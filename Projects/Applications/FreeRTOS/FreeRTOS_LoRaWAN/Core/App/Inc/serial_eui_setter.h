/*
 * serial_eui_setter.h
 *
 *  Created on: May 8, 2025
 *      Author: robdr
 */
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include "stm32wlxx_hal.h"
#include "usart.h"
#include "sys_app.h"
#include "variable_credentials.h"
#include "LmHandler.h"
#include "lora_app.h"

#define RX_BUFFER_SIZE 64
#define PRINT_BUFFER_SIZE 64
#define MAX_RETRIES 25
#define EUI_SIZE 8

void Add_To_Rx_Buffer(const uint8_t* const rx_char);
void Serial_Init(void);
