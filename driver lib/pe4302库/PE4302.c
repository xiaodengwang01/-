#include "PE4302.h"
//C16 C8 C4 C2 C1 C0.5

void PE4302_Open(void)//将P/E脚置高即可
{
	HAL_GPIO_WritePin(P_S_GPIO_Port, P_S_Pin, SET);


}
void PE4302_Close(void)
{

	HAL_GPIO_WritePin(P_S_GPIO_Port, P_S_Pin, RESET);

}
void PE4302_Set(GPIO_TypeDef* GPIO_CLOCK, uint16_t GPIO_Pin_C,GPIO_TypeDef* GPIO_DATA, uint16_t GPIO_Pin_D,float Cin)//0.5<=Cin<=31.5,step=0.5
{//Cin输入多少，就衰减多少db，第一个好是时钟脚第二个是数据脚。
	uint8_t binary;
	binary=(Cin*2);
	uint8_t a=32;
	HAL_GPIO_WritePin(GPIO_CLOCK, GPIO_Pin_C, RESET);
	HAL_GPIO_WritePin(LE_GPIO_Port, LE_Pin, RESET);

	for(uint8_t i=0;i<6;i++)
	{
		if(binary>=a)
		{
			HAL_GPIO_WritePin(GPIO_DATA, GPIO_Pin_D, SET);
			HAL_GPIO_WritePin(GPIO_CLOCK, GPIO_Pin_C, SET);
			HAL_GPIO_WritePin(GPIO_CLOCK, GPIO_Pin_C, RESET);
			binary-=a;
		}
		else
		{
			HAL_GPIO_WritePin(GPIO_DATA, GPIO_Pin_D, RESET);
			HAL_GPIO_WritePin(GPIO_CLOCK, GPIO_Pin_C, SET);
			HAL_GPIO_WritePin(GPIO_CLOCK, GPIO_Pin_C, RESET);
		}
		a=a/2;
	}
	HAL_GPIO_WritePin(LE_GPIO_Port, LE_Pin, SET);
	HAL_GPIO_WritePin(LE_GPIO_Port, LE_Pin, RESET);
}





























