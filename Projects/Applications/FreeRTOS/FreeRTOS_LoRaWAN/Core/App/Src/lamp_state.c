/*
 * lamp_state.c
 *
 *  Created on: May 8, 2025
 *      Author: Mariëlle
 */

#include "lamp_state.h"

#define MAX_BRIGHTNESS UINT8_MAX
#define MIN_BRIGHTNESS 0

LampConfig previous_lamp_config = {MOTION_SENSOR, MAX_BRIGHTNESS};

static LampConfig current_lamp_config = {MOTION_SENSOR, MAX_BRIGHTNESS};

static SemaphoreHandle_t state_mutex;
static QueueHandle_t lamp_state_queue;
static QueueHandle_t brightness_queue;

SemaphoreHandle_t sem_motion_sensor_signal;

///Brief: Initializes lamp state and brightness queues and mutex.
void LampState_Init(void) {
    state_mutex = xSemaphoreCreateMutex();
    sem_motion_sensor_signal = xSemaphoreCreateBinary();
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
        copy = current_lamp_config.state;
        xSemaphoreGive(state_mutex);
    }
    return copy;
}

/// Brief: Converts a LampState enum to a string.
const char* LampState_ToString(const LampState state) {
    switch (state) {
        case OFF: return "Off";
        case ON: return "On";
        case MOTION_SENSOR: return "Motion Sensor";
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
void Send_Brightness(const uint8_t brightness) {
    if (xQueueSend(brightness_queue, &brightness, 0) != pdPASS) {
        APP_LOG(TS_OFF, VLEVEL_M, "Failed to enqueue brightness\r\n");
    }
}

uint8_t Get_Brightness(void) {
    uint8_t copy = MAX_BRIGHTNESS;
    if (xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE) {
        copy = current_lamp_config.brightness;
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
	            current_lamp_config.state = incoming_state;
	            xSemaphoreGive(state_mutex);
	        }

	        switch (current_lamp_config.state) {
	            case OFF:
	                DAC_Disable();
	            	break;
	            case ON:
	            	DAC_Enable();
	            	break;
	            case MOTION_SENSOR:
					// handled in interrupt
	                break;
	            default:
	            	break;
	        }
	    }

	    // Check for brightness change
	    if (xQueueReceive(brightness_queue, &incoming_brightness, 0) == pdTRUE) {
	        if (xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE) {
	            current_lamp_config.brightness = incoming_brightness;
	            xSemaphoreGive(state_mutex);
	        }

	        Warning result = DAC_Set_Brightness(current_lamp_config.brightness);

	    	if (result != NO_WARNING)
	    	{
	    		const uint8_t params[] = { CHANGE_BRIGHTNESS, result };
	    		Tx_Set_Buffer(RESPONSE_OUT_WITH_DATA, RESPONDING_TO_INSTRUCTION_WARNING, (const uint8_t* const)&params, sizeof(params));
	    		return;
	    	}
	    }

	    osDelay(10);  // Give other tasks a chance
	}
}

void Start_Motion_Sensor_Task(void const *argument) {
	for (;;) {
		if (xSemaphoreTake(sem_motion_sensor_signal, portMAX_DELAY) != pdTRUE) {
			Error_Handler();
		}
		if(Get_State_LampState() == MOTION_SENSOR) {
			if (GPIOA->IDR & GPIO_PIN_0) {
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
			} else {
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
			}
		}
	}
}
