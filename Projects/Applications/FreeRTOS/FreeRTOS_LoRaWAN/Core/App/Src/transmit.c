#include "transmit.h"
#include "sys_app.h"
#include "LmHandlerTypes.h"
#include "LmHandler.h"
#include "CayenneLpp.h"
#include "lora_app.h"

static uint8_t tx_buffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];
static uint8_t tx_buffer_idx = 0;
static LmHandlerAppData_t tx_app_data = { LORAWAN_USER_APP_PORT, 0, buffer };

void Tx_Reset_Buffer_Idx(void)
{
	tx_buffer_idx = 0;
}

void Tx_Add_Data(const uint8_t data_id, const uint8_t data_size, )
{
	if (tx_buffer_idx + tx_data_size > LORAWAN_APP_DATA_BUFFER_MAX_SIZE)
	{
		return;
	}

	tx_buffer[tx_buffer_idx++] = data_id;
	tx_buffer[]
}

void Transmit_Data(void)
{
  buffer_idx = 0;

  uint8_t channel = 0;
  UTIL_TIMER_Time_t nextTxIn = 0;
//  uint8_t batteryLevel = GetBatteryLevel();

//  AppDataBuffer;
//  AppData.BufferSize

//  CayenneLppReset();
//  CayenneLppAddBarometricPressure(channel++, 0);
//  CayenneLppAddDigitalInput(channel++, GetBatteryLevel());

  if ((buffer_idx + LPP_DIGITAL_INPUT_SIZE) > LORAWAN_APP_DATA_BUFFER_MAX_SIZE)
  {
    return 0;
  }
  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_DIGITAL_INPUT;
  CayenneLppBuffer[CayenneLppCursor++] = value;
  /* USER CODE BEGIN CayenneLppAddDigitalInput_2 */

  CayenneLppCopy(app_data.Buffer);
  app_data.BufferSize = CayenneLppGetSize();

  if (LmHandlerSend(&app_data, LORAWAN_DEFAULT_CONFIRMED_MSG_STATE, &nextTxIn, false) != LORAMAC_HANDLER_SUCCESS)
  {
	// Error handling? re-send already happens automatically
  }
}
