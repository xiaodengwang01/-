/*
*********************************************************************************************************
*
*	ģ������ : AD9833 ����ģ��(��ͨ����16λADC)
*	�ļ����� : bsp_AD9833.c
*	��    �� : V1.0
*	˵    �� : AD9833ģ���CPU֮�����SPI�ӿڡ�����������֧��Ӳ��SPI�ӿں����SPI�ӿڡ�
*			  ͨ�����л���
*
*	�޸ļ�¼ :
*		�汾��  ����         ����     ˵��
*		V1.0   2015-07-28  armfly  ��ʽ����
*
*	Copyright (C), 2015, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

#define SOFT_SPI		/* ������б�ʾʹ��GPIOģ��SPI�ӿ� */
//#define HARD_SPI		/* ������б�ʾʹ��CPU��Ӳ��SPI�ӿ� */

/* AD9833����Ƶ�� */
#define AD9833_SYSTEM_CLOCK 25000000UL 

/*
	AD9833ģ�����ֱ�Ӳ嵽STM32-V5������CN19��ĸ(2*4P 2.54mm)�ӿ���

   AD9833/ DAC8563ģ��    STM32F407������
	  GND   ------  GND    
	  VCC   ------  3.3V
	  
	  FSYNC  ------  PA4/NRF24L01_CSN
      	  
      SCLK ------  PA5/SPI1_SCK
      SDATA------  PA7/SPI1_MOSI
*/

/*
	AD9833��������:
	1������2.3 - 5.5V;  ������ʹ��3.3V��
	4���ο���ѹ2.5V��ʹ���ڲ��ο�

	��SPI��ʱ���ٶ�Ҫ��: �ߴ�40MHz�� �ٶȺܿ�.
	SCLK�½��ض�ȡ����, ÿ�δ���24bit���ݣ� ��λ�ȴ�
*/

#if !defined(SOFT_SPI) && !defined(HARD_SPI)
 	#error "Please define SPI Interface mode : SOFT_SPI or HARD_SPI"
#endif

#ifdef SOFT_SPI		/* ���SPI */
	/* ����GPIO�˿� */
	#define RCC_SCLK 	RCC_APB2Periph_GPIOA
	#define PORT_SCLK	GPIOA
	#define PIN_SCLK	GPIO_Pin_5
	
	#define RCC_SDATA 	RCC_APB2Periph_GPIOA
	#define PORT_SDATA	GPIOA
	#define PIN_SDATA	GPIO_Pin_7
	
	/* Ƭѡ */
	#define RCC_FSYNC 	RCC_APB2Periph_GPIOA
	#define PORT_FSYNC	GPIOA
	#define PIN_FSYNC	GPIO_Pin_4

	/* ���������0����1�ĺ� */
	#define FSYNC_0()	PORT_FSYNC->BSRRH = PIN_FSYNC
	#define FSYNC_1()	PORT_FSYNC->BSRRL = PIN_FSYNC

	#define SCLK_0()	PORT_SCLK->BSRRH = PIN_SCLK
	#define SCLK_1()	PORT_SCLK->BSRRL = PIN_SCLK

	#define SDATA_0()		PORT_SDATA->BSRRH = PIN_SDATA
	#define SDATA_1()		PORT_SDATA->BSRRL = PIN_SDATA

#endif

#ifdef HARD_SPI		/* Ӳ��SPI (δ��) */
	;
#endif

//#define BSRRL (GPIO_BSRR_BS0 | GPIO_BSRR_BS1 | GPIO_BSRR_BS2 | GPIO_BSRR_BS3 |GPIO_BSRR_BS4 | GPIO_BSRR_BS5 | GPIO_BSRR_BS6 | GPIO_BSRR_BS7 | GPIO_BSRR_BS8 |\
GPIO_BSRR_BS9 | GPIO_BSRR_BS10 | GPIO_BSRR_BS11 | GPIO_BSRR_BS12 | GPIO_BSRR_BS13 |GPIO_BSRR_BS14 |GPIO_BSRR_BS15 )
//#define BSRRH (GPIO_BSRR_BR0 | GPIO_BSRR_BR1 | GPIO_BSRR_BR2 | GPIO_BSRR_BR3 |GPIO_BSRR_BR4 | GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR7 | GPIO_BSRR_BR8 |\
GPIO_BSRR_BR9 | GPIO_BSRR_BR10 | GPIO_BSRR_BR11 | GPIO_BSRR_BR12 | GPIO_BSRR_BR13 |GPIO_BSRR_BR14 |GPIO_BSRR_BR15 )
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitAD9833
*	����˵��: ����STM32��GPIO��SPI�ӿڣ��������� AD9833
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitAD9833(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

#ifdef SOFT_SPI
	GPIO_SetBits(GPIOA,GPIO_Pin_4);	/* FSYNC = 1 */

	/* ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_SCLK | RCC_SDATA | RCC_FSYNC, ENABLE);

	/* ���ü����������IO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		/* ��Ϊ����� */
			/* ��Ϊ����ģʽ */
		/* ���������費ʹ�� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO������ٶ� */

	GPIO_InitStructure.GPIO_Pin = PIN_SCLK;
	GPIO_Init(PORT_SCLK, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_SDATA;
	GPIO_Init(PORT_SDATA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_FSYNC;
	GPIO_Init(PORT_FSYNC, &GPIO_InitStructure);
#endif

}

/*
*********************************************************************************************************
*	�� �� ��: AD9833_Write_16Bits      
*	����˵��: ��SPI���߷���16��bit����   ���Ϳ�����
*	��    ��: _cmd : ����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void AD9833_Write_16Bits(uint16_t _cmd)
{
	uint8_t i;
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	
	/*��AD9833  SCLKʱ�Ӹߴ�40M����˿��Բ��ӳ� */
	for(i = 0; i < 16; i++)
	{
		if (_cmd & 0x8000)
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_7);

		}
		else
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_7);
		}
		GPIO_ResetBits(GPIOA,GPIO_Pin_5);
		_cmd <<= 1;
		GPIO_SetBits(GPIOA,GPIO_Pin_5);
	}
	
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}

/*
*********************************************************************************************************
*	��������AD9833_SelectWave
*	����˵�������λ����

*	IOUT���Ҳ� ��SIGNBITOUT���� ��дFREQREG0 ��дPHASE0
*	ad9833_write_16bit(0x2028) һ����дFREQREG0
*	ad9833_write_16bit(0x0038) ������дFREQREG0��LSB
*	ad9833_write_16bit(0x1038) ������дFREQREG0��MSB
 
*	IOUT���ǲ� ��дPHASE0
*	ad9833_write_16bit(0x2002)һ����дFREQREG0
*	ad9833_write_16bit(0x0002)������дFREQREG0��LSB
*	ad9833_write_16bit(0x1008)������дFREQREG0��MSB

*   �βΣ�_Type -- �������� 
*   ����ֵ ����

*********************************************************************************************************
*/
void AD9833_SelectWave(uint8_t _Type) 
{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);  //�궨��
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
	if(_Type == 0)
	{
		AD9833_Write_16Bits(0x2028); /*Ƶ�ʼĴ����������*/
	}
	else if(_Type == 1)
	{
		AD9833_Write_16Bits(0x2002); /*Ƶ�ʼĴ���������ǲ�*/
	}
	else if(_Type == 2)
	{
		AD9833_Write_16Bits(0x2000); /*Ƶ�ʼĴ���������Ҳ�*/
	}
	else if(_Type == 3)
	{
		AD9833_Write_16Bits(0x00C0); /*�����*/
	}
 

}

/****************************************************************
��������: AD9833_SetFreq
����: ����Ƶ��ֵ   
����: _freq
freq -- Ƶ��ֵ (Freq_value(value)=Freq_data(data)*FCLK/2^28) 
����ֵ: ��
*****************************************************************/
void AD9833_SetFreq(uint32_t _freq)
{
	uint32_t freq;
	uint16_t lsb_14bit;
	uint16_t msb_14bit;
	uint8_t freq_number = 0;

	freq = (uint32_t)(268435456.0 / AD9833_SYSTEM_CLOCK * _freq);
	lsb_14bit = (uint16_t)freq;
	msb_14bit = (uint16_t)(freq >> 14);
	if(freq_number == FREQ_0)
	{
		lsb_14bit &= ~(1U<<15);
		lsb_14bit |= 1<<14;
		msb_14bit &= ~(1U<<15); 
		msb_14bit |= 1<<14;
	}
	else
	{
		lsb_14bit &= ~(1<<14);
		lsb_14bit |= 1U<<15;  
		msb_14bit &= ~(1<<14);
		msb_14bit |= 1U<<15;
	}

	AD9833_Write_16Bits(lsb_14bit);
	AD9833_Write_16Bits(msb_14bit);
}





/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
