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

#define RX_BUFFER_SIZE 64
#define MAX_RETRIES 25

void Interpret_Rx_Buffer(void);
void Print_Rx_Buffer(void);
void Add_To_Rx_Buffer(uint8_t *rxChar);
void Print_EUI(const char *label, uint8_t *eui);
static uint8_t hex2byte(const char *hex);
static void Handle_DevEUI_Command(char *hexStr);
static void Handle_JoinEUI_Command(char *hexStr);
void Handle_Join_Command(void);
bool set_devEUI(uint8_t *EUI);
bool set_joinEUI(uint8_t *EUI);
