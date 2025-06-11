/*********************************************************************
 * @file   lamp_state.c
 * @brief  File for managing the lamp
 *
 * @author KBS Buitenverlichting
 * @date   8 May 2025
 *********************************************************************/
#include "lamp_state.h"
#include "main.h"  // for GPIO control
#include "message_format.h"
#include "cmsis_os.h"
#include "sys_app.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "transmit.h"

LampConfig previous_lamp_config = {MOTION_SENSOR, MAX_BRIGHTNESS};
static LampConfig current_lamp_config = {MOTION_SENSOR, MAX_BRIGHTNESS};

static SemaphoreHandle_t state_mutex;
static QueueHandle_t lamp_state_queue;
static QueueHandle_t brightness_queue;

SemaphoreHandle_t sem_motion_sensor_signal;

TIM_HandleTypeDef tim17;
static TIM_OC_InitTypeDef tim17_oc;

void Lamp_GPIO_Init(void)
{
  /* GPIO Port B Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure GPIO pin : NSS (PB9)
   * Alternate function; Push Pull mode
   * No Pull-up or Pull-down active
   * Speed very high
   * Alternate function TIM17_CH1
   */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = PB9_AF_TIM17_CH1;

  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Lamp_PWM_Init(void)
{
  /* TIM17 Clock Enable */
  __HAL_RCC_TIM17_CLK_ENABLE();

  /* Configure TIM17
   * Prescaler for 100 HZ frequency
   * CounterMode UP
   * Period 255
   * No clock division
   * AutoReloadPreload enabled
   */
  tim17.Instance = TIM17;
  tim17.Init.Prescaler = TIM17_PRESCALER;
  tim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim17.Init.Period = TIM17_PERIOD;
  tim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

  /* Configure TIM17 Output Compare
   * Mode PWM
   * Pulse 0 (start with duty cycle of 0%, lamp off)
   * Polarity HIGH (lamp on/ active on high)
   * Fast mode disabled
   */
  tim17_oc.OCMode = TIM_OCMODE_PWM1;
  tim17_oc.Pulse = 0;
  tim17_oc.OCPolarity = TIM_OCPOLARITY_HIGH;
  tim17_oc.OCFastMode = TIM_OCFAST_DISABLE;

  if (HAL_TIM_PWM_ConfigChannel(&tim17, &tim17_oc, TIM_CHANNEL_1) != HAL_OK) {
	Error_Handler();
  }

  // Timer init after output compare init to prevent short burst of pwm before switching to 0% duty cycle
  if (HAL_TIM_PWM_Init(&tim17) != HAL_OK) {
	Error_Handler();
  }

  if (HAL_TIM_PWM_Start(&tim17, TIM_CHANNEL_1) != HAL_OK) {
	Error_Handler();
  }
}

void LampState_Init(void) {
    state_mutex = xSemaphoreCreateMutex();
    sem_motion_sensor_signal = xSemaphoreCreateBinary();
    lamp_state_queue = xQueueCreate(5, sizeof(LampState));  // queue size = 5 messages
    brightness_queue = xQueueCreate(5, sizeof(uint8_t));

    if (state_mutex == NULL || lamp_state_queue == NULL) {
        Error_Handler();
    }
    if (brightness_queue == NULL) {
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

void Send_LampState(const LampState new_state) {
    if (xQueueSend(lamp_state_queue, &new_state, 0) != pdPASS) {
        Error_Handler();
    }
}

void Send_Brightness(const uint8_t brightness) {
    if (xQueueSend(brightness_queue, &brightness, 0) != pdPASS) {
        Error_Handler();
    }
}

void Lamp_On(void)
{
	// Re-initialize after turning everything off
	Lamp_PWM_Init();
	Set_Duty_Cycle(Get_Brightness());
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

void Set_Duty_Cycle(const uint8_t duty_cycle)
{
	__HAL_TIM_SET_COMPARE(&tim17, TIM_CHANNEL_1, duty_cycle); // Set output compare value
}

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

	        Set_Duty_Cycle(current_lamp_config.brightness);
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
			} else {
				Lamp_Off();
			}
		}
	}
}
