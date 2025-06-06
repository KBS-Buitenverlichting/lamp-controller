/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_lorawan.h"
#include "lamp_state.h"
#include "schedules.h"
#include "cmsis_os.h"
#include "lamp_state.h"

#ifdef TESTING
#include "testing.h"
#endif

/* Private variables ---------------------------------------------------------*/
LPTIM_HandleTypeDef hlptim1;
TIM_HandleTypeDef tim17;
TIM_OC_InitTypeDef tim17_oc;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_GPIO_Init(void);
static void MX_LPTIM1_Init(void);
/* USER CODE BEGIN PFP */
int32_t LED_control(int value);
void Lamp_GPIO_Init(void);
void Rx_Done(uint8_t *rx_char, uint16_t size, uint8_t error);
void Tx_Done(void *arg);
void Motion_Sensor_GPIO_Init(void);
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "serial_eui_setter.h"
#include "usart_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */
osThreadId general_taskhandle;
osThreadId LoRaWAN_TaskHandle;

/* Private function prototypes -----------------------------------------------*/
void StartLoRaWANTask(void const *argument);
void Start_General_Task(void const *argument);
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

#ifdef TESTING
  SystemApp_Init();
  Main_Test();
#else
  /* USER CODE BEGIN SysInit */
  MX_GPIO_Init();
  MX_LPTIM1_Init();
  TIM17_Init();
  Lamp_GPIO_Init();
  Motion_Sensor_GPIO_Init();
  LampState_Init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  osThreadDef(General_Task, Start_General_Task, osPriorityLow, 0, 128);
  general_taskhandle = osThreadCreate(osThread(General_Task), NULL);
  osThreadDef(LoRaWAN_Task, StartLoRaWANTask, osPriorityNormal, 0, 1024);
  LoRaWAN_TaskHandle = osThreadCreate(osThread(LoRaWAN_Task), NULL);
  osThreadDef(LampStateTask, Start_LampState_Task, osPriorityNormal, 0, 256);
  osThreadCreate(osThread(LampStateTask), NULL);
  osThreadDef(MotionSensorTask, Start_Motion_Sensor_Task, osPriorityNormal, 0,
              128);
  osThreadCreate(osThread(MotionSensorTask), NULL);
  osThreadDef(ProcessSchedulesTask, Start_Process_Schedules_Task, osPriorityNormal, 0, 512);
  osThreadCreate(osThread(ProcessSchedulesTask), NULL);
  osKernelStart();
  /* USER CODE END 2 */
#endif

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
   */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Configure the main internal regulator output voltage
   */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks
   */
  RCC_OscInitStruct.OscillatorType =
      RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI | RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
  RCC_OscInitStruct.LSIDiv = RCC_LSI_DIV1;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }
  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3 | RCC_CLOCKTYPE_HCLK |
                                RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief LPTIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_LPTIM1_Init(void) {

  /* USER CODE BEGIN LPTIM1_Init 0 */

  /* USER CODE END LPTIM1_Init 0 */

  /* USER CODE BEGIN LPTIM1_Init 1 */

  /* USER CODE END LPTIM1_Init 1 */
  hlptim1.Instance = LPTIM1;
  hlptim1.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
  hlptim1.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV1;
  hlptim1.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
  hlptim1.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_HIGH;
  hlptim1.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
  hlptim1.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
  hlptim1.Init.Input1Source = LPTIM_INPUT1SOURCE_GPIO;
  hlptim1.Init.Input2Source = LPTIM_INPUT2SOURCE_GPIO;
  if (HAL_LPTIM_Init(&hlptim1) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN LPTIM1_Init 2 */

  /* USER CODE END LPTIM1_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void TIM17_Init(void)
{
	  /* Tim17 Clock Enable */
	  __HAL_RCC_TIM17_CLK_ENABLE();

	  /* Configure TIM17
	   * Prescaler for 100 HZ frequency
	   * CounterMode UP
	   * Period 255
	   * No clock division
	   * AutoReloadPreload enabled
	   */
	  tim17.Instance = TIM17;
	  tim17.Init.Prescaler = TIM17_PRESCALER;
	  tim17.Init.CounterMode = TIM_COUNTERMODE_UP;
	  tim17.Init.Period = TIM17_PERIOD;
	  tim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  tim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    /* Configure TIM17 Output Compare
     * Mode PWM
     * Pulse 0 (start with duty cycle of 0%, lamp off)
     * Polarity HIGH (lamp on/ active on high)
     * Fast mode disabled
     */
	tim17_oc.OCMode = TIM_OCMODE_PWM1;
	tim17_oc.Pulse = 0;
	tim17_oc.OCPolarity = TIM_OCPOLARITY_HIGH;
	tim17_oc.OCFastMode = TIM_OCFAST_DISABLE;

    if (HAL_TIM_PWM_ConfigChannel(&tim17, &tim17_oc, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }

    // Timer init after output compare init to prevent short burst of pwm before switching to 0% duty cycle
    if (HAL_TIM_PWM_Init(&tim17) != HAL_OK) {
	    Error_Handler();
    }

    if (HAL_TIM_PWM_Start(&tim17, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */
void Lamp_GPIO_Init(void) {
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure GPIO pin : NSS (PB9)
   * Alternate function; Push Pull mode
   * No Pull-up or Pull-down active
   * Speed very high
   * Alternate function TIM17_CH1
   */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = PB9_AF_TIM17_CH1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Motion_Sensor_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(EXTI0_IRQn, 15, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

int32_t LED_control(int value) {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, value);
  return 0;
}
/* USER CODE END 4 */
/* USER CODE BEGIN 4 */

void StartLoRaWANTask(void const *argument) {
  /* init code for LoRaWAN */
  MX_LoRaWAN_Init();
  /* USER CODE BEGIN 5 */
#ifdef TESTING
  Main_Test();
#endif

  /* Infinite loop */
  for (;;) {
    MX_LoRaWAN_Process();
    osDelay(10);
  }
  /* USER CODE END 5 */
}
/* USER CODE END 4 */
void Start_General_Task(void const *argument) {
  LED_control(1);
  vcom_Init(Tx_Done);
  vcom_ReceiveInit(Rx_Done);
  Serial_Init();
  for (;;) {
    LED_control(0);
    osDelay(500);
    LED_control(1);
    osDelay(500);
  }
}

void Rx_Done(uint8_t *rx_char, uint16_t size, uint8_t error) {
  Add_To_Rx_Buffer(rx_char);
}

void Tx_Done(void *arg) {}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);
    osDelay(100);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */
  while (1) {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
