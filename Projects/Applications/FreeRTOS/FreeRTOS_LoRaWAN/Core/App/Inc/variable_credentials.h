/*********************************************************************
 * @file   variable_credentials.c
 * @brief  File for saving and retrieving EUIs to and from Flash
 *
 * @author KBS Buitenverlichting
 * @date   23 May 2025
 *********************************************************************/
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define EUI_SIZE 8
#define FLASH_VALID_MARKER 0xA5A5A5A5
#define FLASH_EUI_PTR ((const EUIStorage*) FLASH_EUI_ADDRESS)
#define FLASH_EUI_ADDRESS 0x0803F800

/**
 * @brief Structure to hold the EUI data
 */
typedef struct {
    uint32_t valid_marker;
    uint8_t devEUI[EUI_SIZE];
    uint8_t joinEUI[EUI_SIZE];
} EUIStorage;

/**
 * @brief Loads EUIs from flash
 *
 * @param[out] devEUI	The dev EUI from flash
 * @param[out] joinEUI	The join EUI from flash
 *
 * @return True if operation done successfully, else False
 */
bool Load_EUIs_From_Flash(uint8_t* devEUI, uint8_t* joinEUI);

/**
 * @brief Loads dev EUI from flash
 *
 * @param[out] devEUI	The dev EUI from flash
 *
 * @return True if operation done successfully, else False
 */
bool Load_DevEUI_From_Flash(uint8_t *devEUI);

/**
 * @brief Loads join EUI from flash
 *
 * @param[out] joinEUI	The join EUI from flash
 *
 * @return True if operation done successfully, else False
 */
bool Load_JoinEUI_From_Flash(uint8_t *joinEUI);

/**
 * @brief Store EUIs in flash
 *
 * @param[in] devEUI	The dev EUI from flash
 * @param[in] joinEUI	The join EUI from flash
 *
 * @return True if operation done successfully, else False
 */
bool Save_EUIs_To_Flash(const uint8_t* devEUI, const uint8_t* joinEUI);
