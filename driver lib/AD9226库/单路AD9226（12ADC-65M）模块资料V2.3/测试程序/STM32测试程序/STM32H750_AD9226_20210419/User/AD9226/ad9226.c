#include "ad9226.h"
//-----------------------------------------------------------------

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
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();			// 使能GPIOB时钟
	__HAL_RCC_GPIOC_CLK_ENABLE();			// 使能GPIOC时钟
	__HAL_RCC_GPIOD_CLK_ENABLE();			// 使能GPIOD时钟
	GPIO_Initure.Pin = GPIO_PIN_0;
  GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Initure.Pull = GPIO_PULLUP;
  GPIO_Initure.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_Initure);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);

	GPIO_Initure.Pin = GPIO_PIN_3;
  GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Initure.Pull = GPIO_PULLUP;
  GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_Initure);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);
	
	GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
										 GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
										 GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10| GPIO_PIN_11;
  GPIO_Initure.Mode = GPIO_MODE_INPUT;
  GPIO_Initure.Pull = GPIO_PULLUP;
  GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_Initure);
}

//-----------------------------------------------------------------
// End Of File
//----------------------------------------------------------------- 
