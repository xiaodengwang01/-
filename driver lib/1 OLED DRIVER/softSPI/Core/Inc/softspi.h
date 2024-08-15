/**
 * @file    softspi.h
 * @author  Myth
 * @version 0.2
 * @date    2021.10.12
 * @brief   STM32 SoftSPI Library
 */

#ifndef __SOFTSPI_H
#define __SOFTSPI_H

#include "main.h"
#include "dwt_stm32_delay.h"
#include "gpio.h"

#define SPI_DELAY_TIME_US 50 // SPI频率为20KHz

/**
 * @brief  SoftSPI Structure definition
 */
typedef struct
{
    GPIO_TypeDef *SCLK_GPIO;
    uint32_t SCLK_Pin;

    GPIO_TypeDef *MOSI_GPIO;
    uint32_t MOSI_Pin;

    GPIO_TypeDef *MISO_GPIO;
    uint32_t MISO_Pin;

    GPIO_TypeDef *CS_GPIO;
    uint32_t CS_Pin;

    uint32_t Delay_Time;

    uint8_t CPOL;
    uint8_t CPHA;
} SoftSPI_TypeDef;

#define SCLK_Set HAL_GPIO_WritePin(SoftSPIx->SCLK_GPIO, SoftSPIx->SCLK_Pin, SoftSPIx->CPOL ? GPIO_PIN_RESET : GPIO_PIN_SET)
#define SCLK_Clr HAL_GPIO_WritePin(SoftSPIx->SCLK_GPIO, SoftSPIx->SCLK_Pin, SoftSPIx->CPOL ? GPIO_PIN_SET : GPIO_PIN_RESET)

#define MOSI_Set HAL_GPIO_WritePin(SoftSPIx->MOSI_GPIO, SoftSPIx->MOSI_Pin, GPIO_PIN_SET)
#define MOSI_Clr HAL_GPIO_WritePin(SoftSPIx->MOSI_GPIO, SoftSPIx->MOSI_Pin, GPIO_PIN_RESET)

#define MISO_Read HAL_GPIO_ReadPin(SoftSPIx->MISO_GPIO, SoftSPIx->MISO_Pin)

#define CS_Set HAL_GPIO_WritePin(SoftSPIx->CS_GPIO, SoftSPIx->CS_Pin, GPIO_PIN_SET)
#define CS_Clr HAL_GPIO_WritePin(SoftSPIx->CS_GPIO, SoftSPIx->CS_Pin, GPIO_PIN_RESET)

#define Delay DWT_Delay_us(SoftSPIx->Delay_Time)

extern SoftSPI_TypeDef SoftSPI1;

void softSPI_Init(SoftSPI_TypeDef *SoftSPIx, uint8_t CPOL, uint8_t CPHA);

void SoftSPI_SetCS(SoftSPI_TypeDef *SoftSPIx);
void SoftSPI_ClrCS(SoftSPI_TypeDef *SoftSPIx);

uint8_t SoftSPI_WriteRead(SoftSPI_TypeDef *SoftSPIx, uint8_t byte);

void SoftSPI_WriteReadBuff(SoftSPI_TypeDef *SoftSPIx, uint8_t *pWrite, uint8_t *pRead, uint32_t len);

#endif
