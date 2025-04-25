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
}
