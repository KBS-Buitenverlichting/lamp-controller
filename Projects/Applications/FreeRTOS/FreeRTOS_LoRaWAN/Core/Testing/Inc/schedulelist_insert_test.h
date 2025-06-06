/*
 * schedulelist_insert_test.h
 *
 *  Created on: Jun 3, 2025
 *      Author: Mariëlle
 */

#pragma once

#include "receive.h"
#include "schedules.h"

void Create_Test_Schedules(uint8_t amount);
uint8_t Test_Valid_Insert(void);
uint8_t Test_Max_Schedules(void);
uint8_t Test_Invalid_Data(void);
uint8_t Test_Overlap_Next(void);
uint8_t Test_Overlap_Previous(void);
uint8_t Test_Day_Differences(void);
