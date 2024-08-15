//-----------------------------------------------------------------
// ��������:
//     AD9226-ADC�ɼ�ʵ��
// ��    ��: ���ǵ���
// ��ʼ����: 2020-11-11
// �������: 2020-11-11
// �޸�����: 
// ��ǰ�汾: V1.0
// ��ʷ�汾:
//  - V1.0:  AD9226-ADC�ɼ�ʵ��
// ���Թ���: ����STM32H750���İ塢LZE_ST_LINK2��4.3��/2.8�� TFTҺ��������·AD9226ģ��
// ˵    ��: Ӳ�����ӣ�
//						STM32H750			��·AD9226ģ��
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
// ����˵����KEY1�����Ӵ�����ʣ�KEY2�������Ͳ����ʣ���
// ע�⣺��ʾ�����ǰ���2.8������Ƶģ���4.3����ʱ��ֻ����ʾ�����Ͻǣ���
//    
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// ͷ�ļ�����
//-----------------------------------------------------------------
#include "system.h"
#include "delay.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "ad9226.h"
#include "timer.h"
//-----------------------------------------------------------------

uint16_t AD9226_Data[Sampling_Number];	// ���ݻ�����
uint16_t Draw_data[Draw_Number]; 				// ���Ƶ����ݻ�����
uint8_t  Time_Flag = 0;									// �жϱ�־λ

//-----------------------------------------------------------------
// void LCD_Wave(uint16_t *data)
//-----------------------------------------------------------------
//
// ��������: ��ָ�������ڻ��Ʋ���
// ��ڲ���: uint16_t *data�����ƵĲ�������
// ���ز���: ��
// ע������: ��
//					 
//-----------------------------------------------------------------
void LCD_Wave(uint16_t *data)
{  
	uint16_t adcx;
	uint16_t i;
	uint16_t c1,c2;
	
	// �����һ����ʾ������
	POINT_COLOR=WHITE;
	for(i=0;i<Draw_Number-1;i++)
	{
		c1 = Draw_data[i];
		c2 = Draw_data[i+1];
		LCD_DrawLine(i+40,c1,i+40,c2);
	}
	// ��������
	POINT_COLOR=RED;
	for(i=0;i<Draw_Number;i++)
	{
		// AD9226_Data[300]�õ�����GPIOB����������ݣ�AD9226��12λ�ģ�ֻ��ҪGPIOB�ĵ�ʮ��λ
		adcx = data[i]&0x0FFF;
		// ģ��������������ת����A = (4095 - D)/4095*10000-5000������AΪģ������DΪ������
		// A�ķ�ΧΪ����5V��Ϊ���ò�����ʾ��Y���ǰ200����A��Χ����Ϊ��0��200������Y��0��Ӧ5V��Y��200��Ӧ-5V
		Draw_data[i] = (uint16_t)(((double)adcx/4095*10000)/50);
	}
	// ��ʾ����һ�ε�����
	for(i=0;i<Draw_Number-1;i++)
	{
		c1 = Draw_data[i];
		c2 = Draw_data[i+1];
		LCD_DrawLine(i+40,c1,i+40,c2);
	}
} 

//-----------------------------------------------------------------
// ������
//-----------------------------------------------------------------
int main(void)
{
	uint16_t k=0;
	uint8_t  key,i;
	uint16_t Gear = 0;
	uint32_t Sampling_Rate[8] = {7670000,1000000,500000,100000,50000,10000,5000,1000};// ��λHz
	uint16_t *pb = AD9226_Data;
  HAL_Init();          		// ��ʼ��HAL��
	MPU_Memory_Protection();// ���ñ�������
	SystemClock_Config(); 	// ����ϵͳʱ��,400Mhz  
	SysTick_clkconfig(400);	// SysTick��������
	LED_Init();           	// ��ʼ��LED 
	KEY_Init();							// ��ʼ������
	LCD_Init();           	// LCD��ʼ��
	AD9226_Init();					// ��ʼ��AD9226
	LCD_Clear(WHITE);				// ����
	POINT_COLOR=BLACK; 			// �ʻ���ɫ
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
			if(Sampling_Rate[Gear] == 7670000)	// ������Ϊ7.67MHz���ò�����ȡ����IO�ڵ��ٶ�
			{
				k=Sampling_Number;
				pb = AD9226_Data;
				do
				{
					GPIOC->BSRRH = GPIO_PIN_0;
					*(pb++) = GPIOB->IDR;
					GPIOC->BSRRL = GPIO_PIN_0;	
				}while(k--);
				LCD_Wave(&AD9226_Data[10]);	// ����AD9226�Ĵ�����ʱ10��ʱ�ӣ�ǰ10��ʱ�ӵ�����������
			}
			else	// С�ڵ���1MHz�Ĳ������ö�ʱ������
			{
				TIM3_Init(100-1,200000000/100/Sampling_Rate[Gear]-1);
				while(Time_Flag == 0);
				LCD_Wave(&AD9226_Data[10]);	// ����AD9226�Ĵ�����ʱ10��ʱ�ӣ�ǰ10��ʱ�ӵ�����������
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
