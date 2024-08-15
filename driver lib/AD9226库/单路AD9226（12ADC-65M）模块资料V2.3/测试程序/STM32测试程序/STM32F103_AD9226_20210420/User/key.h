//-----------------------------------------------------------------
// ��������ģ��ͷ�ļ�
// ͷ�ļ���: key.h
// ��    ��: ���ǵ���
// ��дʱ��: 2014-02-01
// �޸�����: 2014-02-02
// ��ǰ�汾: V1.0.1 
// ��ʷ�汾:
//-----------------------------------------------------------------

#ifndef _KEY_H_
#define _KEY_H_
#include "stm32f10x.h"

#define KEY1 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0)
#define KEY2 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1)
#define KEY3 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)
#define KEY4 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)

void Key_Init(void);
u8 Key_Scan(void);


#endif
//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
