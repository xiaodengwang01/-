/**
 * @file    softspi.c
 * @author  Myth
 * @version 0.2
 * @date    2021.10.12
 * @brief   STM32 SoftSPI Library
 */

#include "softspi.h"

SoftSPI_TypeDef SoftSPI1;

void softSPI_Init(SoftSPI_TypeDef *SoftSPIx, uint8_t CPOL, uint8_t CPHA)
{
    DWT_Delay_Init();
    SoftSPIx->Delay_Time = SPI_DELAY_TIME_US;
    SoftSPIx->SCLK_GPIO = SPI_SCK_GPIO_Port;
    SoftSPIx->SCLK_Pin = SPI_SCK_Pin;
    SoftSPIx->MOSI_GPIO = SPI_MOSI_GPIO_Port;
    SoftSPIx->MOSI_Pin = SPI_MOSI_Pin;
    SoftSPIx->MISO_GPIO = SPI_MISO_GPIO_Port;
    SoftSPIx->MISO_Pin = SPI_MISO_Pin;
    SoftSPIx->CS_GPIO = SPI_CS_GPIO_Port;
    SoftSPIx->CS_Pin = SPI_CS_Pin;
    SoftSPIx->CPOL = CPOL;
    SoftSPIx->CPHA = CPHA;
    CS_Set;
    MOSI_Set;
    SCLK_Clr;
}

// SPI的读写操作，可以读写多个字节
void SoftSPI_WriteReadBuff(SoftSPI_TypeDef *SoftSPIx, uint8_t *pWrite, uint8_t *pRead, uint32_t len)
{
    uint8_t data;
    uint8_t byte;
    uint8_t i, j;

    for (j = 0; j < len; j++)
    {
        data = 0;
        byte = pWrite[j];

        if (SoftSPIx->CPHA == 0)
        {
            for (i = 0; i < 8; i++)
            {
                if (byte & 0x80)
                    MOSI_Set;
                else
                    MOSI_Clr;

                Delay;

                byte <<= 1;
                SCLK_Set;

                Delay;

                data <<= 1;
                if (MISO_Read == GPIO_PIN_SET)
                    data |= 0x01;

                Delay;

                SCLK_Clr;

                Delay;
            }
        }
        else
        {
            for (i = 0; i < 8; i++)
            {
                SCLK_Set;

                Delay;

                if (byte & 0x80)
                    MOSI_Set;
                else
                    MOSI_Clr;

                Delay;

                byte <<= 1;
                SCLK_Clr;

                Delay;

                data <<= 1;
                if (MISO_Read == GPIO_PIN_SET)
                    data |= 0x01;

                Delay;
            }
        }

        pRead[j] = data;
    }
}

// 设置CS为高电平
void SoftSPI_SetCS(SoftSPI_TypeDef *SoftSPIx)
{
    CS_Set;
}

// 设置CS为低电平
void SoftSPI_ClrCS(SoftSPI_TypeDef *SoftSPIx)
{
    CS_Clr;
}