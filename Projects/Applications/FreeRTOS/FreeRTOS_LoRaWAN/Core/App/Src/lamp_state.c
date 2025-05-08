/*
 * lamp_state.c
 *
 *  Created on: May 8, 2025
 *      Author: Mariëlle
 */

#include "lamp_state.h"

static LampState Current_State = MotionSensor;

void Set_State_LampState(LampState State){
	Current_State = State;
}

LampState Get_State_LampState(void){
	return Current_State;
}

void Set_Brightness(uint8_t Brightness){
	Current_Brightness = Brightness;
}

uint8_t Get_Brightness(void){
	return Current_Brightness;
}
