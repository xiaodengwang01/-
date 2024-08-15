/*
 * @Description:AD9834���������ļ�
 * @Author: MALossov
 * @Date: 2022-06-28 16:37:55
 * @LastEditTime: 2022-10-06 02:56:10
 * @LastEditors: MALossov
 * @Reference:
 */
#ifndef __AD9834_H 
#define __AD9834_H 
#include <stdint.h>
#include "stm32f4xx_hal.h"  //�������оƬ�ľ����ź�ѡ���Լ���ͷ�ļ�
#include "main.h"
#include "usart.h"
#include "stdio.h"

 /*--------------------- AD9834�����ź� 16Bit ---------------------*/
/**
 * @brief ���ⲿ�ֵ��У�������AD9834�󲿷ֿ����źŵĶ��壬�����˿����źŵĺ궨��
 * @useage ʹ�ú���AD9834_Write_16Bits()��д������źţ�
 *         ����Ҳ����ʹ��AD9834_Select_Wave()��д�루����ͨ������ֱ��ʹ��ĳ�ֲ��Σ�
 */
 //���û����Ĳ��Σ�Ĭ��ʹ��Ƶ�ʼĴ���0
#define Triangle_Wave    0x2002 
#define Sine_Wave  0x2008 
#define Square_Wave 0x2028

//����ʹ��Ƶ�ʼĴ���1
#define Sine_Wave_Reg1 0x2808  
#define Square_Wave_Reg1 0x2828 
#define Triangle_Wave_Reg1 0x2802

//����ͨ������ѡ��������
#define SelectByPin 0x2208  //����PIN_SW λ��ѡ��ͨ������ѡ��Ƶ�ʼĴ�������λ���Ƿ񲻷�����
//�����ϵ�Pin�У�����FS��PS��RESET��SLEEP�ĸ����ţ������Լ������д��

//���¸�λAD9834
#define RESET_AD9834 0x2100

//ֹͣAD9834��DAC���
#define STOP_DAC 0x2040

//���ײ�������
//��������������ֲᣬ��ҪóȻʹ����һ����
//���ʹ���ⲿ�֣���Ը�������� | ����������������ʹ��~����������������׳������⡣

#define AD9834_B28  0x2000  //����Ϊ28λģʽ
#define AD9834_HLB  0x0100  //����Ϊ����ģʽ����Ҫ����ʹ��

#define AD9834_RESET 0x0100  //��λAD9834
//#define AD9834_RESET  GPIO_PIN_10	 
//��ʾ��������ض����� 

#define AD9834_SLEEP1 0x0080  //����Ϊ˯��ģʽ1
#define AD9834_SLEEP12 0x0040  //����Ϊ˯��ģʽ2
#define AD9834_OPBITEN 0x0020  //����Ϊ���ʹ��
#define AD9834_DIV2 0x0010  //����Ϊ��Ƶģʽ
#define AD9834_MODE 0x0002  //����Ϊ���ǲ�ģʽ
#define AD9834_SIGN_PIBIT 0x0001  //����Ϊ���Ҳ�ģʽ
#define AD9834_PHASE 0xC000  //������λ�Ĵ���0
#define AD98er_PHASE1 0xE000  //������λ�Ĵ���1


//------------------AD9834���ƺ���������------------------//
/**
 * @brief ���ⲿ���У�������AD9834��һЩ���ԣ�������AD9834��һЩ�������ź�����ʱ��
 * @notice ��ʹ��AD9834ʱ����Ҫ�����Լ�������޸��ⲿ�ֵ�����
 */

 /* AD9834����Ƶ�� */
#define AD9834_SYSTEM_COLCK     75000000UL 


/* AD9834 �������� */
#define AD9834_Control_Port  GPIOB
#define AD9834_FSYNC  GPIO_PIN_15    
#define AD9834_SCLK   GPIO_PIN_14			
#define AD9834_SDATA  GPIO_PIN_13 		
#define AD9834_RESET  GPIO_PIN_10		
#define AD9834_FS  		GPIO_PIN_12		
#define AD9834_PS  		GPIO_PIN_11		

//FSYNC��֡ͬ�������š�֡ͬ������ͬ�����ε���λ��ȷ�����εĿ�ʼ�ͽ�����ͬ���ġ�
//SCLK������ʱ�ӣ����š�����ʱ�����ڿ������ݴ���Ľ��ࡣ
//SDATA���������ݣ����š������������ڴ��䲨�β����Ϳ�����Ϣ��
//RESET����λ�����š���λ�������ڽ�AD9834���õ���ʼ״̬��
//FS��Ƶ�ʺϳ��������š�Ƶ�ʺϳ����������ò��ε�Ƶ�ʡ�
//PS����λѡ�����š���λѡ���������ò��εĳ�ʼ��λ�� 


//Ϊ���ݱ�׼�⣬�ṩ���º�
#define GPIO_SetBits(x,y) HAL_GPIO_WritePin((x),(y),GPIO_PIN_SET)
#define GPIO_ResetBits(x,y) HAL_GPIO_WritePin((x),(y),GPIO_PIN_RESET)

//Ϊ�˱��ڲ����������ţ��ṩ���º�
#define AD9834_FSYNC_SET   GPIO_SetBits(AD9834_Control_Port ,AD9834_FSYNC) 
#define AD9834_FSYNC_CLR   GPIO_ResetBits(AD9834_Control_Port ,AD9834_FSYNC) 

#define AD9834_SCLK_SET   GPIO_SetBits(AD9834_Control_Port ,AD9834_SCLK) 
#define AD9834_SCLK_CLR   GPIO_ResetBits(AD9834_Control_Port ,AD9834_SCLK) 

#define AD9834_SDATA_SET   GPIO_SetBits(AD9834_Control_Port ,AD9834_SDATA) 
#define AD9834_SDATA_CLR   GPIO_ResetBits(AD9834_Control_Port ,AD9834_SDATA) 

#define AD9834_RESET_SET   GPIO_SetBits(AD9834_Control_Port ,AD9834_RESET) 
#define AD9834_RESET_CLR   GPIO_ResetBits(AD9834_Control_Port ,AD9834_RESET) 

//Ϊ�˱��ڲ���Ƶ�ʵȿ������ţ��ṩ���º�
#define AD9834_FS_SET   GPIO_SetBits(AD9834_Control_Port ,AD9834_FS)
#define AD9834_FS_CLR   GPIO_ResetBits(AD9834_Control_Port ,AD9834_FS)

#define AD9834_PS_SET   GPIO_SetBits(AD9834_Control_Port ,AD9834_PS)
#define AD9834_PS_CLR   GPIO_ResetBits(AD9834_Control_Port ,AD9834_PS)

#define AD9834_RST_SET   GPIO_SetBits(AD9834_Control_Port ,AD9834_RESET)
#define AD9834_RST_CLR   GPIO_ResetBits(AD9834_Control_Port ,AD9834_RESET)

/*--------------------- AD9834����λ���� ---------------------*/
/**
 * @brief �ⲿ�ְ�����AD9834��һЩ����λ��������������AD9834��һЩ����λ�ĺ궨��
 * @useage ʹ�ú���AD9834_Write_16Bits()��д������źš�
 * @notice ����в�����ĵط������������ֲ���и���
 */

 //Ƶ�ʼĴ����Ŀ���λ
#define FREQ_0      0 
#define FREQ_1      1 
#define PHASE_0     0
#define PHASE_1     1

//����λ�Ķ��壬����Ϊ0
//�����Ҫʹ������λ�������������صı���/��Ķ���
#define DB15        0 
#define DB14        0 
#define DB13        B28 
#define DB12        HLB 
#define DB11        FSEL 
#define DB10        PSEL 
#define DB9         PIN_SW 
#define DB8         RESET
#define DB7         SLEEP1 
#define DB6         SLEEP12 
#define DB5         OPBITEN 
#define DB4         SIGN_PIB 
#define DB3         DIV2 
#define DB2         0 
#define DB1         MODE 
#define DB0         0 

//ͨ������λ�ĺ궨�壬���Եõ������źŵ�ֵ
//����ͨ��AD9834_Write_16Bits()������ֱ��д������ź�
#define CONTROL_REGISTER    (DB15<<15)|(DB14<<14)|(DB13<<13)|(DB12<<12)|(DB11<<11)|(DB10<<10)|(DB9<<9)|(DB8<<8)|(DB7<<7)|(DB6<<6)|(DB5<<5)|(DB4<<4)|(DB3<<3)|(DB2<<2)|(DB1<<1)|(DB0<<0) 

 /*--------------------- AD9834�������� ---------------------*/
void AD9834_Write_16Bits(unsigned int data);  //дһ���ֵ�AD9834 
void AD9834_Select_Wave(unsigned int initdata); //ѡ��������� 
void AD9834_Init(void);  					//��ʼ������ 
void AD9834_Set_Freq(unsigned char freq_number, unsigned long freq);//ѡ������Ĵ��������Ƶ��ֵ 

#endif /* AD9834_H */ 
