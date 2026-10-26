/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <xprintf.h>
#include "../../usercodes/sac/sac.h"
#include "../../usercodes/app/app.h"
#include "../../usercodes/service/i2cmn/i2c_manager.h"
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
volatile uint16_t   ADC1_DMA_data[2];
volatile uint16_t   ADC2_DMA_data[6];
volatile uint16_t   timerIntrDevCnt;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/**
 * @brief 500usタイマーハンドラA
 * @details タイマー割り込み内で実施
 */
void timerInterruptHandler_500us(void);

/**
 * @brief 1msタイマーハンドラA
 * @details タイマー割り込み内で実施
 */
void timerInterruptHandler_1msA(void);
/**
 * @brief 1msタイマーハンドラB
 * @details タイマー割り込み内で実施
 */
void timerInterruptHandler_1msB(void);

/**
 * @brief 10msタイマーハンドラA
 * @details タイマー割り込み内で実施
 */
void timerInterruptHandler_10msA(void);
/**
 * @brief 10msタイマーハンドラB
 * @details タイマー割り込み内で実施
 */
void timerInterruptHandler_10msB(void);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void _x_putchar(int ch) {
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1 , 0xFFFF );
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
    if (htim == &htim7) {

        timerInterruptHandler_500us();

        if (timerIntrDevCnt % 2) {
            timerInterruptHandler_1msA();
        }
        else {
            timerInterruptHandler_1msB();
        }

        if (timerIntrDevCnt == 0) {
            timerInterruptHandler_10msA();
        }
        else if (timerIntrDevCnt == 10) {
            timerInterruptHandler_10msB();
        }

        if (timerIntrDevCnt == 19) {
            timerIntrDevCnt = 0;
        }
        else {
            timerIntrDevCnt++;
        }
    }
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
    int cnt = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
      HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
      for (cnt = 0; cnt < 8; cnt++) {
      __HAL_TIM_SET_AUTORELOAD(&htim3, (uint16_t)(4545 / ((cnt + 1))) );
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (uint16_t)(4545 / ((cnt + 1) * 2)));
      __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
      HAL_Delay(64 - (cnt * 4));
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (uint16_t)(0));
      HAL_Delay(128 - (cnt * 8));
    }
    xdev_out(_x_putchar);
  
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC1_DMA_data, 2);
  HAL_ADC_Start_DMA(&hadc2, (uint32_t *)ADC2_DMA_data, 6);

  HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);

  HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);

  i2cmanagerInit();
  /// SAC INIT
  uiswInit(&ADC2_DMA_data[5]);
  buzzerInit();
  motorsInit();
  linesensorsInit(&ADC2_DMA_data[3], &ADC2_DMA_data[2], &ADC2_DMA_data[1], &ADC2_DMA_data[0]);
  markersensorsInit(&ADC1_DMA_data[0], &ADC2_DMA_data[4]);
  batteryInit(&ADC1_DMA_data[1]);
  timerIntrDevCnt = 0;
  HAL_TIM_Base_Start(&htim6);
  __HAL_TIM_SET_COUNTER(&htim7, 127);
  appInit();
  HAL_TIM_Base_Start_IT(&htim7);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    sacLoop();
    i2cmanagerMainloop();
    appMainLoop();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_Delay(10);
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void timerInterruptHandler_500us(void) {

}

void timerInterruptHandler_1msA(void) {
    linesensorsMeasure_1ms();
    markersensorsMeasure_1ms();
    i2cmanagerControl_1ms_A();
    imuMeasure_1ms();
}

void timerInterruptHandler_1msB(void) {
    i2cmanagerControl_1ms_B();
    appIntervalHandler_1ms();
    motorsControl_1ms();
}

void timerInterruptHandler_10msA(void) {
    uisw_10ms();
    buzzer_10ms();
}

void timerInterruptHandler_10msB(void) {
    battery_10ms();
    appIntervalHandler_10ms();
}
/* USER CODE END 4 */

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
