/*
 * lamp_state.h
 *
 *  Created on: May 8, 2025
 *      Author: Mariëlle
 */

#pragma once

#include "stdint.h"
#include "message_format.h"
#include "cmsis_os.h"
#include "sys_app.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "main.h"  // for GPIO control
#include "transmit.h"

#define MAX_BRIGHTNESS UINT8_MAX
#define MIN_BRIGHTNESS 0

typedef enum {
    OFF,
    ON,
    MOTION_SENSOR
}LampState;

typedef struct LampConfig {
	LampState state;
	uint8_t brightness;
} LampConfig;

extern LampConfig previous_lamp_config;
extern SemaphoreHandle_t sem_motion_sensor_signal;

void LampState_Init(void);
void Send_LampState(const LampState new_state);
LampState Get_State_LampState(void);
const char* LampState_ToString(const LampState state);
void Start_LampState_Task(void const *argument);
void Send_Brightness(const uint8_t brightness);
void Start_Motion_Sensor_Task(void const *argument);
void Lamp_On(void);
void Lamp_Off(void);
uint8_t Get_Brightness(void);
void Set_Dutycycle(const uint8_t dutycycle);

