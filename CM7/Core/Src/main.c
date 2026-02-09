/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "crc.h"
#include "dac.h"
#include "dma.h"
#include "dma2d.h"
#include "dsihost.h"
#include "fatfs.h"
#include "fdcan.h"
#include "i2c.h"
#include "jpeg.h"
#include "ltdc.h"
#include "mdma.h"
#include "rng.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_host.h"
#include "gpio.h"
#include "fmc.h"
#include "app_touchgfx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "quadspi.h"
#include "sdmmc.h"
#include "CR_shift_light.h"
#include "CR_encoder.h"
#include "CR_structs.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif

#define NUM_LEDS 7

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

CR_GPIO pin_encoder_SW = { .GPIO_Port = GPIOD, .GPIO_Pin = GPIO_PIN_12 };
CR_GPIO pin_encoder_DT = { .GPIO_Port = GPIOD, .GPIO_Pin = GPIO_PIN_13 };
CR_GPIO pin_encoder_CLK = { .GPIO_Port = GPIOB, .GPIO_Pin = GPIO_PIN_10 };
CR_GPIO pin_potentiometer_input = { .GPIO_Port = GPIOA, .GPIO_Pin = GPIO_PIN_0 }; // Potentiometer is PA0_C, which is connected to PA0
CR_GPIO pin_shift_light_pwm = { .GPIO_Port = GPIOA, .GPIO_Pin = GPIO_PIN_3 };


// CAN Bus Filter. Refer to RM0399 pg 2627 "Acceptance Filter."
// There are 2 filter types (one ext, one standard). We can have 128 filters for standard CAN.
// They are assigned to an RX FIFO/buffer, and each one can filter:
// - A range between two IDs
// - One or two dedicated IDs (ID1 == ID2, OR |ID1 - ID2| == 1)
// - Using a bit mask filter
FDCAN_FilterTypeDef CAN_filter = {
		.IdType = FDCAN_STANDARD_ID,
		.FilterIndex = 0, // can be 0-127, and this MUST be within the range of [0, #filters) you configured.
		.FilterType = FDCAN_FILTER_DUAL, // this is for one OR two specific ID filtering.
		.FilterConfig = FDCAN_FILTER_TO_RXFIFO0, // place matching ID messaged into RX FIFO0
		.FilterID1 = 0x7FE, // ID to filter for
		.FilterID2 = 0x7FF,
		.RxBufferIndex = 0, // This property is ignored for the above FilterConfig
		.IsCalibrationMsg = 0 // This property is ignored for the above FilterConfig
};


CR_shift_light shift_light_handle;
CR_encoder encoder_UI_handle;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
/* USER CODE BEGIN Boot_Mode_Sequence_0 */

/* USER CODE END Boot_Mode_Sequence_0 */

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
  //SCB_EnableDCache();

/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  //uwTickPrio = 4;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();
/* USER CODE BEGIN Boot_Mode_Sequence_2 */

/* USER CODE END Boot_Mode_Sequence_2 */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_MDMA_Init();
  MX_DMA_Init();
  MX_LTDC_Init();
  MX_USART1_UART_Init();
  MX_DAC1_Init();
  MX_DMA2D_Init();
  MX_FDCAN1_Init();
  MX_FDCAN2_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM5_Init();
  MX_UART4_Init();
  MX_UART8_Init();
  MX_CRC_Init();
  MX_I2C2_Init();
  MX_SPI2_Init();
  MX_TIM15_Init();
  MX_DSIHOST_DSI_Init();
  MX_RNG_Init();
  MX_FMC_Init();
  MX_JPEG_Init();
  MX_RTC_Init();
  MX_ADC1_Init();
  MX_ADC3_Init();
  MX_FATFS_Init();
  MX_I2C1_Init();
  MX_TouchGFX_Init();
  /* Call PreOsInit function */
  MX_TouchGFX_PreOSInit();
  /* USER CODE BEGIN 2 */
  if (BSP_SDRAM_SingleTest() != 0)
  {
	  //  Error_Handler();
  }
 // MX_SDMMC1_SD_Init();
  if (CSP_QUADSPI_Init() == HAL_OK) {
	  CSP_QSPI_EnableMemoryMappedMode();
  }
  // *****************************************
  // Citrus add Below
  // *****************************************

  if (CR_shift_light_init(&shift_light_handle, &htim5, LED_PWM_TICK_CYCLE, TIM_CHANNEL_4, NUM_LEDS) != CR_OK){
	  Error_Handler();
  }

  CR_encoder_init(&encoder_UI_handle, &pin_encoder_DT, &pin_encoder_CLK, &pin_encoder_SW);

  HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
  if (HAL_ADC_Start(&hadc1) != HAL_OK){
	  Error_Handler();
  }

  // Enable optional CAN filters. You MUST configure both global and regular config filter; by global default, rejects just dump into RXFIFO0... so in that case filter isn't active.
  HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);
  HAL_FDCAN_ConfigFilter(&hfdcan1, &CAN_filter);
  if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK){
	  Error_Handler();
  }

  if (HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1) != HAL_OK)
  {
      /* PWM Generation Error */
      Error_Handler();
  }

  // *****************************************
  // Citrus End
  // *****************************************
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  //TODO NOTE: I am using:
  // the tiny pin expander which attaches to pin range [1, 20]
  // - pin 37 AND onboard BTN1 PC6
  // - pin 20 PA0_C (uses ADC1_INN1) potentiometer // if this doesn't work, see if need to disable pin 22 PA1_C ADC1_INP1
  // - pin 8 PA4 (disable DAC1_OUT1) encoder GPIO IN

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 240;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 24;
  RCC_OscInitStruct.PLL.PLLR = 12;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVQ);
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLL1QCLK, RCC_MCODIV_1);
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLL3.PLL3M = 12;
  PeriphClkInitStruct.PLL3.PLL3N = 231;
  PeriphClkInitStruct.PLL3.PLL3P = 6;
  PeriphClkInitStruct.PLL3.PLL3Q = 6;
  PeriphClkInitStruct.PLL3.PLL3R = 6;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	CR_encoder_status result = CR_check_encoder(&encoder_UI_handle);
	if (result == ENCODER_CLICK){
		CR_set_all_lights(&shift_light_handle, 255, 255, 255);
		HAL_Delay(500);
		CR_set_all_lights(&shift_light_handle, 0, 0, 0);
		HAL_Delay(500);
		CR_set_all_lights(&shift_light_handle, 255, 255, 255);
		HAL_Delay(500);
		CR_set_all_lights(&shift_light_handle, 0, 0, 0);
		HAL_Delay(500);
	} else if (result == ENCODER_RIGHT){
		CR_set_all_lights(&shift_light_handle, 255, 0, 0);
		HAL_Delay(500);
		CR_set_all_lights(&shift_light_handle, 0, 0, 0);
		HAL_Delay(500);
		CR_set_all_lights(&shift_light_handle, 255, 0, 0);
		HAL_Delay(500);
		CR_set_all_lights(&shift_light_handle, 0, 0, 0);
		HAL_Delay(500);
	} else if (result == ENCODER_LEFT){
		CR_set_all_lights(&shift_light_handle, 0, 0, 255);
		HAL_Delay(500);
		CR_set_all_lights(&shift_light_handle, 0, 0, 0);
		HAL_Delay(500);
		CR_set_all_lights(&shift_light_handle, 0, 0, 255);
		HAL_Delay(500);
		CR_set_all_lights(&shift_light_handle, 0, 0, 0);
		HAL_Delay(500);
	}
}


/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
