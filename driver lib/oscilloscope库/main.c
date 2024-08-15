/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "font.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define CLOCKTIM_PRESC 0
#define SYSCLK_FREQ 72000000
#define BUFFER_LEN 200
#define RISING 1
#define FALLING 0

#define UPPER_VOLTAGE 3.3
#define LOWER_VOLTAGE 0.0
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t adcBuf[BUFFER_LEN];
int atten = 1;
float vdiv = 2.0;
float trigVoltage = 0.0;
uint8_t trig = RISING, trigged;
int trigPoint;
float tdiv = 10000.0;
uint32_t sampRate;
float sampPer;
float maxVoltage, minVoltage;
float measuredFreq, sigPer;

volatile uint8_t finishedConversion = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
float adcToVoltage(uint16_t samp);
void drawWave();
void findTrigger();

void dottedHLine(int x, int y, int l);
void dottedVLine(int x, int y, int l);
void drawGraticule(int hei, int wit, int pix, int divx, int divy);
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
  MX_ADC1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(20); // 单片机启动比OLED上电快,需要延迟等待一下
  OLED_Init();
  // 初始化ADC采集
  HAL_TIM_Base_Start(&htim3);                                // Start the timebase timer
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adcBuf, BUFFER_LEN); // Start the ADC in DMA mode
  // 显示初始界面
  OLED_NewFrame();
  OLED_PrintString(0, 0, "Oscilloscope", &font16x16, OLED_COLOR_NORMAL);
  OLED_ShowFrame();
  HAL_Delay(1000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (finishedConversion) // We finished aquiring one buffer
    {
      // Find the trigger point
      findTrigger();
      if (trigged)
        HAL_GPIO_WritePin(GREEN_GPIO_Port, GREEN_Pin, 1);

      // Run the UI
      OLED_NewFrame();
      drawWave();
      OLED_ShowFrame();

      // Start again
      HAL_GPIO_WritePin(GREEN_GPIO_Port, GREEN_Pin, 0);
      finishedConversion = 0;
      HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adcBuf, BUFFER_LEN);
    }
    HAL_Delay(1000);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  finishedConversion = 1;
}
// Convert ADC value to volts
float adcToVoltage(uint16_t samp)
{
  return atten * 2 * (((3.3 * samp) / 4096.0) - 1.65);
}

// Draw the waveform on the screen
void drawWave()
{
  drawGraticule(64, 96, 16, 4, 4); // Draw the graticule

  maxVoltage = LOWER_VOLTAGE;
  minVoltage = UPPER_VOLTAGE;

  for (int i = 0; i <= 94; i++)
  {
    // If we're looping through the buffer, let's compute the minimum and maximum voltage values while we're at it
    float voltage1 = adcToVoltage(adcBuf[i + trigPoint]);
    float voltage2 = adcToVoltage(adcBuf[i + trigPoint + 1]);
    if (voltage2 > maxVoltage)
      maxVoltage = voltage2;
    if (voltage2 < minVoltage)
      minVoltage = voltage2;

    // Draw lines between sample points
    OLED_DrawLine(i, 31 - (voltage1 * 16 / vdiv), i + 1, 31 - (voltage2 * 16 / vdiv), OLED_COLOR_NORMAL);
  }
}

// This function finds the trigger point and also computes the frequency of thge signal
void findTrigger()
{
  int trigLevel = (4096.0 * (trigVoltage / (2.0 * atten) + 1.65)) / 3.3; // ADC level at which we should trigger
  int trigPoint2;                                                        // another trigger point, this will help us determine the period of the signal

  trigPoint = 0;
  trigged = 0;
  measuredFreq = 0;

  // The trigged variable will be 0 if we're not triggering, 1 if we only found 1 trigger point and 2 if we have at least two trigger points

  for (int i = 1; i < BUFFER_LEN / 2 && trigged != 2; i++) // we're looking for trigger points in the first half of the buffer
    if ((trig == RISING && adcBuf[i] >= trigLevel && adcBuf[i - 1] < trigLevel) || (trig == FALLING && adcBuf[i] <= trigLevel && adcBuf[i - 1] > trigLevel))
    {
      if (!trigged) // Looking for the first trigger point
      {
        trigPoint = i;
        trigged = 1;
      }
      else // Looking for the second one
      {
        trigPoint2 = i;
        trigged = 2;
      }
    }

  if (trigged == 2) // If we found at least two trigger points
  {
    // sigPer = sampPer * (trigPoint2 - trigPoint); // we compute the period of the signal in uS
    // measuredFreq = 1000000.0 / sigPer;           // and then we convert it into frequency, in Hz
  }
}

void dottedHLine(int x, int y, int l)
{
  uint8_t col = 1;
  for (int i = 0; i <= l; i++)
  {
    OLED_SetPixel(x + i, y, col);
    col = !col;
  }
}
void dottedVLine(int x, int y, int l)
{
  uint8_t col = 1;
  for (int i = 0; i <= l; i++)
  {
    OLED_SetPixel(x, y + i, col);
    col = !col;
  }
}
void drawGraticule(int hei, int wit, int pix, int divx, int divy)
{
  for (int i = 0; i <= wit; i += pix)
    dottedVLine(i, 0, hei);

  for (int i = 0; i <= hei + 1; i += pix)
    dottedHLine(0, i, wit);
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
