#include "receive.h"
#include "sys_app.h" // Used for APP_LOG to write output to serial

void Process_Rx_Data(const LmHandlerAppData_t *const app_data, const LmHandlerRxParams_t *const params)
{
    APP_LOG(TS_OFF, VLEVEL_M, "Received payload (hex): ");
    for (uint8_t i = 0; i < app_data->BufferSize; i++)
    {
    	APP_LOG(TS_OFF, VLEVEL_M, "%02X ", app_data->Buffer[i]);
    }
    APP_LOG(TS_OFF, VLEVEL_M, "\r\n");

    if (app_data->Buffer[0] == 0xAB)
    {
    	APP_LOG(TS_OFF, VLEVEL_M, "Instructions for lamp controller received\n");
    	switch (app_data->Buffer[1]){
			case 0x00:
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
				APP_LOG(TS_OFF, VLEVEL_M, "Lamp off\n");
				break;
			case 0x01:
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
				APP_LOG(TS_OFF, VLEVEL_M, "Lamp on\n");
				break;
    	}

    }
}
