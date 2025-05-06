#include "receive.h"
#include "sys_app.h" // Used for APP_LOG to write output to serial

void Interpret_Message(uint8_t *Buffer, uint8_t BufferSize){
	if(Buffer[0]==0xAB){
		switch(Buffer[1]){
			case 0x00:
				APP_LOG(TS_OFF, VLEVEL_M,"Zet lamp uit");
				break;
			case 0x01:
				APP_LOG(TS_OFF, VLEVEL_M,"Zet lamp aan");
				break;
			case 0x02:
				APP_LOG(TS_OFF, VLEVEL_M,"Stel lamp in op bewegingssensor");
				break;
			case 0x03:
				APP_LOG(TS_OFF, VLEVEL_M,"Verander helderheid naar ");
				APP_LOG(TS_OFF, VLEVEL_M, "%02X ",Buffer[2]);
				break;
			case 0x04:
				APP_LOG(TS_OFF, VLEVEL_M,"Rapporteer status batterij");
			}
	}
}

void Process_Rx_Data(const LmHandlerAppData_t *const app_data, const LmHandlerRxParams_t *const params)
{
    APP_LOG(TS_OFF, VLEVEL_M, "Received payload (hex): ");
    for (uint8_t i = 0; i < app_data->BufferSize; i++)
    {
    	APP_LOG(TS_OFF, VLEVEL_M, "%02X ", app_data->Buffer[i]);
    }
    APP_LOG(TS_OFF, VLEVEL_M, "\r\n");
    Interpret_Message(app_data->Buffer, app_data->BufferSize);
}
