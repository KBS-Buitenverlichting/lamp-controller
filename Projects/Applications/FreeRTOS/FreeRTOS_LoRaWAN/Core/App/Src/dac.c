/*
 * dac.c
 *
 *  Created on: May 16, 2025
 *      Author: Bjorn Wakker
 */
#include "dac.h"

void DAC_Init(void)
{
	/* DAC peripheral clock Enabled */
	__HAL_RCC_DAC_CLK_ENABLE();

	/* Configure DAC (runs on PA10)
	 * CR : DAC enabled
	 * MCR : Normal mode, output to external pin, buffer enabled (MODE[2:0] = 0b000)
	 * DHR8R1 : Data register for output value, 0 volts
	 */
	DAC->CR |= DAC_CR_EN1;
	DAC->MCR &= ~(DAC_MCR_MODE1);
//	DAC->DHR12R1 = 0;
	DAC_Set_Brightness(UINT8_MAX);
}

void DAC_Set_Value(const uint16_t value)
{
	if (value > VREF)
	{
		return;
	}

	const uint16_t normalized = (((uint32_t)value * DAC_MAX) / VREF);
	DAC->DHR12R1 = (normalized & DAC_DHR12R1_DACC1DHR);
}

void DAC_Set_Brightness(const uint8_t brightness)
{
	const uint16_t normalized = (((uint32_t)brightness * (LED_VMAX - LED_VMIN)) / UINT8_MAX) + LED_VMIN;
	DAC_Set_Value(normalized);
}
