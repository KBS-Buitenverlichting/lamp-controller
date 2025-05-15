/*
 * lamp_state.c
 *
 *  Created on: May 8, 2025
 *      Author: Mariëlle
 */

#include "lamp_state.h"

#define MAX_BRIGHTNESS 255
#define MIN_BRIGHTNESS 0

static LampState current_lamp_state = MOTIONSENSOR;
static uint8_t current_brightness = MAX_BRIGHTNESS;

static SemaphoreHandle_t state_mutex;
static QueueHandle_t lamp_state_queue;
static QueueHandle_t brightness_queue;

///Brief: Initializes lamp state and brightness queues and mutex.
void LampState_Init(void) {
    state_mutex = xSemaphoreCreateMutex();
    lamp_state_queue = xQueueCreate(5, sizeof(LampState));  // queue size = 5 messages
    brightness_queue = xQueueCreate(5, sizeof(uint8_t));

    if (state_mutex == NULL || lamp_state_queue == NULL) {
        APP_LOG(TS_OFF, VLEVEL_M, "Error initializing state system!\r\n");
        Error_Handler();
    }
    if (brightness_queue == NULL) {
        APP_LOG(TS_OFF, VLEVEL_M, "Error initializing brightness system!\r\n");
        Error_Handler();
    }
}

LampState Get_State_LampState(void) {
    LampState copy = OFF;
    if (xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE) {
        copy = current_lamp_state;
        xSemaphoreGive(state_mutex);
    }
    return copy;
}

/// Brief: Converts a LampState enum to a string.
const char* LampState_ToString(const LampState state) {
    switch (state) {
        case OFF: return "Off";
        case ON: return "On";
        case MOTIONSENSOR: return "Motion Sensor";
        default: return "Unknown";
    }
}

/// Brief: Sends a new lamp state to the queue.
void Send_LampState(const LampState new_state) {
    if (xQueueSend(lamp_state_queue, &new_state, 0) != pdPASS) {
        APP_LOG(TS_OFF, VLEVEL_M, "Failed to enqueue LampState\r\n");
    }
}

/// Brief: Sends a new brightness value to the queue.
void Send_Brightness(const LampState brightness) {
    if (xQueueSend(brightness_queue, &brightness, 0) != pdPASS) {
        APP_LOG(TS_OFF, VLEVEL_M, "Failed to enqueue brightness\r\n");
    }
}

uint8_t Get_Brightness(void) {
    uint8_t copy = MAX_BRIGHTNESS;
    if (xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE) {
        copy = current_brightness;
        xSemaphoreGive(state_mutex);
    }
    return copy;
}

/// Brief: Main task loop for handling lamp state and brightness.
void Start_LampState_Task(void const *argument) {
	LampState incoming_state;
	uint8_t incoming_brightness;

	for (;;) {
	    // Check for state change
	    if (xQueueReceive(lamp_state_queue, &incoming_state, 0) == pdTRUE) {
	        if (xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE) {
	            current_lamp_state = incoming_state;
	            xSemaphoreGive(state_mutex);
	        }

	        switch (current_lamp_state) {
	            case OFF:
	            	APP_LOG(TS_OFF, VLEVEL_M, "Lamp off\r\n");
	                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	                break;
	            case ON:
	            	APP_LOG(TS_OFF, VLEVEL_M, "Lamp on\r\n");
	                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
	                break;
	            case MOTIONSENSOR:
	            	APP_LOG(TS_OFF, VLEVEL_M, "Select motion sensor!\r\n");
	                break;
	        }

	        APP_LOG(TS_OFF, VLEVEL_M, "Lamp state changed to: %s\r\n", LampState_ToString(current_lamp_state));
	    }

	    // Check for brightness change
	    if (xQueueReceive(brightness_queue, &incoming_brightness, 0) == pdTRUE) {
	    	if (incoming_brightness <= MESSAGE_MIN_BYTES) {
	    		APP_LOG(TS_OFF, VLEVEL_M, "Brightness command does not include brightness\r\n");
	    	}
	        if (incoming_brightness > MAX_BRIGHTNESS) {
	            incoming_brightness = MAX_BRIGHTNESS;
	            APP_LOG(TS_OFF, VLEVEL_M, "Brightness outside range, brightness is now %u!\r\n", incoming_brightness);
	        }
	        if (xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE) {
	            current_brightness = incoming_brightness;
	            APP_LOG(TS_OFF, VLEVEL_M, "Change brightness to %u!\r\n", incoming_brightness);
	            xSemaphoreGive(state_mutex);
	        }

	        APP_LOG(TS_OFF, VLEVEL_M, "Brightness changed to: %u\r\n", current_brightness);
	    }

	    osDelay(10);  // Give other tasks a chance
	}
}
