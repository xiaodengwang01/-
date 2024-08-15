//-----------------------------------------------------------------
// ��������:
//     AD9226��������
// ��    ��: ���ǵ���
// ��ʼ����: 2021-04-20
// �������: 2021-04-20
// �޸�����: 2021-04-20
// ��ǰ�汾: V1.0
// ��ʷ�汾:
//  - V1.0: AD9226����
// ���Թ���: ����STM32���Ŀ����塢LZE_ST_LINK2����·AD9226ģ��
// ˵    ��:
//
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// ͷ�ļ�����
//-----------------------------------------------------------------
#include <stm32f10x.h>
#include "ad9226.h"

//-----------------------------------------------------------------
// void AD9226_Init(void)
//-----------------------------------------------------------------
// 
// ��������: AD9226��ʼ��
// ��ڲ���: �� 
// �� �� ֵ: ��
// ע������: ��
//
//-----------------------------------------------------------------
void AD9226_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // ʹ��IO��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | 
																	GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | 
																	GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10| GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

}

//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
