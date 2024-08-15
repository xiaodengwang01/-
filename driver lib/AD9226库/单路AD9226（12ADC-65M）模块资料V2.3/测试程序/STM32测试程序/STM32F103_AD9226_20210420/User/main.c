//-----------------------------------------------------------------
// ��������:
//    AD9226-ADC�ɼ�ʵ��
// ��    ��: ���ǵ���
// ��ʼ����: 2021-04-20
// �������: 2021-04-20
// �޸�����: 2021-04-20
//   - V1.0:  AD9226-ADC�ɼ�ʵ��
// ���Թ���: ����STM32���Ŀ����塢LZE_ST LINK2��2.8��Һ������·AD9226ģ��
// ˵    ��: Ӳ�����ӣ�
//						STM32H750			��·AD9226ģ��
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
// ����˵����KEY1�����Ӵ�����ʣ�KEY2�������Ͳ����ʣ���
//
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// ͷ�ļ�����
//-----------------------------------------------------------------
#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "key.h"
#include "lze_lcd.h"
#include "TIM_TimeBase.h"
#include "ad9226.h"

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
	uint16_t c1,c2,Draw_H;
	
	// �����һ����ʾ������
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
	// ��������
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
// ������
//-----------------------------------------------------------------
int main(void)
{
	uint16_t k=0;
	uint8_t  key,i=0;
	uint16_t Gear = 0;
	uint32_t Sampling_Rate[7] = {2300000,250000,100000,50000,10000,5000,1000};// ��λHz
	uint16_t *pb = AD9226_Data;

	GPIO_LED_Configuration(); // LED��ʼ��
	Key_Init();								// ������ʼ��
	LCD_Init();               // Һ����ʼ��
	AD9226_Init();					// ��ʼ��AD9226
	LCD_Clear(LCD_COLOR_WHITE);				// ����
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
			if(Sampling_Rate[Gear] == 2300000)	// ������Ϊ2.3MHz���ò�����ȡ����IO�ڵ��ٶ�
			{
				k=Sampling_Number;
				pb = AD9226_Data;
				do
				{
					GPIOA->BRR =  GPIO_Pin_0;
					*(pb++) = GPIOC->IDR;
					GPIOA->BSRR = GPIO_Pin_0;	
				}while(k--);
				LCD_Wave(&AD9226_Data[10]);	// ����AD9226�Ĵ�����ʱ10��ʱ�ӣ�ǰ10��ʱ�ӵ�����������
			}
			else	// С�ڵ���250KHz�Ĳ������ö�ʱ������
			{
				TIM3_Init(144-1,72000000/144/Sampling_Rate[Gear]-1);
				while(Time_Flag == 0);
				LCD_Wave(&AD9226_Data[10]);	// ����AD9226�Ĵ�����ʱ10��ʱ�ӣ�ǰ10��ʱ�ӵ�����������
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
