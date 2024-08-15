/*
*********************************************************************************************************
*
*	ģ������ : DAC8562/8563 ����ģ��(��ͨ����16λDAC)
*	�ļ����� : bsp_dac8562.c
*	��    �� : V1.0
*	˵    �� : DAC8562/8563ģ���CPU֮�����SPI�ӿڡ�����������֧��Ӳ��SPI�ӿں����SPI�ӿڡ�
*			  ͨ�����л���
*
*	�޸ļ�¼ :
*		�汾��  ����         ����     ˵��
*		V1.0    2014-01-17  armfly  ��ʽ����
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

#define SOFT_SPI		/* ������б�ʾʹ��GPIOģ��SPI�ӿ� */
//#define HARD_SPI		/* ������б�ʾʹ��CPU��Ӳ��SPI�ӿ� */

/*
	DAC8501ģ�����ֱ�Ӳ嵽STM32-V5������CN19��ĸ(2*4P 2.54mm)�ӿ���

   AD9833/ DAC8563ģ��    STM32F407������
	  GND   ------  GND    
	  VCC   ------  3.3V
	  
			FSYNC/SYNC  ------  PF7/NRF24L01_CSN
      	  
      SCLK/SCLK  ------  PB3/SPI3_SCK
      MOSI/DIN   ------  PB5/SPI3_MOSI

			------  PB4/SPI3_MISO
 	  CLR   ------  PH7/NRF24L01_IRQ
*/

/*
	AD9833��������:
	1������2.3 - 5.5V;  ������ʹ��3.3V��
	4���ο���ѹ2.5V��ʹ���ڲ��ο�

	��SPI��ʱ���ٶ�Ҫ��: �ߴ�40MHz�� �ٶȺܿ�.
	SCLK�½��ض�ȡ����, ÿ�δ���24bit���ݣ� ��λ�ȴ�?�/???
*/

#if !defined(SOFT_SPI) && !defined(HARD_SPI)
 	#error "Please define SPI Interface mode : SOFT_SPI or HARD_SPI"
#endif

#ifdef SOFT_SPI		/* ���SPI */
	/* ����GPIO�˿� */
	#define RCC_SCLK 	RCC_AHB1Periph_GPIOB
	#define PORT_SCLK	GPIOB
	#define PIN_SCLK	GPIO_Pin_3
	
	#define RCC_DIN 	RCC_AHB1Periph_GPIOB
	#define PORT_DIN	GPIOB
	#define PIN_DIN	GPIO_Pin_5
	
	/* Ƭѡ */
	#define RCC_SYNC 	RCC_AHB1Periph_GPIOF
	#define PORT_SYNC	GPIOF
	#define PIN_SYNC	GPIO_Pin_7

	/* ���������0����1�ĺ� */
	#define SYNC_0()	PORT_SYNC->BSRRH = PIN_SYNC
	#define SYNC_1()	PORT_SYNC->BSRRL = PIN_SYNC

	#define SCLK_0()	PORT_SCLK->BSRRH = PIN_SCLK
	#define SCLK_1()	PORT_SCLK->BSRRL = PIN_SCLK

	#define DIN_0()		PORT_DIN->BSRRH = PIN_DIN
	#define DIN_1()		PORT_DIN->BSRRL = PIN_DIN

#endif

#ifdef HARD_SPI		/* Ӳ��SPI (δ��) */
	;
#endif

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitAD9833
*	����˵��: ����STM32��GPIO��SPI�ӿڣ��������� AD9833
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitDAC8562(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

#ifdef SOFT_SPI
	SYNC_1();	/* SYNC = 1 */

	/* ��GPIOʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_SCLK | RCC_DIN | RCC_SYNC, ENABLE);

	/* ���ü����������IO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* ���������費ʹ�� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO������ٶ� */

	GPIO_InitStructure.GPIO_Pin = PIN_SCLK;
	GPIO_Init(PORT_SCLK, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_DIN;
	GPIO_Init(PORT_DIN, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_SYNC;
	GPIO_Init(PORT_SYNC, &GPIO_InitStructure);
#endif

	/* Power up DAC-A and DAC-B */
	DAC8562_WriteCmd((4 << 19) | (0 << 16) | (3 << 0));
	
	/* LDAC pin inactive for DAC-B and DAC-A  ��ʹ��LDAC���Ÿ������� */
	DAC8562_WriteCmd((6 << 19) | (0 << 16) | (3 << 0));

	/* ��λ2��DAC���м�ֵ, ���2.5V */
	DAC8562_SetData(0, 32767);
	DAC8562_SetData(1, 32767);

	/* ѡ���ڲ��ο�����λ2��DAC������=2 ����λʱ���ڲ��ο��ǽ�ֹ��) */
	DAC8562_WriteCmd((7 << 19) | (0 << 16) | (1 << 0));
}

/*
*********************************************************************************************************
*	�� �� ��: AD9833_WriteCmd
*	����˵��: ��SPI���߷���16��bit����
*	��    ��: _cmd : ����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DAC8562_WriteCmd(uint32_t _cmd)
{
	uint8_t i;
	
	FSYNC_0();
	
	/*��DAC8562 SCLKʱ�Ӹߴ�50M����˿��Բ��ӳ� */
	for(i = 0; i < 16; i++)
	{
		if (_cmd & 0x800000)
		{
			DIN_1();
		}
		else
		{
			DIN_0();
		}
		SCLK_1();
		_cmd <<= 1;
		SCLK_0();
	}
	
	FSYNC_1();
}

/*
*********************************************************************************************************
*��������AD9833_Select_Wave
*����˵����
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*	�� �� ��: DAC8562_SetData
*	����˵��: ����DAC��������������¡�
*	��    ��: _ch, ͨ��, 0 , 1
*		     _data : ����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DAC8562_SetData(uint8_t _ch, uint16_t _dac)
{
	if (_ch == 0)
	{
		/* Write to DAC-A input register and update DAC-A; */
		DAC8562_WriteCmd((3 << 19) | (0 << 16) | (_dac << 0));
	}
	else if (_ch == 1)
	{
		/* Write to DAC-B input register and update DAC-A; */
		DAC8562_WriteCmd((3 << 19) | (1 << 16) | (_dac << 0));		
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
