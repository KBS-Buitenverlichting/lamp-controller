/*
 * lamp_state.h
 *
 *  Created on: May 8, 2025
 *      Author: Mariëlle
 */

#pragma once

#include "stdint.h"

static uint8_t Current_Brightness = 255;
typedef enum {
    Off,
    On,
    MotionSensor
}LampState;


void Set_State_LampState(LampState State);
LampState Get_State_LampState(void);
void Set_Brightness(uint8_t Brightness);
uint8_t Get_Brightness(void);

