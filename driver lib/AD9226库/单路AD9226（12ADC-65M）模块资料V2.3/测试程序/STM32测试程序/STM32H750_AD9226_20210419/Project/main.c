//-----------------------------------------------------------------
// 程序描述:
//     AD9226-ADC采集实验
// 作    者: 凌智电子
// 开始日期: 2020-11-11
// 完成日期: 2020-11-11
// 修改日期: 
// 当前版本: V1.0
// 历史版本:
//  - V1.0:  AD9226-ADC采集实验
// 调试工具: 凌智STM32H750核心板、LZE_ST_LINK2、4.3寸/2.8寸 TFT液晶屏、单路AD9226模块
// 说    明: 硬件连接：
//						STM32H750			单路AD9226模块
//							+5V			-> 			+5V
//							GND			->			GND
// 							PC0  		-> 		AD9226_CLK
// 							PB0 		-> 		AD9226_D11
// 							PB1 		-> 		AD9226_D10
// 							PB2  		-> 		AD9226_D9
// 							PB3  		-> 		AD9226_D8
// 							PB4  		-> 		AD9226_D7
// 							PB5  		-> 		AD9226_D6
// 							PB6  		-> 		AD9226_D5
// 							PB7 		-> 		AD9226_D4
// 							PB8 		-> 		AD9226_D3
// 							PB9 		-> 		AD9226_D2
// 							PB10 		-> 		AD9226_D1
// 							PB11 		-> 		AD9226_D0
//
// 按键说明：KEY1按键加大采样率，KEY2按键降低采样率！！
// 注意：显示界面是按照2.8寸屏设计的，用4.3寸屏时，只会显示在左上角！！
//    
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// 头文件包含
//-----------------------------------------------------------------
#include "system.h"
#include "delay.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "ad9226.h"
#include "timer.h"
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
	uint16_t c1,c2;
	
	// 清除上一次显示的数据
	POINT_COLOR=WHITE;
	for(i=0;i<Draw_Number-1;i++)
	{
		c1 = Draw_data[i];
		c2 = Draw_data[i+1];
		LCD_DrawLine(i+40,c1,i+40,c2);
	}
	// 处理数据
	POINT_COLOR=RED;
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
		LCD_DrawLine(i+40,c1,i+40,c2);
	}
} 

//-----------------------------------------------------------------
// 主程序
//-----------------------------------------------------------------
int main(void)
{
	uint16_t k=0;
	uint8_t  key,i;
	uint16_t Gear = 0;
	uint32_t Sampling_Rate[8] = {7670000,1000000,500000,100000,50000,10000,5000,1000};// 单位Hz
	uint16_t *pb = AD9226_Data;
  HAL_Init();          		// 初始化HAL库
	MPU_Memory_Protection();// 设置保护区域
	SystemClock_Config(); 	// 设置系统时钟,400Mhz  
	SysTick_clkconfig(400);	// SysTick参数配置
	LED_Init();           	// 初始化LED 
	KEY_Init();							// 初始化按键
	LCD_Init();           	// LCD初始化
	AD9226_Init();					// 初始化AD9226
	LCD_Clear(WHITE);				// 清屏
	POINT_COLOR=BLACK; 			// 笔画颜色
	LCD_ShowString(23,0,40,16,16,"5V");
	LCD_ShowString(7,42,40,16,16,"2.5V");
	LCD_ShowString(23,92,40,16,16,"0V");
	LCD_ShowString(0,142,40,16,16,"-2.5V");
	LCD_ShowString(15,184,40,16,16,"-5V");
	LCD_DrawLine(39,0,39,199);
	LCD_DrawLine(0,201,319,201);
	LCD_ShowString(76,202,200,16,16,"Sampling_Rate:7.67MHz");
	LCD_ShowString(24,220,300,16,16,"KEY1: +Sampling    KEY2: -Sampling");
	while(1)
	{	
		key = KEY_Scan(0);
		if(key == KEY2_PRES)
		{
			if(Gear < 7)
				Gear += 1;
			POINT_COLOR=BLACK;
			switch(Gear)
			{
				case 0: LCD_ShowString(76,202,200,16,16,"Sampling_Rate:7.67MHz");break;
				case 1: LCD_ShowString(76,202,200,16,16,"Sampling_Rate:   1MHz");break;
				case 2: LCD_ShowString(76,202,200,16,16,"Sampling_Rate: 500KHz");break;
				case 3: LCD_ShowString(76,202,200,16,16,"Sampling_Rate: 100KHz");break;
				case 4: LCD_ShowString(76,202,200,16,16,"Sampling_Rate:  50KHz");break;
				case 5: LCD_ShowString(76,202,200,16,16,"Sampling_Rate:  10KHz");break;
				case 6: LCD_ShowString(76,202,200,16,16,"Sampling_Rate:   5KHz");break;
				case 7: LCD_ShowString(76,202,200,16,16,"Sampling_Rate:   1KHz");break;
				default: break;
			}
		}
		else if(key == KEY1_PRES)
		{
			if(Gear > 0)
				Gear -= 1;
			POINT_COLOR=BLACK;
			switch(Gear)
			{
				case 0: LCD_ShowString(76,202,200,16,16,"Sampling_Rate:7.67MHz");break;
				case 1: LCD_ShowString(76,202,200,16,16,"Sampling_Rate:   1MHz");break;
				case 2: LCD_ShowString(76,202,200,16,16,"Sampling_Rate: 500KHz");break;
				case 3: LCD_ShowString(76,202,200,16,16,"Sampling_Rate: 100KHz");break;
				case 4: LCD_ShowString(76,202,200,16,16,"Sampling_Rate:  50KHz");break;
				case 5: LCD_ShowString(76,202,200,16,16,"Sampling_Rate:  10KHz");break;
				case 6: LCD_ShowString(76,202,200,16,16,"Sampling_Rate:   5KHz");break;
				case 7: LCD_ShowString(76,202,200,16,16,"Sampling_Rate:   1KHz");break;
				default: break;
			}
		}
		if(i>=20)
		{
			if(Sampling_Rate[Gear] == 7670000)	// 采样率为7.67MHz，该采样率取决于IO口的速度
			{
				k=Sampling_Number;
				pb = AD9226_Data;
				do
				{
					GPIOC->BSRRH = GPIO_PIN_0;
					*(pb++) = GPIOB->IDR;
					GPIOC->BSRRL = GPIO_PIN_0;	
				}while(k--);
				LCD_Wave(&AD9226_Data[10]);	// 由于AD9226的传输延时10个时钟，前10个时钟的数据舍弃掉
			}
			else	// 小于等于1MHz的采样率用定时器控制
			{
				TIM3_Init(100-1,200000000/100/Sampling_Rate[Gear]-1);
				while(Time_Flag == 0);
				LCD_Wave(&AD9226_Data[10]);	// 由于AD9226的传输延时10个时钟，前10个时钟的数据舍弃掉
				Time_Flag = 0;
			}
			i=0;
			LED_G_Toggle;
		}
		delay_ms(5);
		i++;
	}
}

//-----------------------------------------------------------------
// End Of File
//----------------------------------------------------------------- 
