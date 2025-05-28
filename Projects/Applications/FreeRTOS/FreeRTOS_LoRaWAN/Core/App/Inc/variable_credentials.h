/*
 * variable_credentials.h
 *
 *  Created on: May 15, 2025
 *      Author: robdr
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "stm32wlxx_hal.h"

extern uint8_t devEUI[];
extern uint8_t joinEUI[];

#define EUI_SIZE 8
#define FLASH_VALID_MARKER 0xA5A5A5A5
#define FLASH_EUI_PTR ((const EUIStorage*) FLASH_EUI_ADDRESS)
#define FLASH_EUI_ADDRESS 0x0803F800

typedef struct {
    uint32_t valid_marker;
    uint8_t devEUI[EUI_SIZE];
    uint8_t joinEUI[EUI_SIZE];
} EUIStorage;

bool Load_EUIs_From_Flash(uint8_t* devEUI, uint8_t* joinEUI);
bool Load_DevEUI_From_Flash(uint8_t *devEUI);
bool Load_JoinEUI_From_Flash(uint8_t *joinEUI);
bool Save_EUIs_To_Flash(const uint8_t* devEUI, const uint8_t* joinEUI);
