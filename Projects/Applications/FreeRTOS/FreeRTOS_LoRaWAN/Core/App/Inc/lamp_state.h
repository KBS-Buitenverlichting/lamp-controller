/*********************************************************************
 * @file   lamp_state.h
 * @brief  File for managing the lamp
 *
 * @author KBS Buitenverlichting
 * @date   8 May 2025
 *********************************************************************/
#pragma once

#include <stdint.h>

#define MAX_BRIGHTNESS UINT8_MAX
#define MIN_BRIGHTNESS 0

/**
 * @brief All lamp states
 */
typedef enum {
	OFF,
	ON,
	MOTION_SENSOR
} LampState;

/**
 * @brief Struct to hold the lamp configuration
 */
typedef struct LampConfig {
	LampState state;
	uint8_t brightness;
} LampConfig;

/**
 * @brief Initializes the lamp gpio pins
 */
void Lamp_GPIO_Init(void);

/**
 * @brief Initializes a timer to control the lamp brightness using pwm
 */
void Lamp_PWM_Init(void);

/**
 * @brief Initializes lamp state and brightness queues and mutex
 */
void LampState_Init(void);

/**
 * @brief Sends a new lamp state to the queue
 */
void Send_LampState(const LampState new_state);

/**
 * @brief Gets the current lamp state
 *
 * @return The lamp state
 */
LampState Get_State_LampState(void);

/**
 * @brief Converts a LampState enum to a string
 *
 * @param[in] state The state to convert
 *
 * @return The output string
 */
const char* LampState_ToString(const LampState state);

/**
 * @brief Main task loop for handling lamp state and brightness
 *
 * @param[in] argument Any argument the function may receive
 */
void Start_LampState_Task(void const *argument);

/**
 * @brief Sends a new brightness value to the queue
 *
 * @param[in] brightness The new brightness
 */
void Send_Brightness(const uint8_t brightness);

/**
 * @brief Struct to hold the lamp configuration
 *
 * @param[in] argument Any argument the function may receive
 */
void Start_Motion_Sensor_Task(void const *argument);

/**
 * @brief Initializes the lamp and turns it on at the last used brightness
 */
void Lamp_On(void);

/**
 * @brief Turns off the lamp and its peripherals
 */
void Lamp_Off(void);

/**
 * @brief Gets the brightness
 *
 * @return The currently set brightness
 */
uint8_t Get_Brightness(void);

/**
 * @brief Sets the pwm duty cycle (corresponds to the brightness)
 *
 * @param[in] duty_cycle The new duty cycle
 */
void Set_Duty_Cycle(const uint8_t duty_cycle);
