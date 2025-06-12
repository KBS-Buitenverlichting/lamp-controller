/*********************************************************************
 * @file   variable_credentials.c
 * @brief  File for saving and retrieving EUIs to and from Flash
 *
 * @author KBS Buitenverlichting
 * @date   23 May 2025
 *********************************************************************/
#include "variable_credentials.h"
#include <string.h>
#include "stm32wlxx_hal.h"

extern uint8_t devEUI[];
extern uint8_t joinEUI[];

bool Load_EUIs_From_Flash(uint8_t *devEUI, uint8_t *joinEUI) {
	if (FLASH_EUI_PTR->valid_marker == FLASH_VALID_MARKER) {
		memcpy(devEUI, FLASH_EUI_PTR->devEUI, EUI_SIZE);
		memcpy(joinEUI, FLASH_EUI_PTR->joinEUI, EUI_SIZE);
		return true;
	}
	return false;
}

bool Load_DevEUI_From_Flash(uint8_t *devEUI) {
	if (FLASH_EUI_PTR->valid_marker == FLASH_VALID_MARKER) {
		memcpy(devEUI, FLASH_EUI_PTR->devEUI, EUI_SIZE);
		return true;
	}
	return false;
}

bool Load_JoinEUI_From_Flash(uint8_t *joinEUI) {
	if (FLASH_EUI_PTR->valid_marker == FLASH_VALID_MARKER) {
		memcpy(joinEUI, FLASH_EUI_PTR->joinEUI, EUI_SIZE);
		return true;
	}
	return false;
}

bool Save_EUIs_To_Flash(const uint8_t *devEUI, const uint8_t *joinEUI) {
	HAL_FLASH_Unlock();

	// Remove old EUIs
	FLASH_EraseInitTypeDef eraseInitStruct = {
		.TypeErase = FLASH_TYPEERASE_PAGES,
		.Page = (FLASH_EUI_ADDRESS - FLASH_BASE) / FLASH_PAGE_SIZE,
		.NbPages = 1
	};
	uint32_t sectorError = 0;
	if (HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError) != HAL_OK) {
		HAL_FLASH_Lock();
		return false;
	}

	// Store new EUIs
	EUIStorage dataToSave = { .valid_marker = FLASH_VALID_MARKER };
	memcpy(dataToSave.devEUI, devEUI, EUI_SIZE);
	memcpy(dataToSave.joinEUI, joinEUI, EUI_SIZE);

	uint64_t *src = (uint64_t*) &dataToSave;
	for (uint32_t offset = 0; offset < sizeof(EUIStorage); offset += 8) {
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_EUI_ADDRESS + offset, src[offset / 8]) != HAL_OK) {
			HAL_FLASH_Lock();
			return false;
		}
	}

	HAL_FLASH_Lock();
	return true;
}

