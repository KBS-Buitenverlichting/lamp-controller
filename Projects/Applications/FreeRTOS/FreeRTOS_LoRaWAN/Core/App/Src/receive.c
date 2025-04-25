#include "receive.h"
#include "sys_app.h" // Used for APP_LOG to write output to serial

void Process_Rx_Data(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params)
{
    APP_LOG(TS_OFF, VLEVEL_M, "Received payload (hex): ");
    for (uint8_t i = 0; i < appData->BufferSize; i++)
    {
    	APP_LOG(TS_OFF, VLEVEL_M, "%02X ", appData->Buffer[i]);
    }
    APP_LOG(TS_OFF, VLEVEL_M, "\r\n");
}
