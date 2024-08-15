/*
*********************************************************************************************************
*
*	ģ������ : AD9833 ����ģ��(��ͨ����16λDAC)
*	�ļ����� : bsp_adc9833.c
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_AD9833_H
#define _BSP_AD9833_H

/*���岨�εļĴ���ֵ*/
#define Triangle_Wave  0x2002
#define Sine_Wave 0x2028
#define  square wave 0x2000

#define FREQ_0 0
#define FREQ_1 1

void bsp_InitAD9833(void);
void AD9833_Write_16Bits(uint16_t _cmd);

/* AD9833�������� */
void AD9833_Write_16Bits(uint16_t _cmd) ;//дһ���ֵ�AD9833
void AD9833_SetFreq(uint32_t _freq) ;
void AD9833_SelectWave(uint8_t _Type) ;

#endif


/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
