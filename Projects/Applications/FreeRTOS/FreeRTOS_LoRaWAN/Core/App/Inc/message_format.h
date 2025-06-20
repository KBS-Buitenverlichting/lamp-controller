/*********************************************************************
 * @file   message_format.h
 * @brief  File for all message format related structs, enums, defines
 *
 * @author KBS Buitenverlichting
 * @date   8 May 2025
 *********************************************************************/
#pragma once

#include "lora_app.h"

// Format related defines
#define IDENTIFIER_BYTE 0
#define IDENTIFIER_BYTE_COUNT 1
#define SUBTYPE_BYTE 1
#define SUBTYPE_BYTE_COUNT 1
#define PARAMETERS_START_BYTE 2
#define PARAMETER_MAX_BYTES (LORAWAN_APP_DATA_BUFFER_MAX_SIZE - ((IDENTIFIER_BYTE_COUNT + SUBTYPE_BYTE_COUNT)))

#define MESSAGE_MIN_BYTES (IDENTIFIER_BYTE_COUNT + SUBTYPE_BYTE_COUNT)

// Instruction parameter byte counts
#define BRIGHTNESS_PARAMS_BYTE_COUNT 1
#define BATTERY_VREF_PARAMS_BYTE_COUNT 4
#define TIME_DATE_BYTE_COUNT 9
#define SET_TIMESCHEDULE_BYTE_COUNT 18

/**
 * @brief All possible identifiers
 *
 * @detail
 * Message identifiers are used to check what type
 * of message is being send to or from the module
 *
 * @see Documentation TO
 */
typedef enum Identifiers {
	INSTRUCTION_IN = 0xAB,
	RESPONSE_OUT = 0xC0,
	RESPONSE_OUT_WITH_DATA = 0xC1,
	MESSAGE_OUT = 0xD1
} Identifier;

/**
 * @brief All possible subtypes for instructions
 *
 * @detail
 * Instruction subtypes are used to check what type
 * of instruction is being send to the module
 *
 * @see Documentation TO
 */
typedef enum InstructionSubtypes {
	LAMP_OFF = 0x00,
	LAMP_ON = 0x01,
	ACTIVATE_MOTION_SENSOR = 0x02,
	CHANGE_BRIGHTNESS = 0x03,
	SEND_BATTERY_STATUS = 0x04,
	SYNCHRONIZE_TIME_AND_DATE = 0x06,
	SET_TIMESCHEDULE = 0x07,
	SHOW_TIMETABLE = 0x08,
	REMOVE_TIMESCHEDULE = 0x0A
} InstructionSubtype;

/**
 * @brief All possible subtypes for responses
 *
 * @detail
 * Response subtypes are used to check what type
 * of response is being send from the module
 *
 * @see Documentation TO
 */
typedef enum ResponseSubtypes {
	INSTRUCTION_COMPLETED = 0x00,
	INVALID_DATA = 0x01,
	MISSING_DATA = 0x02
} ResponseSubtype;

/**
 * @brief All possible subtypes for responses with data
 *
 * @detail
 * Response with data subtypes are used to check what type
 * of response data is being send from the module
 *
 * @see Documentation TO
 */
typedef enum ResponseWithDataSubtypes {
	RESPONDING_TO_INSTRUCTION = 0x00,
	RESPONDING_TO_INSTRUCTION_WARNING = 0x01,
	RESPONDING_TO_INSTRUCTION_ERROR = 0x02
} ResponseWithDataSubtype;

/**
 * @brief All warning codes
 *
 * @see Documentation TO
 */
typedef enum Warnings {
	NO_WARNING = 0x00
} Warning;

/**
 * @brief All error codes
 *
 * @see Documentation TO
 */
typedef enum Errors {
	NO_ERROR = 0x00,
	VREFS_NOT_INITIALIZED = 0x50,
	FAILED_TO_SET_RTC = 0x60,
	SCHEDULE_NOT_FOUND = 0xA0,
	LIST_FULL = 0x70,
	SCHEDULE_OVERLAP = 0x71,
	ERROR_SCHEDULE_BEFORE_RTC_TIME = 0x72
} Error;
