/*
 * transmit_test.h
 *
 *  Created on: May 6, 2025
 *      Author: Bjorn Wakker
 */
#pragma once

#include "stdint.h"
#include "transmit.h"
#include "lora_app.h"

/**
 * @brief Tests the Tx_Clear_Buffer function
 *
 * @return if success 0 else the test case it failed on
 */
uint8_t Tx_Clear_Buffer_Test(void);

/**
 * @brief Tests the Tx_Set_Buffer function
 *
 * @return if success 0 else the test case it failed on
 */
uint8_t Tx_Set_Buffer_Test(void);

/**
 * @brief Tests the Tx_Set_Ack function
 *
 * @return if success 0 else the test case it failed on
 */
uint8_t Tx_Set_Ack_Test(void);
