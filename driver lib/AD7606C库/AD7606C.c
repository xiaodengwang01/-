#include "AD7606C.h"

uint16_t AD7606C_Init(uint16_t* P, uint16_t Range, uint16_t Bandwidth, uint16_t OverSamp)
{
	uint16_t ID = 0;
	//设置采样范围
	*P = Range1_Adress | Range;
	HAL_Delay(1);
	*P = Range2_Adress | Range;
		HAL_Delay(1);
	*P = Range3_Adress | Range;
		HAL_Delay(1);
	*P = Range4_Adress | Range;
		HAL_Delay(1);
	//设置带宽
	*P = Bandwidth_Adress | Bandwidth;
		HAL_Delay(1);
	//设置过采样
	*P = Oversampling_Adress | OverSamp;
		HAL_Delay(1);
	//读取ID
	*P = 0x8000 | Name_ID_Adress;
	HAL_Delay(1);
	ID = *P;
		HAL_Delay(1);
	//恢复ADC模式
	*P = 0x0000;
	
	return ID;
}
