/*
 * transmit_test.h
 *
 *  Created on: Apr 25, 2025
 *      Author: Bjorn Wakker
 */
#pragma once

#include "stdint.h"
#include "stdbool.h"

struct LppData
{
	uint8_t sensor_id; // Has to be different from sensors with the same type
	uint8_t data_type; // Data type to indicate what type of sensor the data is from
	union {
		uint8_t digital_value;
		int16_t analog_value;
		uint16_t luminosity;
		uint8_t presence;
		int16_t temperature;
		uint8_t relative_humidity;
		struct {
			int16_t x;
			int16_t y;
			int16_t z;
		} accelerometer;
		uint16_t barometricPressure;
		struct {
			int16_t x;
			int16_t y;
			int16_t z;
		} gyrometer;
		struct {
			int32_t lat;
			int32_t lon;
			int32_t alt;
		} gps;
	} data;
};

void Tx_Reset_Buffer_Index(void);
bool Tx_Add_Data(const struct LppData* const data);
void Tx_Transmit_Data(void);
