#ifndef KeyScan_H
#define KeyScan_H
#include "stdint.h"

#define Key_Num			4		//��������,ע���������������ĳ�ʼ��ֵҪ���ϣ����Ż�Ϊ0

#define Key_Up			1		//��������
#define Key_Down		0		//��������

#define Enter_Yes		1		//������������
#define Enter_No		0		//���������

extern uint8_t Continue_Enter[Key_Num];
extern uint8_t Press_Short[Key_Num];
extern uint8_t Press_Long[Key_Num];

#define KEY1_Short Press_Short[0]
#define KEY2_Short Press_Short[1]
#define KEY3_Short Press_Short[2]
#define KEY4_Short Press_Short[3]

#define KEY1_Long Press_Long[0]
#define KEY2_Long Press_Long[1]
#define KEY3_Long Press_Long[2]
#define KEY4_Long Press_Long[3]

void Key_Scan(void);
void Key_Dispose(void);

#endif
