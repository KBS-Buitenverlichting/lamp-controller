/*
 * transmit_test.h
 *
 *  Created on: Apr 25, 2025
 *      Author: Bjorn Wakker
 */
#pragma once

#include "stdint.h"
#include "message_format.h"
#include "LmHandlerTypes.h"
#include "LmHandler.h"
#include "lora_app.h"

/**
 * @brief Clears the buffer and resets the size to 0
 */
void Tx_Clear_Buffer(void);

/**
 * @brief Sets the buffer for next data transmission
 *
 * @param identifier 			The message type
 * @param subtype	 			The message subtype
 * @param parameters 			The message parameters
 * @param nr_of_parameter_bytes The total byte count of parameters
 *
 * @see Documentation TO
 */
void Tx_Set_Buffer(const Identifier identifier, const uint8_t subtype, const uint8_t* const parameters, const uint8_t nr_of_parameter_bytes);

/**
 * @brief Sets the buffer up for a simple acknowledgement
 *
 * @param instruction The instruction you want to acknowledge
 *
 * @see Documentation TO
 */
void Tx_Set_Ack(const InstructionSubtype instruction);

/**
 * @brief Transmits the data buffer and clears it after a successfull send
 */
void Tx_Transmit_Data(void);
