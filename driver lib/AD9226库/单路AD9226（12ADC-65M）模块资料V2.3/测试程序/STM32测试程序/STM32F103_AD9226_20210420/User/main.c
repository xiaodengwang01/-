//-----------------------------------------------------------------
// 程序描述:
//    AD9226-ADC采集实验
// 作    者: 凌智电子
// 开始日期: 2021-04-20
// 完成日期: 2021-04-20
// 修改日期: 2021-04-20
//   - V1.0:  AD9226-ADC采集实验
// 调试工具: 凌智STM32核心开发板、LZE_ST LINK2、2.8寸液晶、单路AD9226模块
// 说    明: 硬件连接：
//						STM32H750			单路AD9226模块
//							+5V			-> 			+5V
//							GND			->			GND
// 							PA0  		-> 		AD9226_CLK
// 							PC0 		-> 		AD9226_D11
// 							PC1 		-> 		AD9226_D10
// 							PC2  		-> 		AD9226_D9
// 							PC3  		-> 		AD9226_D8
// 							PC4  		-> 		AD9226_D7
// 							PC5  		-> 		AD9226_D6
// 							PC6  		-> 		AD9226_D5
// 							PC7 		-> 		AD9226_D4
// 							PC8 		-> 		AD9226_D3
// 							PC9 		-> 		AD9226_D2
// 							PC10 		-> 		AD9226_D1
// 							PC11 		-> 		AD9226_D0
//
// 按键说明：KEY1按键加大采样率，KEY2按键降低采样率！！
//
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// 头文件包含
//-----------------------------------------------------------------
#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "key.h"
#include "lze_lcd.h"
#include "TIM_TimeBase.h"
#include "ad9226.h"

//-----------------------------------------------------------------

uint16_t AD9226_Data[Sampling_Number];	// 数据缓存区
uint16_t Draw_data[Draw_Number]; 				// 绘制点数据缓冲区
uint8_t  Time_Flag = 0;									// 中断标志位

//-----------------------------------------------------------------
// void LCD_Wave(uint16_t *data)
//-----------------------------------------------------------------
//
// 函数功能: 在指定区域内绘制波形
// 入口参数: uint16_t *data：绘制的波形数据
// 返回参数: 无
// 注意事项: 无
//					 
//-----------------------------------------------------------------
void LCD_Wave(uint16_t *data)
{  
	uint16_t adcx;
	uint16_t i;
	uint16_t c1,c2,Draw_H;
	
	// 清除上一次显示的数据
	for(i=0;i<Draw_Number-1;i++)
	{
		c1 = Draw_data[i];
		c2 = Draw_data[i+1];
		if(c1>c2)
		{
			Draw_H=c1-c2;
			LCD_Line_V(i+40,c2,Draw_H,LCD_COLOR_WHITE);
		}
		else if(c1<c2)
		{
			Draw_H=c2-c1;
			LCD_Line_V(i+40,c1,Draw_H,LCD_COLOR_WHITE);
		}
		else
		{
			LCD_SetPoint(i+40,c1,LCD_COLOR_WHITE);
		}
	}
	// 处理数据
	for(i=0;i<Draw_Number;i++)
	{
		// AD9226_Data[300]得到的是GPIOB整个组的数据，AD9226是12位的，只需要GPIOB的低十二位
		adcx = data[i]&0x0FFF;
		// 模拟量与数字量的转换：A = (4095 - D)/4095*10000-5000，其中A为模拟量，D为数字量
		// A的范围为：±5V，为了让波形显示在Y轴的前200，将A范围设置为：0～200，其中Y轴0对应5V，Y轴200对应-5V
		Draw_data[i] = (uint16_t)(((double)adcx/4095*10000)/50);
	}
	// 显示最新一次的数据
	for(i=0;i<Draw_Number-1;i++)
	{
		c1 = Draw_data[i];
		c2 = Draw_data[i+1];
		if(c1>c2)
		{
			Draw_H=c1-c2;
			LCD_Line_V(i+40,c2,Draw_H,LCD_COLOR_RED);
		}
		else if(c1<c2)
		{
			Draw_H=c2-c1;
			LCD_Line_V(i+40,c1,Draw_H,LCD_COLOR_RED);
		}
		else
		{
			LCD_SetPoint(i+40,c1,LCD_COLOR_RED);
		}
	}
} 

//-----------------------------------------------------------------
// 主程序
//-----------------------------------------------------------------
int main(void)
{
	uint16_t k=0;
	uint8_t  key,i=0;
	uint16_t Gear = 0;
	uint32_t Sampling_Rate[7] = {2300000,250000,100000,50000,10000,5000,1000};// 单位Hz
	uint16_t *pb = AD9226_Data;

	GPIO_LED_Configuration(); // LED初始化
	Key_Init();								// 按键初始化
	LCD_Init();               // 液晶初始化
	AD9226_Init();					// 初始化AD9226
	LCD_Clear(LCD_COLOR_WHITE);				// 清屏
	LCD_WriteString(23,0,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"5V");
	LCD_WriteString(7,42,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"2.5V");
	LCD_WriteString(23,92,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"0V");
	LCD_WriteString(0,142,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"-2.5V");
	LCD_WriteString(15,184,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"-5V");
	LCD_WriteString(76,202,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"Sampling_Rate: 2.3MHz");
	LCD_WriteString(24,220,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"KEY1: +Sampling    KEY2: -Sampling");
	LCD_Line_V(39,0,200,LCD_COLOR_BLACK);
	LCD_Line_H(0,201,320,LCD_COLOR_BLACK);
	while(1)
	{	
		key = Key_Scan();
		if(key == 2)
		{
			if(Gear < 6)
				Gear += 1;
			switch(Gear)
			{
				case 0: LCD_WriteString(76,202,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"Sampling_Rate: 2.3MHz");break;
				case 1: LCD_WriteString(76,202,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"Sampling_Rate: 250KHz");break;
				case 2: LCD_WriteString(76,202,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"Sampling_Rate: 100KHz");break;
				case 3: LCD_WriteString(76,202,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"Sampling_Rate:  50KHz");break;
				case 4: LCD_WriteString(76,202,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"Sampling_Rate:  10KHz");break;
				case 5: LCD_WriteString(76,202,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"Sampling_Rate:   5KHz");break;
				case 6: LCD_WriteString(76,202,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"Sampling_Rate:   1KHz");break;
				default: break;
			}
		}
		else if(key == 1)
		{
			if(Gear > 0)
				Gear -= 1;
			switch(Gear)
			{
				case 0: LCD_WriteString(76,202,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"Sampling_Rate: 2.3MHz");break;
				case 1: LCD_WriteString(76,202,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"Sampling_Rate: 250KHz");break;
				case 2: LCD_WriteString(76,202,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"Sampling_Rate: 100KHz");break;
				case 3: LCD_WriteString(76,202,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"Sampling_Rate:  50KHz");break;
				case 4: LCD_WriteString(76,202,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"Sampling_Rate:  10KHz");break;
				case 5: LCD_WriteString(76,202,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"Sampling_Rate:   5KHz");break;
				case 6: LCD_WriteString(76,202,LCD_COLOR_BLACK,LCD_COLOR_WHITE,(uint8_t *)"Sampling_Rate:   1KHz");break;
				default: break;
			}
		}
		if(i>=20)
		{
			if(Sampling_Rate[Gear] == 2300000)	// 采样率为2.3MHz，该采样率取决于IO口的速度
			{
				k=Sampling_Number;
				pb = AD9226_Data;
				do
				{
					GPIOA->BRR =  GPIO_Pin_0;
					*(pb++) = GPIOC->IDR;
					GPIOA->BSRR = GPIO_Pin_0;	
				}while(k--);
				LCD_Wave(&AD9226_Data[10]);	// 由于AD9226的传输延时10个时钟，前10个时钟的数据舍弃掉
			}
			else	// 小于等于250KHz的采样率用定时器控制
			{
				TIM3_Init(144-1,72000000/144/Sampling_Rate[Gear]-1);
				while(Time_Flag == 0);
				LCD_Wave(&AD9226_Data[10]);	// 由于AD9226的传输延时10个时钟，前10个时钟的数据舍弃掉
				Time_Flag = 0;
			}
			i=0;
			LED1_Toggle;
		}
		Delay_1ms(5);
		i++;
	}
}

//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
