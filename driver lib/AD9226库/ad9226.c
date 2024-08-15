#include "ad9226.h"

uint16_t data_bit=0;//adc单次采样的数据
uint16_t data1[4096];//后面位运算只能uint16_t不能float 



//定义12个adc数据输入引脚，对应低位到高位即d0到d11 
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} GPIO_PinDef;

// 定义12个Adc数据输入引脚及其对应的端口 对应低位到高位即d0到d11
GPIO_PinDef ADCPins[12] = {
    {GPIOE, GPIO_PIN_9},
    {GPIOE, GPIO_PIN_13},
    {GPIOE, GPIO_PIN_8},
    {GPIOE, GPIO_PIN_14},
    {GPIOE, GPIO_PIN_7},
    {GPIOE, GPIO_PIN_15},
    {GPIOD, GPIO_PIN_1},
    {GPIOD, GPIO_PIN_8},
    {GPIOD, GPIO_PIN_0},
    {GPIOD, GPIO_PIN_9},
    {GPIOD, GPIO_PIN_15},
    {GPIOD, GPIO_PIN_10}
};

uint16_t getonedata(void){ //获取单次采样的值
	data_bit = 0; // 清空数据

        // 逐位读取GPIO引脚，并组合成一个12位的整数
        for (int j = 0; j < 12; j++)
        {
            // 读取引脚状态，并将其放置到正确的位
            data_bit |= (HAL_GPIO_ReadPin(ADCPins[j].port, ADCPins[j].pin) << j);//采用移位法 从低到高采样
        }
		return data_bit;
}

void movedata1(uint16_t i){//将单次的数据存进数组中 该过程持续进行 不受任何中断影响
		data1[i]=getonedata();//将单次采样的数据存入该数组
}
//原始数据储存在data1[0]到data1[4095] 


//直接这样中断采集数据 
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//	 if (htim->Instance == TIM4){
//				movedata1(i);
// 				printf("%d\r\n",data1[i]);  //打印数据 输出0是可能还没采集到数据 可用printf("%d\r\n",data_bit); 
//				i++;//每次定时器2中断时 代表adc已经触发一次 则进行一次数据处理
//			if(i==4096) i=0;//在累加到4096后重置
//		}
//}





//-----------------------------------------------------------------
// End Of File
//----------------------------------------------------------------- 
