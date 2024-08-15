
#ifndef INC_PE4302_H_
#define INC_PE4302_H_

#include "main.h"

void PE4302_Open(void);
void PE4302_Close(void);
void PE4302_Set(GPIO_TypeDef* GPIO_CLOCK, uint16_t GPIO_Pin_C,GPIO_TypeDef* GPIO_DATA, uint16_t GPIO_Pin_D,float Cin);//前一个是时钟，后一个是数据，0.5<=Cin<=31.5,step=0.5






#endif /* INC_PE4302_H_ */
