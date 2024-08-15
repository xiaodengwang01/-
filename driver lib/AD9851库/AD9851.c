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
//ad9851.C
#include "ad9851.h"
#include "stm32f4xx_hal.h" //��Ϊ??32��hal��ͷ?��
typedef unsigned int u16;
typedef unsigned char u8;
void AD9851_Init(void)
{
AD9851_WCLK_L;
AD9851_FQUD_L;
AD9851_RESET_L;
AD9851_RESET_H;
AD9851_Delay(0xffff);
AD9851_RESET_L;
AD9851_WCLK_L;
AD9851_WCLK_H;
AD9851_Delay(0xffff);
AD9851_WCLK_L;
AD9851_FQUD_L;
AD9851_FQUD_H;
AD9851_Delay(0xffff);
AD9851_FQUD_L;
}

void AD9851_Write(u8 w0,double frequence)
{
unsigned char i;
long int y;
double x;
u8 wdata;
x=4294967295U/180;//�ʺ�125M����
//���ʱ��Ƶ�ʲ�Ϊ180MHZ���޸ĸô���Ƶ��ֵ����λMHz ������
frequence=frequence/1000000;
frequence=frequencex;
y=frequence;
//дw4����
wdata=(y>>=0);
for(i=0;i<8;i++)
{
if(wdata&0x01)
AD9851_DATA_H;
else
AD9851_DATA_L;
AD9851_WCLK_H;
wdata=wdata>>1;
AD9851_WCLK_L;
}
wdata=(y>>8);
for(i=0;i<8;i++)
{
if(wdata&0x01)
AD9851_DATA_H;
else
AD9851_DATA_L;
AD9851_WCLK_H;
wdata=wdata>>1;
AD9851_WCLK_L;
}
wdata=y>>16;
for(i=0;i<8;i++)
{
if(wdata&0x01)
AD9851_DATA_H;
else
AD9851_DATA_L;
AD9851_WCLK_H;
wdata=wdata>>1;
AD9851_WCLK_L;
}
wdata=y>>24;
for(i=0;i<8;i++)
{
if(wdata&0x01)

AD9851_DATA_H;
else
AD9851_DATA_L;
AD9851_WCLK_H;
wdata=wdata>>1;
AD9851_WCLK_L;
}
wdata=w0;
for(i=0;i<8;i++)
{
if(wdata&0x01)
AD9851_DATA_H;
else
AD9851_DATA_L;
AD9851_WCLK_H;
wdata=wdata>>1;
AD9851_WCLK_L;
}
AD9851_FQUD_H;
// AD9851_Delay(0xffff);
AD9851_FQUD_L;
}
void AD9851_Delay(unsigned int z)
{
for(;z>0;z--)
{;}
}
