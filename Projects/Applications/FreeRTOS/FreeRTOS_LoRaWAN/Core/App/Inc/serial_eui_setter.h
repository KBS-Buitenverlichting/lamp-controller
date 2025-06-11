/*********************************************************************
 * @file   serial_eui_setter.c
 * @brief  File for setting EUIs over serial
 *
 * @author KBS Buitenverlichting
 * @date   8 May 2025
 *********************************************************************/
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define RX_BUFFER_SIZE 64
#define PRINT_BUFFER_SIZE 64
#define MAX_RETRIES 25

extern bool continue_lora;

/**
 * @brief All serial commands
 */
typedef enum CommandTypes {
	CMD_UNKNOWN,
	CMD_DEVEUI,  // !DEVEUI=...
	CMD_JOINEUI, // !JOINEUI=...
	CMD_JOIN,    // !JOIN
	CMD_PRINT,   // !PRINT
	CMD_STATUS   // !STATUS
} CommandType;

/**
 * @brief Load EUIs from flash and check join status
 */
void Serial_Init(void);

/**
 * @brief Adds received data over serial to the buffer
 *
 * @param[in] rx_char The received data
 */
void Add_To_Rx_Buffer(const uint8_t *const rx_char);

/**
 * @brief Loads EUIs from flash in to variable
 */
void Update_EUIs(void);

/**
 * @brief Prints EUIs
 */
void Print_EUIs(void);

/**
 * @brief Prints buffer
 */
void Print_Rx_Buffer(void);

/**
 * @brief Prints a single EUI
 *
 * @param[in] label	Text preceding the EUI
 * @param[in] eui	The EUI
 */
void Print_EUI(const char *const label, const uint8_t *const eui);

/**
 * @brief Checks what instruction is to be executed
 *
 * @return True if buffer was correctly interpreted, else False
 */
bool Interpret_Rx_Buffer(void);

/**
 * @brief Translates a string hex value to a byte representation
 *
 * @param[in] hex A string hex value with length of 1 byte (without preceding "0x..")
 *
 * @return The byte representation
 */
uint8_t Hex_To_Byte(const char *const hex);

/**
 * @brief Handles the Dev EUI command
 *
 * @param[in] hex_str The serial string input containing the Dev EUI
 */
void Handle_DevEUI(const char *const hex_str);

/**
 * @brief Handles the Join EUI command
 *
 * @param[in] hex_str The serial string input containing the Join EUI
 */
void Handle_JoinEUI(const char *const hex_str);

/**
 * @brief Initializes LoRa to try a Join
 */
void Handle_Join(void);

/**
 * @brief Prints connection status
 */
void Handle_Status(void);

/**
 * @brief Sets the Dev EUI
 *
 * @param[in] EUI The Dev EUI
 *
 * @return True if operation was successful, else False
 */
bool Set_DevEUI(uint8_t *EUI);

/**
 * @brief Sets the Join EUI
 *
 * @param[in] EUI The Join EUI
 *
 * @return True if operation was successful, else False
 */
bool Set_JoinEUI(uint8_t *EUI);

/**
 * @brief Gets the EUIs that are currently used for connection
 */
void Get_Currently_Used_EUIs(void);

/**
 * @brief Checks if an EUI is empty
 *
 * @param[in] eui The EUI to check
 *
 * @return True if empty, else False
 */
bool Is_EUI_Empty(const uint8_t *eui);

/**
 * @brief Checks if device is connected over LoRa
 *
 * @return True on connection, else False
 */
bool Is_Joined(void);

/**
 * @brief Disconnects the device
 */
void Disconnect(void);
