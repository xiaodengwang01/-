/*---------------------------------------------------------------------
����״̬ɨ��
Ч�ʣ��ǳ��ߣ����Ĳ��ַǳ���
���߱����ܣ�����������������������Ӧ��������Ӧһ��
ע�⣺˫������������ֻ��һ��ʱ�����ж��Ƿ��ٴε�������
���ߣ��������
----------------------------------------------------------------------*/
#include "KeyScan.h"
#include "main.h"
#include "cot_menu.h"

#define TimerGap		1		//��ʱ���ü�� ������1ms,������10ms


//�����ӿڣ�����51��Ƭ������ֱ����ôд��32ֻҪRead_GPIO�������ؼ���
#define Key_One			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)		//����1
#define Key_Two			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)		//����2
#define Key_Thr			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)		//����3
#define Key_Fou			HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)		//����4

#define DownTime_Init   (1000/TimerGap)		//��ʼ����ʱ�䣬1s
#define Enter_Gap		(100/TimerGap)		//����������,0.1s


uint16_t DownTime_Set[Key_Num] = 		//�����趨ʱ��
{
	DownTime_Init,DownTime_Init,DownTime_Init,DownTime_Init,
};

uint16_t DownTime_Now[Key_Num] = 		//����ʵʱʱ��
{
	0,0,0,0,
};

/*---------------------���ձ�־----------------------*/
uint8_t Press_Short[Key_Num] =			//�̰���־
{
	Key_Up,Key_Up,Key_Up,Key_Up,
};

uint8_t Press_Long[Key_Num]  = 			//������־
{
	Key_Up,Key_Up,Key_Up,Key_Up,
};


/*�Ƿ���������������*/
uint8_t Continue_Enter[Key_Num] =
{
	Enter_Yes,Enter_Yes,Enter_Yes,Enter_Yes,
};




/*---------------------------------------------------------------------------
������Key_Scan
���ܣ�����ɨ��
ʹ�÷�������ʱ�����ã�����Ƶ�ʺ�TimerGap�йأ�Ҫô1ms,Ҫô10ms
----------------------------------------------------------------------------*/
void Key_Scan(void)
{
	uint8_t i=0;
	static uint8_t Press_Long_In[Key_Num] =		//�ڲ���¼����
	{
		Key_Up,Key_Up,Key_Up,Key_Up,
	};
	uint8_t Key_State[Key_Num] = 		//��¼����״̬
	{
		Key_Up,Key_Up,Key_Up,Key_Up,
	};
	/*--------------------ɨ��n������------------------------*/
	Key_State[0] = Key_One;			//ע�ⰴ��Ϊ0������Ϊ1
	Key_State[1] = Key_Two;
	Key_State[2] = Key_Thr;
	Key_State[3] = Key_Fou;

	/*-----------------------���̰��㷨------------------------*/
	for(i=0;i<4;i++)
	{
		if(Key_State[i] == Key_Down)		//����
		{
			DownTime_Now[i]++;

			if(DownTime_Now[i]>(2500/TimerGap))			//��ֹ���Ϊ��Ϊʲô��2500���ֵ��8λ���255�йأ������ó���8λ��ʱ���������250
			{
				DownTime_Now[i] = DownTime_Init+10;		//ÿ�����Ҫ���óɱ�Ŀ��ʱ���һЩ��ֵ����Ȼ���ж�Ϊ�̰�
			}

			/*��������*/
			if(Continue_Enter[i] == Enter_Yes)			//�Ƿ�������������
			{
				if(DownTime_Now[i] > DownTime_Set[i])		//����
				{
					DownTime_Now[i] -= Enter_Gap;				//���ý�������20x10ms=200ms,һֱ���Ż�ÿ���200msִ��һ��
													//��Ϊÿ�ν�����ȥ20������Ҫһ��ʱ��ȥ���
					//���볤����Ҫִ�еĲ���
					Press_Long[i]    = Key_Down;			//�������±�־
					Press_Long_In[i] = Key_Down;			//���������ڲ���־
				}

			}
		}
		else						//�ͷ�
		{
			//���ж��ǲ��ǳ���
			if(DownTime_Now[i] > DownTime_Set[i] || Press_Long_In[i] == Key_Down)		//ʱ��ﵽ�����б�־λ
			{
				Press_Long_In[i] = Key_Up;		//Press_Long_In������������DownTime_Now<DownTime_Set���ж�Ϊ�̰�
			}
			else if(DownTime_Now[i]>(20/TimerGap))		//�̰�,���ڶ���ʱ�䣬20ms
			{
				Press_Short[i] = Key_Down;		//�̰���־����
			}
			DownTime_Now[i] = 0;		//�ͷź�ʱ������
			Continue_Enter[i] = Enter_Yes;
		}
	}
}



uint8_t test = 0;

/*-------------------------------------------------------------------
������Key_Dispose
���ܣ���������
ʹ�÷�������ѭ������
-------------------------------------------------------------------*/
void Key_Dispose(void)
{
//	if(Press_Short[0] == Key_Down)
//	{
//		Press_Short[0] = Key_Up;	
//		test++;	
//		cotMenu_SelectPrevious(true);
//		
//	}
//	if(Press_Short[1] == Key_Down)
//	{
//		Press_Short[1] = Key_Up;		
//		test--;		
//		cotMenu_SelectNext(true);
//	}
//	if(Press_Long[0] == Key_Down)
//	{
//		Press_Long[0] = Key_Up;
//		test++;	
//		cotMenu_SelectPrevious(true);
//	}
//	if(Press_Long[1] == Key_Down)
//	{
//		Press_Long[1] = Key_Up;
//		test--;	
//		cotMenu_SelectNext(true);
//	}	
	/*
	�ж����ձ�־����
	Press_Short
	Press_Long

	��if(Press_Short[0] == Key_Down)
	{
		Press_Short[0] = Key_Up;

		//ִ����Ҫ�Ĳ���
	}

	��ϼ���ô�죿
	�½�һ��KeyStatus
	��״̬�����������������ͳһ�ж�


	KeyStatus |= 0x01;		//����1����
	KeyStatus |= 0x02;		//����2����
	KeyStatus |= 0x04;		//����3����
	KeyStatus |= 0x08;		//����4����

	//�ж�1��2��ͬʱ����
	if(KeyStatus & 0x03 == 0x03)
	*/

}


