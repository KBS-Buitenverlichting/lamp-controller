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

typedef enum {
    Off,
    On,
    MotionSensor
}LampState;

void LampState_Init(void);
void Send_LampState(LampState new_state);
LampState Get_State_LampState(void);
const char* LampState_ToString(LampState state);
void StartLampStateTask(void const *argument);
void Send_Brightness(uint8_t brightness);
uint8_t Get_Brightness(void);

