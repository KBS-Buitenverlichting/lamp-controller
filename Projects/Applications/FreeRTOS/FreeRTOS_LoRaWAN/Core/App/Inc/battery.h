/*
 * battery.h
 *
 *  Created on: May 9, 2025
 *      Author: Bjorn Wakker
 */
#pragma once

#include "stdint.h"
#include "transmit.h"
#include "stm32wlxx_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "main.h"

extern SemaphoreHandle_t sem_start_battery_read;

void Start_Get_Battery_Level_Task(void const *argument);
