#include "AD7606C.h"

uint16_t AD7606C_Init(uint16_t* P, uint16_t Range, uint16_t Bandwidth, uint16_t OverSamp)
{
	uint16_t ID = 0;
	//���ò�����Χ
	*P = Range1_Adress | Range;
	HAL_Delay(1);
	*P = Range2_Adress | Range;
		HAL_Delay(1);
	*P = Range3_Adress | Range;
		HAL_Delay(1);
	*P = Range4_Adress | Range;
		HAL_Delay(1);
	//���ô���
	*P = Bandwidth_Adress | Bandwidth;
		HAL_Delay(1);
	//���ù�����
	*P = Oversampling_Adress | OverSamp;
		HAL_Delay(1);
	//��ȡID
	*P = 0x8000 | Name_ID_Adress;
	HAL_Delay(1);
	ID = *P;
		HAL_Delay(1);
	//�ָ�ADCģʽ
	*P = 0x0000;
	
	return ID;
}
