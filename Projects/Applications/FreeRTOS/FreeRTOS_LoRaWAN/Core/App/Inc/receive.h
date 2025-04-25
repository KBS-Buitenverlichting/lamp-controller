/*
 * receive.h
 *
 *  Created on: Apr 24, 2025
 *      Author: robdr
 */

#pragma once

#include "LmHandler.h"
#include "LmHandlerTypes.h"

void Process_Rx_Data(const LmHandlerAppData_t *app_data, const LmHandlerRxParams_t *params);
