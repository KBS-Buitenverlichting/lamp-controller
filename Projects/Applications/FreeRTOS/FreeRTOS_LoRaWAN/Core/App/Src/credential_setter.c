/*
 * credential_setter.c
 *
 *  Created on: May 15, 2025
 *      Author: robdr
 */
#include "credential_setter.h"

uint8_t devEUI[] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x07, 0x02, 0x97 };
uint8_t joinEUI[] = { 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x02, 0x01 };

void set_devEUI(uint8_t *EUI) {
	memcpy(devEUI, EUI, sizeof(devEUI));
}

void set_joinEUI(uint8_t *EUI) {
	memcpy(joinEUI, EUI, sizeof(joinEUI));
}
