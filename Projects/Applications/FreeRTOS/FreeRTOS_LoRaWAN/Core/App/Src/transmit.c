#include "transmit.h"
#include "sys_app.h"
#include "LmHandlerTypes.h"
#include "LmHandler.h"
#include "lora_app.h"
#include "CayenneLpp.h"

static uint8_t tx_buffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];
static uint8_t tx_buffer_idx = 0;
static LmHandlerAppData_t tx_app_data = { LORAWAN_USER_APP_PORT, 0, tx_buffer };

void Tx_Reset_Buffer_Idx(void)
{
    tx_buffer_idx = 0;
}

bool Tx_Add_Data(const struct LppData* const lpp_data)
{
	uint8_t byte_count = 2; // Sensor id and data type

	switch (lpp_data->data_type) {
	    case LPP_DIGITAL_INPUT:
	    case LPP_DIGITAL_OUTPUT:
	    	byte_count += 1;
	        break;

	    case LPP_ANALOG_INPUT:
	    case LPP_ANALOG_OUTPUT:
	    	byte_count += 2;
	        break;

		case LPP_LUMINOSITY: // Not implemented
		case LPP_PRESENCE: // Not implemented
		case LPP_TEMPERATURE: // Not implemented
		case LPP_RELATIVE_HUMIDITY: // Not implemented
		case LPP_ACCELEROMETER: // Not implemented
		case LPP_BAROMETRIC_PRESSURE: // Not implemented
		case LPP_GYROMETER: // Not implemented
		case LPP_GPS: // Not implemented
	    default:
	    	break;
	}

	if (tx_buffer_idx + byte_count > LORAWAN_APP_DATA_BUFFER_MAX_SIZE)
	{
		return false;
	}

	tx_buffer[tx_buffer_idx++] = lpp_data->sensor_id;
	tx_buffer[tx_buffer_idx++] = lpp_data->data_type;

	switch(lpp_data->data_type)
	{
	case LPP_DIGITAL_INPUT:
	case LPP_DIGITAL_OUTPUT:
		tx_buffer[tx_buffer_idx++] = lpp_data->data.digital_value;
		break;

	case LPP_ANALOG_INPUT:
	case LPP_ANALOG_OUTPUT:
	{
		uint16_t value = lpp_data->data.analog_value * 100; // Multiply by 100 to conform to formatting (see CayenneLpp.h)
		tx_buffer[tx_buffer_idx++] = (uint8_t)((value >> 8) & 0xFF);
		tx_buffer[tx_buffer_idx++] = (uint8_t)(value & 0xFF);
		break;
	}

	case LPP_LUMINOSITY: // Not implemented
	case LPP_PRESENCE: // Not implemented
	case LPP_TEMPERATURE: // Not implemented
	case LPP_RELATIVE_HUMIDITY: // Not implemented
	case LPP_ACCELEROMETER: // Not implemented
	case LPP_BAROMETRIC_PRESSURE: // Not implemented
	case LPP_GYROMETER: // Not implemented
	case LPP_GPS: // Not implemented
	default:
		break;
	}

	return true;
}

void Tx_Transmit_Data(void)
{
	// Fill with data code block
	Tx_Reset_Buffer_Idx();

	struct LppData data;
	data.sensor_id = 0;
	data.data_type = LPP_DIGITAL_OUTPUT;
	data.data.digital_value = GetBatteryLevel();
	(void)Tx_Add_Data(&data);
	// Fill with data code block

	tx_app_data.BufferSize = tx_buffer_idx;

	UTIL_TIMER_Time_t nextTxIn = 0;
	if (LmHandlerSend(&tx_app_data, LORAWAN_DEFAULT_CONFIRMED_MSG_STATE, &nextTxIn, false) != LORAMAC_HANDLER_SUCCESS)
	{
	// Error handling? re-send already happens automatically as it runs on a timer
	}
}
