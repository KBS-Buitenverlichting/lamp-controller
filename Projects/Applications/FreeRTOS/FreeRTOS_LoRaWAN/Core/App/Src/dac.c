/*
 * dac.c
 *
 *  Created on: May 16, 2025
 *      Author: Bjorn Wakker
 */
#include "dac.h"

void DAC_Enable()
{
	DAC->CR |= DAC_CR_EN1;
}

void DAC_Disable()
{
	DAC->CR &= ~(DAC_CR_EN1);
}

void DAC_Init(void)
{
	/* DAC peripheral clock Enabled */
	__HAL_RCC_DAC_CLK_ENABLE();

	/* Configure DAC (runs on PA10)
	 * DAC disabled (lamp off on startup)
	 * Normal mode, output to external pin, buffer enabled (MODE[2:0] = 0b000)
	 * Max brightness
	 */
	DAC->MCR &= ~(DAC_MCR_MODE1);
	DAC_Set_Brightness(UINT8_MAX);
	DAC_Disable();
}

Warning DAC_Set_Value(uint16_t value)
{
	const uint16_t battery_voltage = SYS_GetBatteryLevel();
	Warning result = NO_WARNING;

	if (value > battery_voltage)
	{
		value = battery_voltage;
		result = BATTERY_BELOW_LAMP_MAX_VDD;
	}

	/* Normalize from [0..VDD] to [0..DAC_MAX] (convert from voltage to register value) */
	const uint16_t normalized = (((uint32_t)value * DAC_MAX) / battery_voltage);
	DAC->DHR12R1 = (normalized & DAC_DHR12R1_DACC1DHR);

	return result;
}

Warning DAC_Set_Brightness(const uint8_t brightness) {
	if (brightness == 0) {
		DAC_Disable();
		if (Get_State_LampState() != MOTION_SENSOR) {
			Send_LampState(OFF);
		}
		return NO_WARNING;
	}

	if(Get_State_LampState() != MOTION_SENSOR) {
		DAC_Enable();
		Send_LampState(ON);
	}

	/* Normalize from [0..UINT8_MAX] to [LED_VMIN..LED_VMAX] (convert from digital value to voltage) */
	const uint16_t normalized = (((uint32_t) brightness * (LED_VMAX - LED_VMIN))
			/ UINT8_MAX) + LED_VMIN;
	return DAC_Set_Value(normalized);
}
