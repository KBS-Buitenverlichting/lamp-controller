/*
 * receive.h
 *
 *  Created on: Apr 24, 2025
 *      Author: robdr
 */

#pragma once

#include "stm32wlxx_hal.h"
#include "LmHandler.h"
#include "LmHandlerTypes.h"

void Process_Rx_Data(const LmHandlerAppData_t *const app_data, const LmHandlerRxParams_t *const params);
