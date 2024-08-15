//-----------------------------------------------------------------
// 程序描述:
//     AD9226驱动程序
// 作    者: 凌智电子
// 开始日期: 2021-04-20
// 完成日期: 2021-04-20
// 修改日期: 2021-04-20
// 当前版本: V1.0
// 历史版本:
//  - V1.0: AD9226驱动
// 调试工具: 凌智STM32核心开发板、LZE_ST_LINK2、单路AD9226模块
// 说    明:
//
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// 头文件包含
//-----------------------------------------------------------------
#include <stm32f10x.h>
#include "ad9226.h"

//-----------------------------------------------------------------
// void AD9226_Init(void)
//-----------------------------------------------------------------
// 
// 函数功能: AD9226初始化
// 入口参数: 无 
// 返 回 值: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void AD9226_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // 使能IO口时钟
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
