/*
*********************************************************************************************************
*
*	模块名称 : AD9833 驱动模块(单通道带16位ADC)
*	文件名称 : bsp_AD9833.c
*	版    本 : V1.0
*	说    明 : AD9833模块和CPU之间采用SPI接口。本驱动程序支持硬件SPI接口和软件SPI接口。
*			  通过宏切换。
*
*	修改记录 :
*		版本号  日期         作者     说明
*		V1.0   2015-07-28  armfly  正式发布
*
*	Copyright (C), 2015, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

#define SOFT_SPI		/* 定义此行表示使用GPIO模拟SPI接口 */
//#define HARD_SPI		/* 定义此行表示使用CPU的硬件SPI接口 */

/* AD9833晶振频率 */
#define AD9833_SYSTEM_CLOCK 25000000UL 

/*
	AD9833模块可以直接插到STM32-V5开发板CN19排母(2*4P 2.54mm)接口上

   AD9833/ DAC8563模块    STM32F407开发板
	  GND   ------  GND    
	  VCC   ------  3.3V
	  
	  FSYNC  ------  PA4/NRF24L01_CSN
      	  
      SCLK ------  PA5/SPI1_SCK
      SDATA------  PA7/SPI1_MOSI
*/

/*
	AD9833基本特性:
	1、供电2.3 - 5.5V;  【本例使用3.3V】
	4、参考电压2.5V，使用内部参考

	对SPI的时钟速度要求: 高达40MHz， 速度很快.
	SCLK下降沿读取数据, 每次传送24bit数据， 高位先传
*/

#if !defined(SOFT_SPI) && !defined(HARD_SPI)
 	#error "Please define SPI Interface mode : SOFT_SPI or HARD_SPI"
#endif

#ifdef SOFT_SPI		/* 软件SPI */
	/* 定义GPIO端口 */
	#define RCC_SCLK 	RCC_APB2Periph_GPIOA
	#define PORT_SCLK	GPIOA
	#define PIN_SCLK	GPIO_Pin_5
	
	#define RCC_SDATA 	RCC_APB2Periph_GPIOA
	#define PORT_SDATA	GPIOA
	#define PIN_SDATA	GPIO_Pin_7
	
	/* 片选 */
	#define RCC_FSYNC 	RCC_APB2Periph_GPIOA
	#define PORT_FSYNC	GPIOA
	#define PIN_FSYNC	GPIO_Pin_4

	/* 定义口线置0和置1的宏 */
	#define FSYNC_0()	PORT_FSYNC->BSRRH = PIN_FSYNC
	#define FSYNC_1()	PORT_FSYNC->BSRRL = PIN_FSYNC

	#define SCLK_0()	PORT_SCLK->BSRRH = PIN_SCLK
	#define SCLK_1()	PORT_SCLK->BSRRL = PIN_SCLK

	#define SDATA_0()		PORT_SDATA->BSRRH = PIN_SDATA
	#define SDATA_1()		PORT_SDATA->BSRRL = PIN_SDATA

#endif

#ifdef HARD_SPI		/* 硬件SPI (未做) */
	;
#endif

//#define BSRRL (GPIO_BSRR_BS0 | GPIO_BSRR_BS1 | GPIO_BSRR_BS2 | GPIO_BSRR_BS3 |GPIO_BSRR_BS4 | GPIO_BSRR_BS5 | GPIO_BSRR_BS6 | GPIO_BSRR_BS7 | GPIO_BSRR_BS8 |\
GPIO_BSRR_BS9 | GPIO_BSRR_BS10 | GPIO_BSRR_BS11 | GPIO_BSRR_BS12 | GPIO_BSRR_BS13 |GPIO_BSRR_BS14 |GPIO_BSRR_BS15 )
//#define BSRRH (GPIO_BSRR_BR0 | GPIO_BSRR_BR1 | GPIO_BSRR_BR2 | GPIO_BSRR_BR3 |GPIO_BSRR_BR4 | GPIO_BSRR_BR5 | GPIO_BSRR_BR6 | GPIO_BSRR_BR7 | GPIO_BSRR_BR8 |\
GPIO_BSRR_BR9 | GPIO_BSRR_BR10 | GPIO_BSRR_BR11 | GPIO_BSRR_BR12 | GPIO_BSRR_BR13 |GPIO_BSRR_BR14 |GPIO_BSRR_BR15 )
/*
*********************************************************************************************************
*	函 数 名: bsp_InitAD9833
*	功能说明: 配置STM32的GPIO和SPI接口，用于连接 AD9833
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitAD9833(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

#ifdef SOFT_SPI
	GPIO_SetBits(GPIOA,GPIO_Pin_4);	/* FSYNC = 1 */

	/* 打开GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_SCLK | RCC_SDATA | RCC_FSYNC, ENABLE);

	/* 配置几个推挽输出IO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		/* 设为输出口 */
			/* 设为推挽模式 */
		/* 上下拉电阻不使能 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO口最大速度 */

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
*	函 数 名: AD9833_Write_16Bits      
*	功能说明: 向SPI总线发送16个bit数据   发送控制字
*	形    参: _cmd : 数据
*	返 回 值: 无
*********************************************************************************************************
*/
void AD9833_Write_16Bits(uint16_t _cmd)
{
	uint8_t i;
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	
	/*　AD9833  SCLK时钟高达40M，因此可以不延迟 */
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
*	函数名：AD9833_SelectWave
*	功能说明：软件位控制

*	IOUT正弦波 ，SIGNBITOUT方波 ，写FREQREG0 ，写PHASE0
*	ad9833_write_16bit(0x2028) 一次性写FREQREG0
*	ad9833_write_16bit(0x0038) 单独改写FREQREG0的LSB
*	ad9833_write_16bit(0x1038) 单独改写FREQREG0的MSB
 
*	IOUT三角波 ，写PHASE0
*	ad9833_write_16bit(0x2002)一次性写FREQREG0
*	ad9833_write_16bit(0x0002)单独改写FREQREG0的LSB
*	ad9833_write_16bit(0x1008)单独改写FREQREG0的MSB

*   形参：_Type -- 波形类型 
*   返回值 ：无

*********************************************************************************************************
*/
void AD9833_SelectWave(uint8_t _Type) 
{
	GPIO_SetBits(GPIOA,GPIO_Pin_4);  //宏定义
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
	if(_Type == 0)
	{
		AD9833_Write_16Bits(0x2028); /*频率寄存器输出方波*/
	}
	else if(_Type == 1)
	{
		AD9833_Write_16Bits(0x2002); /*频率寄存器输出三角波*/
	}
	else if(_Type == 2)
	{
		AD9833_Write_16Bits(0x2000); /*频率寄存器输出正弦波*/
	}
	else if(_Type == 3)
	{
		AD9833_Write_16Bits(0x00C0); /*无输出*/
	}
 

}

/****************************************************************
函数名称: AD9833_SetFreq
功能: 设置频率值   
参数: _freq
freq -- 频率值 (Freq_value(value)=Freq_data(data)*FCLK/2^28) 
返回值: 无
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





/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
