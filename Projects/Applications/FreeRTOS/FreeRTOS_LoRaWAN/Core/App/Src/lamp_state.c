/*
 * lamp_state.c
 *
 *  Created on: May 8, 2025
 *      Author: Mariëlle
 */

#include "lamp_state.h"

#define MAX_BRIGHTNESS 255
#define MIN_BRIGHTNESS 0

static LampState CurrentLampState = MotionSensor;
static uint8_t current_brightness = 255;

static SemaphoreHandle_t StateMutex;
static QueueHandle_t LampStateQueue;
static QueueHandle_t BrightnessQueue;

///Brief: Initializes lamp state and brightness queues and mutex.
void LampState_Init(void) {
    StateMutex = xSemaphoreCreateMutex();
    LampStateQueue = xQueueCreate(5, sizeof(LampState));  // queue size = 5 messages
    BrightnessQueue = xQueueCreate(5, sizeof(uint8_t));

    if (StateMutex == NULL || LampStateQueue == NULL) {
        APP_LOG(TS_OFF, VLEVEL_M, "Error initializing state system!\r\n");
    }
    if (BrightnessQueue == NULL) {
        APP_LOG(TS_OFF, VLEVEL_M, "Error initializing brightness system!\r\n");
    }
}

LampState Get_State_LampState(void) {
    LampState copy = Off;
    if (xSemaphoreTake(StateMutex, portMAX_DELAY) == pdTRUE) {
        copy = CurrentLampState;
        xSemaphoreGive(StateMutex);
    }
    return copy;
}

/// Brief: Converts a LampState enum to a string.
const char* LampState_ToString(LampState state) {
    switch (state) {
        case Off: return "Off";
        case On: return "On";
        case MotionSensor: return "Motion Sensor";
        default: return "Unknown";
    }
}

/// Brief: Sends a new lamp state to the queue.
void Send_LampState(LampState new_state) {
    if (xQueueSend(LampStateQueue, &new_state, 0) != pdPASS) {
        APP_LOG(TS_OFF, VLEVEL_M, "Failed to enqueue LampState\r\n");
    }
}

/// Brief: Sends a new brightness value to the queue.
void Send_Brightness(uint8_t brightness) {
    if (xQueueSend(BrightnessQueue, &brightness, 0) != pdPASS) {
        APP_LOG(TS_OFF, VLEVEL_M, "Failed to enqueue brightness\r\n");
    }
}

uint8_t Get_Brightness(void) {
    uint8_t copy = MAX_BRIGHTNESS;
    if (xSemaphoreTake(StateMutex, portMAX_DELAY) == pdTRUE) {
        copy = current_brightness;
        xSemaphoreGive(StateMutex);
    }
    return copy;
}

/// Brief: Main task loop for handling lamp state and brightness.
void StartLampStateTask(void const *argument) {
	LampState incoming_state;
	uint8_t incoming_brightness;

	for (;;) {
	    // Check for state change
	    if (xQueueReceive(LampStateQueue, &incoming_state, 0) == pdTRUE) {
	        if (xSemaphoreTake(StateMutex, portMAX_DELAY) == pdTRUE) {
	            CurrentLampState = incoming_state;
	            xSemaphoreGive(StateMutex);
	        }

	        switch (CurrentLampState) {
	            case Off:
	            	APP_LOG(TS_OFF, VLEVEL_M, "Lamp off\r\n");
	                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	                break;
	            case On:
	            	APP_LOG(TS_OFF, VLEVEL_M, "Lamp on\r\n");
	                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
	                break;
	            case MotionSensor:
	            	APP_LOG(TS_OFF, VLEVEL_M, "Select motion sensor!\r\n");
	                break;
	        }

	        APP_LOG(TS_OFF, VLEVEL_M, "Lamp state changed to: %s\r\n", LampState_ToString(CurrentLampState));
	    }

	    // Check for brightness change
	    if (xQueueReceive(BrightnessQueue, &incoming_brightness, 0) == pdTRUE) {
	    	if (incoming_brightness <= MESSAGE_MIN_BYTES) {
	    		APP_LOG(TS_OFF, VLEVEL_M, "Brightness command does not include brightness\r\n");
	    	}
	        if (incoming_brightness < MIN_BRIGHTNESS || incoming_brightness > MAX_BRIGHTNESS) {
	            incoming_brightness = MAX_BRIGHTNESS;
	            APP_LOG(TS_OFF, VLEVEL_M, "Brightness outside range, brightness is now %u!\r\n", incoming_brightness);
	        }
	        if (xSemaphoreTake(StateMutex, portMAX_DELAY) == pdTRUE) {
	            current_brightness = incoming_brightness;
	            APP_LOG(TS_OFF, VLEVEL_M, "Change brightness to %u!\r\n", incoming_brightness);
	            xSemaphoreGive(StateMutex);
	        }

	        APP_LOG(TS_OFF, VLEVEL_M, "Brightness changed to: %u\r\n", current_brightness);
	    }

	    osDelay(10);  // Give other tasks a chance
	}
}
