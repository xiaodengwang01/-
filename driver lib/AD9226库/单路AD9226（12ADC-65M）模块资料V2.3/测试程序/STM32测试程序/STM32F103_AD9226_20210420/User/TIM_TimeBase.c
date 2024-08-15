//-----------------------------------------------------------------
// ��������: 
//     - TIM3�����ó�ʼ����TIM3�ж������������
// ��    ��: ���ǵ���
// ��ʼ����: 2014-01-31
// �������: 2014-01-31
// �޸�����:
// ��    ��: V1.0
//   - V1.0: TIM3��ʱ�ж�
// ���Թ���: ����STM32���Ŀ����塢LZE_ST LINK2
// ˵    ��:
//    - ����ʹ�õ�ϵͳʱ��Ƶ��Fsysclk=72MHz��
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// ͷ�ļ�����
//-----------------------------------------------------------------
#include <stm32f10x.h>
#include "TIM_TimeBase.h"
//-----------------------------------------------------------------

extern uint16_t AD9226_Data[Draw_Number];
extern uint8_t  Time_Flag;
uint16_t k=0;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//-----------------------------------------------------------------
// void TIM3_Mode_Init(uint16_t arr,uint16_t psc)
//-----------------------------------------------------------------
//
// ��������: TIM3���ã�����ʱ�ӡ���ʱʱ��
// ��ڲ���: ��
// �� �� ֵ: ��
// ע������: TIMx�Ķ�ʱʱ��T=((Period+1)*(Prescaler+1))/TIMx_clk
//      - Period:�Զ�װ��ֵ
//			- Prescaler����Ƶֵ
//			- TIMx_clk: APB2ʱ�ӣ���Ϊ72M
//-----------------------------------------------------------------
void TIM3_Mode_Init(uint16_t arr,uint16_t psc)
{
	// ʹ��TIM3ʱ��    
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  // ��������һ�������¼�װ���Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Period = arr;	
	// ����TIMxʱ�ӵ�Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 			 
	// ����ʱ�ӷָ�:Tdts = Tck_int�����ָ�
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 			 			
	// TIMx���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 									
 																						
}

//-----------------------------------------------------------------
// static void NVIC_Configuration(void)
//-----------------------------------------------------------------
//
// ��������: TIM3����Ƕ���ж�����
// ��ڲ���: ��
// �� �� ֵ: ��
// ע������: 
//-----------------------------------------------------------------
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  // ����NVIC�жϷ���2������2λ��ռ���ȼ���2λ��Ӧ���ȼ�
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               
  // TIM3�ж�
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;		 
	// ��ռ���ȼ�0��
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	// �����ȼ�0��
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
	// ʹ��TIM3�ж�
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
  NVIC_Init(&NVIC_InitStructure);
	
		// ʹ��TIMx�����ж�
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);													
	// ʹ��TIMx
	TIM_Cmd(TIM3, ENABLE); 
}

//-----------------------------------------------------------------
// void TIM3_Init(uint16_t arr,uint16_t psc)
//-----------------------------------------------------------------
//
// ��������: TIM3��ʼ��
// ��ڲ���: ��
// �� �� ֵ: ��
// ע������:
//-----------------------------------------------------------------
void TIM3_Init(uint16_t arr,uint16_t psc)
{
	TIM3_Mode_Init(arr,psc);
	NVIC_Configuration();
}

//-----------------------------------------------------------------
// void TIM3_IRQHandler(void)
//-----------------------------------------------------------------
//
// ��������: ��ʱ��1�����жϷ�����
// ��ڲ���: ��
// �� �� ֵ: ��
// ע������: ��
//-----------------------------------------------------------------

void TIM3_IRQHandler(void)   
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)     //���ָ����TIM�жϷ��������ж�
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);          //���TIMx�ĸ����жϱ�־λ	
		GPIOA->BRR =  GPIO_Pin_0;
		AD9226_Data[k] = GPIOC->IDR;
		GPIOA->BSRR = GPIO_Pin_0;	
		k++;
		if(k==Sampling_Number)
		{
			Time_Flag = 1;
			k = 0;
			TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
			TIM_Cmd(TIM3, DISABLE);
		}
	}
}
//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
