/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fdcan.h"
#include "adc.h"
#include "CR_CAN_parse.h"
#include "CR_shift_light.h"
#include "CR_structs.h"
#include "CR_encoder.h"

extern 	FDCAN_TxHeaderTypeDef TxHeader;
extern CR_CAN_vals latest_CAN_Vals;
extern CR_shift_light shift_light_handle;
extern CR_GPIO pin_btn_menu;
extern CR_GPIO pin_btn_back;
extern uint8_t menu_btn_state;
extern uint8_t back_btn_state;
extern CR_encoder_status encoder_status;
extern CR_encoder encoder_UI_handle;



uint8_t TxData[] = {0x10, 0x32, 0x54, 0x76, 0x98, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x00};
FDCAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[16];
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for VideoTask */
osThreadId_t VideoTaskHandle;
const osThreadAttr_t VideoTask_attributes = {
  .name = "VideoTask",
  .stack_size = 4096 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for TouchGFXTask */
osThreadId_t TouchGFXTaskHandle;
const osThreadAttr_t TouchGFXTask_attributes = {
  .name = "TouchGFXTask",
  .stack_size = 8192 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for ButtonTask */
osThreadId_t ButtonTaskHandle;
const osThreadAttr_t ButtonTask_attributes = {
  .name = "ButtonTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for PollADC */
osThreadId_t PollADCHandle;
const osThreadAttr_t PollADC_attributes = {
  .name = "PollADC",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for ReceiveCANTask */
osThreadId_t ReceiveCANTaskHandle;
const osThreadAttr_t ReceiveCANTask_attributes = {
  .name = "ReceiveCANTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for ShiftLightTask */
osThreadId_t ShiftLightTaskHandle;
const osThreadAttr_t ShiftLightTask_attributes = {
  .name = "ShiftLightTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow2,
};
/* Definitions for UIButtonUpdate */
osThreadId_t UIButtonUpdateHandle;
const osThreadAttr_t UIButtonUpdate_attributes = {
  .name = "UIButtonUpdate",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for ButtonQueue */
osMessageQueueId_t ButtonQueueHandle;
const osMessageQueueAttr_t ButtonQueue_attributes = {
  .name = "ButtonQueue"
};
/* Definitions for ADCQueue */
osMessageQueueId_t ADCQueueHandle;
const osMessageQueueAttr_t ADCQueue_attributes = {
  .name = "ADCQueue"
};
/* Definitions for CAN_640_Queue */
osMessageQueueId_t CAN_640_QueueHandle;
const osMessageQueueAttr_t CAN_640_Queue_attributes = {
  .name = "CAN_640_Queue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
extern void videoTaskFunc(void *argument);
extern void TouchGFX_Task(void *argument);
void ButtonTaskFunc(void *argument);
void PollADCFunc(void *argument);
void ReceiveCANFunc(void *argument);
void ShiftLightFunc(void *argument);
void UIButtonUpdateFunc(void *argument);

extern void MX_USB_HOST_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 2 */
void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of ButtonQueue */
  ButtonQueueHandle = osMessageQueueNew (16, sizeof(uint8_t), &ButtonQueue_attributes);

  /* creation of ADCQueue */
  ADCQueueHandle = osMessageQueueNew (16, sizeof(uint16_t), &ADCQueue_attributes);

  /* creation of CAN_640_Queue */
  CAN_640_QueueHandle = osMessageQueueNew (16, sizeof(uint64_t), &CAN_640_Queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of VideoTask */
  VideoTaskHandle = osThreadNew(videoTaskFunc, NULL, &VideoTask_attributes);

  /* creation of TouchGFXTask */
  TouchGFXTaskHandle = osThreadNew(TouchGFX_Task, NULL, &TouchGFXTask_attributes);

  /* creation of ButtonTask */
  ButtonTaskHandle = osThreadNew(ButtonTaskFunc, NULL, &ButtonTask_attributes);

  /* creation of PollADC */
  PollADCHandle = osThreadNew(PollADCFunc, NULL, &PollADC_attributes);

  /* creation of ReceiveCANTask */
  ReceiveCANTaskHandle = osThreadNew(ReceiveCANFunc, NULL, &ReceiveCANTask_attributes);

  /* creation of ShiftLightTask */
  ShiftLightTaskHandle = osThreadNew(ShiftLightFunc, NULL, &ShiftLightTask_attributes);

  /* creation of UIButtonUpdate */
  UIButtonUpdateHandle = osThreadNew(UIButtonUpdateFunc, NULL, &UIButtonUpdate_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_HOST */
  MX_USB_HOST_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  //if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
  {
    /* Start Error */
 //   Error_Handler();
  }
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);



 /*  if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, &TxData[0]) != HAL_OK)
    {

      Error_Handler();
    }

    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, &TxData[8]) != HAL_OK)
    {

      Error_Handler();
    }*/
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_ButtonTaskFunc */
/**
* @brief Function implementing the ButtonTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ButtonTaskFunc */
void ButtonTaskFunc(void *argument)
{
  /* USER CODE BEGIN ButtonTaskFunc */
  /* Infinite loop */
  for(;;)
  {
	// Poll the built-in button for its current state
	GPIO_PinState val = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6);
	osMessageQueuePut(ButtonQueueHandle, &val, 0, 0); // lowest priority, no timeout (no waiting)
    osDelay(100);
  }
  /* USER CODE END ButtonTaskFunc */
}

/* USER CODE BEGIN Header_PollADCFunc */
/**
* @brief Function implementing the PollADC thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_PollADCFunc */
void PollADCFunc(void *argument)
{
  /* USER CODE BEGIN PollADCFunc */
  /* Infinite loop */
  for(;;)
  {
	HAL_ADC_PollForConversion(&hadc1, 10);
	uint16_t analogVal = HAL_ADC_GetValue(&hadc1);
	CR_set_brightness(&shift_light_handle, analogVal/65535.0);
	osMessageQueuePut(ADCQueueHandle, &analogVal, 0, 0);
    osDelay(20);
  }
  /* USER CODE END PollADCFunc */
}

/* USER CODE BEGIN Header_ReceiveCANFunc */
/**
* @brief Function implementing the ReceiveCANTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ReceiveCANFunc */
void ReceiveCANFunc(void *argument)
{
  /* USER CODE BEGIN ReceiveCANFunc */
  /* Infinite loop */
  for(;;)
  {
	FDCAN_RxHeaderTypeDef CAN_RX_info_handle;
	CR_parse_CAN(&latest_CAN_Vals, &hfdcan1, FDCAN_RX_FIFO0, &CAN_RX_info_handle);
    osDelay(1);
  }
  /* USER CODE END ReceiveCANFunc */
}

/* USER CODE BEGIN Header_ShiftLightFunc */
/**
* @brief Function implementing the ShiftLightTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ShiftLightFunc */
void ShiftLightFunc(void *argument)
{
  /* USER CODE BEGIN ShiftLightFunc */
  /* Infinite loop */
  for(;;)
  {
	uint16_t rpm = latest_CAN_Vals.engine_speed;

	uint16_t lightnum = (uint16_t)(((float)rpm / 13000) * shift_light_handle.num_leds);

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;

	if (rpm > 10000)
	{
		r = 255;
		g = 0;
		b = 0;
	}
	else if (rpm > 8000)
	{
		r = 255;
		g = 155;
		b = 0;
	}
	else
	{
		r = 0;
		g = 255;
		b = 0;
	}

	CR_clear_all_lights(&shift_light_handle);
	for(int i = 0; i < shift_light_handle.num_leds; i++){
		if (i < lightnum)
			CR_set_light(&shift_light_handle, i, r, g, b); //green, blue, red
		else
			CR_set_light(&shift_light_handle, i, 0, 0, 0);
	}

	osDelay(20);
  }
  /* USER CODE END ShiftLightFunc */
}

/* USER CODE BEGIN Header_UIButtonUpdateFunc */
/**
* @brief Function implementing the UIButtonUpdate thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_UIButtonUpdateFunc */
void UIButtonUpdateFunc(void *argument)
{
  /* USER CODE BEGIN UIButtonUpdateFunc */
  /* Infinite loop */
  for(;;)
  {
	  CR_check_encoder(&encoder_UI_handle);
	  osDelay(5);
  }
  /* USER CODE END UIButtonUpdateFunc */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

