/*
 * message_format.h
 *
 *  Created on: May 8, 2025
 *      Author: Bjorn Wakker
 */
#pragma once

#include "lora_app.h"

#define IDENTIFIER_BYTE 0
#define IDENTIFIER_BYTE_COUNT 1
#define SUBTYPE_BYTE 1
#define SUBTYPE_BYTE_COUNT 1
#define PARAMETERS_START_BYTE 2
#define PARAMETER_MAX_BYTES (LORAWAN_APP_DATA_BUFFER_MAX_SIZE - 2)

#define MESSAGE_MIN_BYTES (IDENTIFIER_BYTE_COUNT + SUBTYPE_BYTE_COUNT)

typedef enum Identifiers
{
	INSTRUCTION_IN = 0xAB,
	RESPONSE_OUT = 0xC0,
	RESPONSE_OUT_WITH_DATA = 0xC1,
	MESSAGE_OUT = 0xD1
} Identifier;

typedef enum InstructionSubtypes
{
	LAMP_OFF = 0x00,
	LAMP_ON = 0x01,
	ACTIVATE_MOTION_SENSOR = 0x02,
	CHANGE_BRIGHTNESS = 0x03,
	SEND_BATTERY_STATUS = 0x04,
	SET_BATTERY_VREF = 0x05,
	SYNCHRONIZE_TIME_AND_DATE = 0x06,
	SET_TIMESLOT = 0x07,
	SHOW_TIMETABLE = 0x08,
	CHANGE_TIMESLOT = 0x09,
	REMOVE_TIMESLOT = 0x0A
} InstructionSubtype;

typedef enum ResponseSubtypes
{
	INSTRUCTION_COMPLETED = 0x00,
	INVALID_DATA = 0x01,
	MISSING_DATA = 0x02
} ResponseSubtypes;

typedef enum ResponseWithDataSubtypes
{
	RESPONDING_TO_INSTRUCTION = 0x00,
	RESPONDING_TO_INSTRUCTION_WARNING = 0x01
} ResponseWithDataSubtypes;

typedef enum Warnings
{
	NO_WARNING = 0x00,
	MIN_VREF_BELOW_CUTOFF_BOARD = 0x50,
	MIN_VREF_ABOVE_MAX_VDD_BOARD = 0x51,
	MAX_VREF_ABOVE_MAX_VDD_BOARD = 0x52
} Warning;
