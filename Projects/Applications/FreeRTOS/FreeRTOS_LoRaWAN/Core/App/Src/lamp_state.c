/*
 * lamp_state.c
 *
 *  Created on: May 8, 2025
 *      Author: Mariëlle
 */

#include "lamp_state.h"
#include "stdint.h"

static uint8_t Current_Brightness = 255;

static LampState Current_State = MotionSensor;

void Set_State_LampState(LampState State){
	Current_State = State;
}

LampState Get_State_LampState(void){
	return Current_State;
}

void Set_Brightness(uint8_t Brightness){
	if(Brightness < 255 && Brightness > 0){
	Current_Brightness = Brightness;
	}	else{
		Current_Brightness = 255;
	}
}

uint8_t Get_Brightness(void){
	return Current_Brightness;
}

const char* LampState_ToString(LampState state) {
    switch (state) {
        case Off: return "Off";
        case On: return "On";
        case MotionSensor: return "Motion Sensor";
        default: return "Unknown";
    }
}

