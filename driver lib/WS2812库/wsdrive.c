#include "gpio.h"
#include "tim.h"
#include "wsdrive.h"

#define WS2812_1 61
#define WS2812_0 29
#define NUM 8	//灯珠数量

uint16_t Send[NUM*24+300] = {0,};

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)//写入DMA停止规则
{
	if(htim == &htim1)
	HAL_TIM_PWM_Stop_DMA(&htim1,TIM_CHANNEL_1);
}

void ws_load(void)	//发送缓冲区
{
	HAL_TIM_PWM_Start_DMA(&htim1,TIM_CHANNEL_1,(uint32_t*)Send,NUM*24+300);
}
/*R G B为灯珠的RGB值，array为数组，定义灯珠的点亮规则，规定array[i]为0则不亮，非0值则亮起
如array[8] = {1,1,1,1,0,0,1,1}代表1号、2号、3号、4号、7号、8号灯珠亮起，5号、6号灯珠不亮*/
void ws_light(uint8_t R, uint8_t G, uint8_t B, uint8_t *array)
{
	uint32_t buff[24];
	uint8_t i,j;
	for(i=8;i>0;i--)
	{
		buff[i-1] = G & 0x1 ? WS2812_1 : WS2812_0;
		G >>= 1;
	}
	for(i=8;i>0;i--)
	{
		buff[i+7] = R & 0x1 ? WS2812_1 : WS2812_0;
		R >>= 1;
	}
	for(i=8;i>0;i--)
	{
		buff[i+15] = B & 0x1 ? WS2812_1 : WS2812_0;
		B >>= 1;
	}
	for(i=0;i<NUM;i++)
	{
		if(array[i])
		{
			for(j=0; j<24; j++)
			Send[i*24+j] = buff[j];
		}
		else
		{
			for(j=0; j<24; j++)
			Send[i*24+j] = WS2812_0;	
		}
	}
	ws_load();
}

void ws_rainbow(void)
{
	static uint8_t Rv[NUM]={6,};	//RGB数组
	static uint8_t Gv[NUM]={6,};
	static uint8_t Bv[NUM]={6,};
	
	static uint8_t RS=1;
	static uint8_t GS=1;
	static uint8_t BS=1;
	
	uint32_t buff[24];
	uint8_t i,j;

	for(i=NUM-1;i>0;i--)//刷新灯的颜色
	{
		Rv[i] = Rv[i-1];
	}
	if(Rv[1]>240) RS = 0;
	else if (Rv[1]<10) RS = 1;
	if(RS)	Rv[0] = Rv[1]+3;
	else Rv[0] = Rv[1]-3;
	
	for(i=NUM-1;i>0;i--)
	{
		Gv[i] = Gv[i-1];
	}
	if(Gv[1]>240) GS = 0;
	else if (Gv[1]<10) GS = 1;
	if(GS)	Gv[0] = Gv[1]+4;
	else Gv[0] = Gv[1]-4;
	
	for(i=NUM-1;i>0;i--)
	{
		Bv[i] = Bv[i-1];
	}
	if(Bv[1]>240) BS = 0;
	else if (Bv[1]<10) BS = 1;
	if(BS)	Bv[0] = Bv[1]+5;
	else Bv[0] = Bv[1]-5;
	
	

	for(j=0;j<NUM;j++)//加载灯光
	{
		uint8_t R = Rv[j];
		uint8_t G = Gv[j];
		uint8_t B = Bv[j];
		for(i=8;i>0;i--)
		{
			buff[i-1] = G & 0x1 ? WS2812_1 : WS2812_0;
			G >>= 1;
		}
		for(i=8;i>0;i--)
		{
			buff[i+7] = R & 0x1 ? WS2812_1 : WS2812_0;
			R >>= 1;
		}
		for(i=8;i>0;i--)
		{
			buff[i+15] = B & 0x1 ? WS2812_1 : WS2812_0;
			B >>= 1;
		}
		
		for(i=0; i<24; i++)
			Send[j*24+i] = buff[i];
	}
	
	ws_load();
}
