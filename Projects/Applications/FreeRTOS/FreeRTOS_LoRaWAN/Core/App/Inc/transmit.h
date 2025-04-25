#pragma once

struct LppData
{
	uint8_t sensor_id; // Has to be different from sensors with the same type
	uint8_t data_type;
	union data {
		uint8_t digital_value;
		int16_t analog_value;
		uint16_t luminosity;
		uint8_t presence;
		int16_t temperature;
		uint8_t relative_humidity;
		struct accelerometer {
			int16_t x;
			int16_t y;
			int16_t z;
		};
		uint16_t barometricPressure;
		struct gyrometer {
			int16_t x;
			int16_t y;
			int16_t z;
		};
		struct gps {
			int32_t lat;
			int32_t lon;
			int32_t alt;
		};
	};
};

void Tx_Reset_Buffer_Idx(void);
bool Tx_Add_Data(const LppData* const data);
void Tx_Transmit_Data(void);
