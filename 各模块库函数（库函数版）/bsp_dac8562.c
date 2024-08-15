/*
*********************************************************************************************************
*
*	Ä£¿éÃû³Æ : DAC8562/8563 Çý¶¯Ä£¿é(µ¥Í¨µÀ´ø16Î»DAC)
*	ÎÄ¼þÃû³Æ : bsp_dac8562.c
*	°æ    ±¾ : V1.0
*	Ëµ    Ã÷ : DAC8562/8563Ä£¿éºÍCPUÖ®¼ä²ÉÓÃSPI½Ó¿Ú¡£±¾Çý¶¯³ÌÐòÖ§³ÖÓ²¼þSPI½Ó¿ÚºÍÈí¼þSPI½Ó¿Ú¡£
*			  Í¨¹ýºêÇÐ»»¡£
*
*	ÐÞ¸Ä¼ÇÂ¼ :
*		°æ±¾ºÅ  ÈÕÆÚ         ×÷Õß     ËµÃ÷
*		V1.0    2014-01-17  armfly  ÕýÊ½·¢²¼
*
*	Copyright (C), 2013-2014, °²¸»À³µç×Ó www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

#define SOFT_SPI		/* ¶¨Òå´ËÐÐ±íÊ¾Ê¹ÓÃGPIOÄ£ÄâSPI½Ó¿Ú */
//#define HARD_SPI		/* ¶¨Òå´ËÐÐ±íÊ¾Ê¹ÓÃCPUµÄÓ²¼þSPI½Ó¿Ú */

/*
	DAC8501Ä£¿é¿ÉÒÔÖ±½Ó²åµ½STM32-V5¿ª·¢°åCN19ÅÅÄ¸(2*4P 2.54mm)½Ó¿ÚÉÏ

   AD9833/ DAC8563Ä£¿é    STM32F407¿ª·¢°å
	  GND   ------  GND    
	  VCC   ------  3.3V
	  
			FSYNC/SYNC  ------  PF7/NRF24L01_CSN
      	  
      SCLK/SCLK  ------  PB3/SPI3_SCK
      MOSI/DIN   ------  PB5/SPI3_MOSI

			------  PB4/SPI3_MISO
 	  CLR   ------  PH7/NRF24L01_IRQ
*/

/*
	AD9833»ù±¾ÌØÐÔ:
	1¡¢¹©µç2.3 - 5.5V;  ¡¾±¾ÀýÊ¹ÓÃ3.3V¡¿
	4¡¢²Î¿¼µçÑ¹2.5V£¬Ê¹ÓÃÄÚ²¿²Î¿¼

	¶ÔSPIµÄÊ±ÖÓËÙ¶ÈÒªÇó: ¸ß´ï40MHz£¬ ËÙ¶ÈºÜ¿ì.
	SCLKÏÂ½µÑØ¶ÁÈ¡Êý¾Ý, Ã¿´Î´«ËÍ24bitÊý¾Ý£¬ ¸ßÎ»ÏÈ´«?¡/???
*/

#if !defined(SOFT_SPI) && !defined(HARD_SPI)
 	#error "Please define SPI Interface mode : SOFT_SPI or HARD_SPI"
#endif

#ifdef SOFT_SPI		/* Èí¼þSPI */
	/* ¶¨ÒåGPIO¶Ë¿Ú */
	#define RCC_SCLK 	RCC_AHB1Periph_GPIOB
	#define PORT_SCLK	GPIOB
	#define PIN_SCLK	GPIO_Pin_3
	
	#define RCC_DIN 	RCC_AHB1Periph_GPIOB
	#define PORT_DIN	GPIOB
	#define PIN_DIN	GPIO_Pin_5
	
	/* Æ¬Ñ¡ */
	#define RCC_SYNC 	RCC_AHB1Periph_GPIOF
	#define PORT_SYNC	GPIOF
	#define PIN_SYNC	GPIO_Pin_7

	/* ¶¨Òå¿ÚÏßÖÃ0ºÍÖÃ1µÄºê */
	#define SYNC_0()	PORT_SYNC->BSRRH = PIN_SYNC
	#define SYNC_1()	PORT_SYNC->BSRRL = PIN_SYNC

	#define SCLK_0()	PORT_SCLK->BSRRH = PIN_SCLK
	#define SCLK_1()	PORT_SCLK->BSRRL = PIN_SCLK

	#define DIN_0()		PORT_DIN->BSRRH = PIN_DIN
	#define DIN_1()		PORT_DIN->BSRRL = PIN_DIN

#endif

#ifdef HARD_SPI		/* Ó²¼þSPI (Î´×ö) */
	;
#endif

/*
*********************************************************************************************************
*	º¯ Êý Ãû: bsp_InitAD9833
*	¹¦ÄÜËµÃ÷: ÅäÖÃSTM32µÄGPIOºÍSPI½Ó¿Ú£¬ÓÃÓÚÁ¬½Ó AD9833
*	ÐÎ    ²Î: ÎÞ
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void bsp_InitDAC8562(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

#ifdef SOFT_SPI
	SYNC_1();	/* SYNC = 1 */

	/* ´ò¿ªGPIOÊ±ÖÓ */
	RCC_AHB1PeriphClockCmd(RCC_SCLK | RCC_DIN | RCC_SYNC, ENABLE);

	/* ÅäÖÃ¼¸¸öÍÆÍêÊä³öIO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ÉèÎªÊä³ö¿Ú */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ÉèÎªÍÆÍìÄ£Ê½ */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* ÉÏÏÂÀ­µç×è²»Ê¹ÄÜ */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO¿Ú×î´óËÙ¶È */

	GPIO_InitStructure.GPIO_Pin = PIN_SCLK;
	GPIO_Init(PORT_SCLK, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_DIN;
	GPIO_Init(PORT_DIN, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_SYNC;
	GPIO_Init(PORT_SYNC, &GPIO_InitStructure);
#endif

	/* Power up DAC-A and DAC-B */
	DAC8562_WriteCmd((4 << 19) | (0 << 16) | (3 << 0));
	
	/* LDAC pin inactive for DAC-B and DAC-A  ²»Ê¹ÓÃLDACÒý½Å¸üÐÂÊý¾Ý */
	DAC8562_WriteCmd((6 << 19) | (0 << 16) | (3 << 0));

	/* ¸´Î»2¸öDACµ½ÖÐ¼äÖµ, Êä³ö2.5V */
	DAC8562_SetData(0, 32767);
	DAC8562_SetData(1, 32767);

	/* Ñ¡ÔñÄÚ²¿²Î¿¼²¢¸´Î»2¸öDACµÄÔöÒæ=2 £¨¸´Î»Ê±£¬ÄÚ²¿²Î¿¼ÊÇ½ûÖ¹µÄ) */
	DAC8562_WriteCmd((7 << 19) | (0 << 16) | (1 << 0));
}

/*
*********************************************************************************************************
*	º¯ Êý Ãû: AD9833_WriteCmd
*	¹¦ÄÜËµÃ÷: ÏòSPI×ÜÏß·¢ËÍ16¸öbitÊý¾Ý
*	ÐÎ    ²Î: _cmd : Êý¾Ý
*	·µ »Ø Öµ: ÎÞ
*********************************************************************************************************
*/
void DAC8562_WriteCmd(uint32_t _cmd)
{
	uint8_t i;
	
	FSYNC_0();
	
	/*¡¡DAC8562 SCLKÊ±ÖÓ¸ß´ï50M£¬Òò´Ë¿ÉÒÔ²»ÑÓ³Ù */
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
*º¯ÊýÃû£ºAD9833_Select_Wave
*¹¦ÄÜËµÃ÷£º
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*	º¯ Êý Ãû: DAC8562_SetData
*	¹¦ÄÜËµÃ÷: ÉèÖÃDACÊä³ö£¬²¢Á¢¼´¸üÐÂ¡£
*	ÐÎ    ²Î: _ch, Í¨µÀ, 0 , 1
*		     _data : Êý¾Ý
*	·µ »Ø Öµ: ÎÞ
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

/***************************** °²¸»À³µç×Ó www.armfly.com (END OF FILE) *********************************/
