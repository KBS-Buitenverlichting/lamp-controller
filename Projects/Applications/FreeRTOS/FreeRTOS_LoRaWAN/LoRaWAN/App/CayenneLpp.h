/*!
  * @file      CayenneLpp.h
  *
  * @brief     Implements the Cayenne Low Power Protocol
  *
  * @copyright Revised BSD License, see section \ref LICENSE.
  *
  * @code
  *                ______                              _
  *               / _____)             _              | |
  *              ( (____  _____ ____ _| |_ _____  ____| |__
  *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
  *               _____) ) ____| | | || |_| ____( (___| | | |
  *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
  *              (C)2013-2018 Semtech
  *
  * @endcode
  *
  * @author    Miguel Luis ( Semtech )
  */
#ifndef __CAYENNE_LPP_H__
#define __CAYENNE_LPP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
#define CAYENNE_LPP_MAXBUFFER_SIZE                  242
#define LPP_DIGITAL_INPUT       0       /* 1 byte */
#define LPP_DIGITAL_OUTPUT      1       /* 1 byte */
#define LPP_ANALOG_INPUT        2       /* 2 bytes, 0.01 signed */
#define LPP_ANALOG_OUTPUT       3       /* 2 bytes, 0.01 signed */
#define LPP_LUMINOSITY          101     /* 2 bytes, 1 lux unsigned */
#define LPP_PRESENCE            102     /* 1 byte, 1 */
#define LPP_TEMPERATURE         103     /* 2 bytes, 0.1 Celsius degrees signed */
#define LPP_RELATIVE_HUMIDITY   104     /* 1 byte, 0.5% unsigned */
#define LPP_ACCELEROMETER       113     /* 2 bytes per axis, 0.001G */
#define LPP_BAROMETRIC_PRESSURE 115     /* 2 bytes 0.1 hPa Unsigned */
#define LPP_GYROMETER           134     /* 2 bytes per axis, 0.01 degrees/s */
#define LPP_GPS                 136     /* 3 byte lon/lat 0.0001 degrees, 3 bytes alt 0.01m */

/* Data ID + Data Type + Data Size */
#define LPP_DIGITAL_INPUT_SIZE       3
#define LPP_DIGITAL_OUTPUT_SIZE      3
#define LPP_ANALOG_INPUT_SIZE        4
#define LPP_ANALOG_OUTPUT_SIZE       4
#define LPP_LUMINOSITY_SIZE          4
#define LPP_PRESENCE_SIZE            3
#define LPP_TEMPERATURE_SIZE         4
#define LPP_RELATIVE_HUMIDITY_SIZE   3
#define LPP_ACCELEROMETER_SIZE       8
#define LPP_BAROMETRIC_PRESSURE_SIZE 4
#define LPP_GYROMETER_SIZE           8
#define LPP_GPS_SIZE                 11
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void CayenneLppInit(void);

void CayenneLppReset(void);

uint8_t CayenneLppGetSize(void);

uint8_t *CayenneLppGetBuffer(void);

uint8_t CayenneLppCopy(uint8_t *buffer);

uint8_t CayenneLppAddDigitalInput(uint8_t channel, uint8_t value);

uint8_t CayenneLppAddDigitalOutput(uint8_t channel, uint8_t value);

uint8_t CayenneLppAddAnalogInput(uint8_t channel, uint16_t value);

uint8_t CayenneLppAddAnalogOutput(uint8_t channel, uint16_t value);

uint8_t CayenneLppAddLuminosity(uint8_t channel, uint16_t lux);

uint8_t CayenneLppAddPresence(uint8_t channel, uint8_t value);

uint8_t CayenneLppAddTemperature(uint8_t channel, int16_t celsius);

uint8_t CayenneLppAddRelativeHumidity(uint8_t channel, uint16_t rh);

uint8_t CayenneLppAddAccelerometer(uint8_t channel, int16_t x, int16_t y, int16_t z);

uint8_t CayenneLppAddBarometricPressure(uint8_t channel, uint16_t hpa);

uint8_t CayenneLppAddGyrometer(uint8_t channel, int16_t x, int16_t y, int16_t z);

uint8_t CayenneLppAddGps(uint8_t channel, int32_t latitude, int32_t longitude, int32_t meters);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __CAYENNE_LPP_H__ */
