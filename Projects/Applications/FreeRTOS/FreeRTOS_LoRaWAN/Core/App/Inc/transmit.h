/*
 * transmit_test.h
 *
 *  Created on: Apr 25, 2025
 *      Author: Bjorn Wakker
 */
#pragma once

#include "stdint.h"
#include "LmHandlerTypes.h"
#include "LmHandler.h"
#include "lora_app.h"

void Tx_Set_Buffer(const uint8_t identifier, const uint8_t subtype, const uint8_t* const parameters, const uint8_t nr_of_parameter_bytes);
void Tx_Transmit_Data(void);
