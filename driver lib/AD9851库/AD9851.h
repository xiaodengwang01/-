#ifndef _AD9851_H
#define _AD9851_H
#include "stm32f4xx_hal.h" //�ĳ�??32��hal��ͷ?��
#include "main.h"
typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long int u32;
#define AD9851_CONTROL_PORT GPIOF
#define AD9851_FQUD GPIO_PIN_1 //�ĳ�??���ӵ�����
#define AD9851_WCLK GPIO_PIN_2 //�ĳ�??���ӵ�����
#define AD9851_DATA GPIO_PIN_3 //�ĳ�??���ӵ�����
#define AD9851_RST GPIO_PIN_0 //�ĳ�??���ӵ�����
#define AD9851_WCLK_H HAL_GPIO_WritePin(AD9851_CONTROL_PORT, AD9851_WCLK, GPIO_PIN_SET)
#define AD9851_WCLK_L HAL_GPIO_WritePin(AD9851_CONTROL_PORT, AD9851_WCLK, GPIO_PIN_RESET)
#define AD9851_FQUD_H HAL_GPIO_WritePin(AD9851_CONTROL_PORT, AD9851_FQUD, GPIO_PIN_SET)
#define AD9851_FQUD_L HAL_GPIO_WritePin(AD9851_CONTROL_PORT, AD9851_FQUD, GPIO_PIN_RESET)
#define AD9851_DATA_H HAL_GPIO_WritePin(AD9851_CONTROL_PORT, AD9851_DATA, GPIO_PIN_SET)
#define AD9851_DATA_L HAL_GPIO_WritePin(AD9851_CONTROL_PORT, AD9851_DATA, GPIO_PIN_RESET)
#define AD9851_RESET_H HAL_GPIO_WritePin(AD9851_CONTROL_PORT, AD9851_RST, GPIO_PIN_SET)
#define AD9851_RESET_L HAL_GPIO_WritePin(AD9851_CONTROL_PORT, AD9851_RST, GPIO_PIN_RESET)

void AD9851_Delay(unsigned int z);
void AD9851_Write(unsigned char w0,double frequence);
//void AD9851_Write(u8 w0,double frequence);
void AD9851_Init(void);
#endif
