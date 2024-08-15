//-----------------------------------------------------------------
// 程序描述: 
//     - TIM3的配置初始化和TIM3中断向量表的配置
// 作    者: 凌智电子
// 开始日期: 2014-01-31
// 完成日期: 2014-01-31
// 修改日期:
// 版    本: V1.0
//   - V1.0: TIM3定时中断
// 调试工具: 凌智STM32核心开发板、LZE_ST LINK2
// 说    明:
//    - 调试使用的系统时钟频率Fsysclk=72MHz；
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// 头文件包含
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
// 函数功能: TIM3配置，包括时钟、定时时基
// 入口参数: 无
// 返 回 值: 无
// 注意事项: TIMx的定时时基T=((Period+1)*(Prescaler+1))/TIMx_clk
//      - Period:自动装载值
//			- Prescaler：分频值
//			- TIMx_clk: APB2时钟，即为72M
//-----------------------------------------------------------------
void TIM3_Mode_Init(uint16_t arr,uint16_t psc)
{
	// 使能TIM3时钟    
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  // 设置在下一个更新事件装入自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Period = arr;	
	// 设置TIMx时钟的预分频值
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 			 
	// 设置时钟分割:Tdts = Tck_int，不分割
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 			 			
	// TIMx向上计数模式
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 									
 																						
}

//-----------------------------------------------------------------
// static void NVIC_Configuration(void)
//-----------------------------------------------------------------
//
// 函数功能: TIM3配置嵌套中断向量
// 入口参数: 无
// 返 回 值: 无
// 注意事项: 
//-----------------------------------------------------------------
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  // 设置NVIC中断分组2，其中2位抢占优先级，2位响应优先级
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               
  // TIM3中断
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;		 
	// 先占优先级0级
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	// 从优先级0级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
	// 使能TIM3中断
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
  NVIC_Init(&NVIC_InitStructure);
	
		// 使能TIMx更新中断
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);													
	// 使能TIMx
	TIM_Cmd(TIM3, ENABLE); 
}

//-----------------------------------------------------------------
// void TIM3_Init(uint16_t arr,uint16_t psc)
//-----------------------------------------------------------------
//
// 函数功能: TIM3初始化
// 入口参数: 无
// 返 回 值: 无
// 注意事项:
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
// 函数功能: 定时器1更新中断服务函数
// 入口参数: 无
// 返 回 值: 无
// 注意事项: 无
//-----------------------------------------------------------------

void TIM3_IRQHandler(void)   
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)     //检查指定的TIM中断发生更新中断
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);          //清除TIMx的更新中断标志位	
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
