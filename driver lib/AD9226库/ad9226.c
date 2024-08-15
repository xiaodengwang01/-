#include "ad9226.h"

uint16_t data_bit=0;//adc���β���������
uint16_t data1[4096];//����λ����ֻ��uint16_t����float 



//����12��adc�����������ţ���Ӧ��λ����λ��d0��d11 
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} GPIO_PinDef;

// ����12��Adc�����������ż����Ӧ�Ķ˿� ��Ӧ��λ����λ��d0��d11
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

uint16_t getonedata(void){ //��ȡ���β�����ֵ
	data_bit = 0; // �������

        // ��λ��ȡGPIO���ţ�����ϳ�һ��12λ������
        for (int j = 0; j < 12; j++)
        {
            // ��ȡ����״̬����������õ���ȷ��λ
            data_bit |= (HAL_GPIO_ReadPin(ADCPins[j].port, ADCPins[j].pin) << j);//������λ�� �ӵ͵��߲���
        }
		return data_bit;
}

void movedata1(uint16_t i){//�����ε����ݴ�������� �ù��̳������� �����κ��ж�Ӱ��
		data1[i]=getonedata();//�����β��������ݴ��������
}
//ԭʼ���ݴ�����data1[0]��data1[4095] 


//ֱ�������жϲɼ����� 
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//	 if (htim->Instance == TIM4){
//				movedata1(i);
// 				printf("%d\r\n",data1[i]);  //��ӡ���� ���0�ǿ��ܻ�û�ɼ������� ����printf("%d\r\n",data_bit); 
//				i++;//ÿ�ζ�ʱ��2�ж�ʱ ����adc�Ѿ�����һ�� �����һ�����ݴ���
//			if(i==4096) i=0;//���ۼӵ�4096������
//		}
//}





//-----------------------------------------------------------------
// End Of File
//----------------------------------------------------------------- 
