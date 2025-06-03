/*
 * lamp_state.c
 *
 *  Created on: May 8, 2025
 *      Author: Mariëlle
 */

#include "lamp_state.h"

LampConfig previous_lamp_config = {MOTION_SENSOR, MAX_BRIGHTNESS};

static LampConfig current_lamp_config = {MOTION_SENSOR, MAX_BRIGHTNESS};

static SemaphoreHandle_t state_mutex;
static QueueHandle_t lamp_state_queue;
static QueueHandle_t brightness_queue;

SemaphoreHandle_t sem_motion_sensor_signal;

extern TIM_HandleTypeDef tim17;

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

void Lamp_On(void)
{
	// Re-initialize after turning everything off
	TIM17_Init();
}

void Lamp_Off(void)
{
	HAL_TIM_PWM_Stop(&tim17, TIM_CHANNEL_1); // Disable the timer
	__HAL_RCC_TIM17_CLK_DISABLE(); // Disable the peripheral clock
}

uint8_t Get_Brightness(void) {
    uint8_t copy = MAX_BRIGHTNESS;
    if (xSemaphoreTake(state_mutex, portMAX_DELAY) == pdTRUE) {
        copy = current_lamp_config.brightness;
        xSemaphoreGive(state_mutex);
    }
    return copy;
}

void Set_Dutycycle(const uint8_t dutycycle)
{
	__HAL_TIM_SET_COMPARE(&tim17, TIM_CHANNEL_1, dutycycle); // Set output compare value
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
	            	Lamp_Off();
	            	break;
	            case ON:
	            	Lamp_On();
	            	break;
	            case MOTION_SENSOR:
	            	xSemaphoreGive(sem_motion_sensor_signal);
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

	        Set_Dutycycle(current_lamp_config.brightness);
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
				Lamp_On();
				Set_Dutycycle(Get_Brightness());
			} else {
				Lamp_Off();
			}
		}
	}
}
