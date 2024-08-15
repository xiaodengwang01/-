#include "main.h"
#include "ad7606.h"

extern uint16_t flag;
extern SPI_HandleTypeDef hspi2;

void AD7606_Init(void)//ad7606��ʼ������
{	
	HAL_GPIO_WritePin(AD_OS0_GPIO_Port, AD_OS0_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(AD_OS1_GPIO_Port, AD_OS1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AD_OS2_GPIO_Port, AD_OS2_Pin, GPIO_PIN_RESET);//���ù�����Ϊ��
	
	HAL_GPIO_WritePin(AD_RANGE_GPIO_Port, AD_RANGE_Pin, GPIO_PIN_RESET);//���ò�����ΧΪ-5v-+5v
	
  HAL_GPIO_WritePin(AD_CS_GPIO_Port, AD_CS_Pin, GPIO_PIN_SET);//����cs���� ����оƬ״̬ ��ʱ���ܽ������ݴ���
  //HAL_GPIO_WritePin(AD_CA_GPIO_Port, AD_CA_Pin, GPIO_PIN_SET);//��ca��Ϊ�ߵ�ƽ ��ʱת��δ��ʼ ������ ֻ����aͨ�� cb�㶨����ߵ�ƽ
	//HAL_GPIO_WritePin(AD_CB_GPIO_Port, AD_CB_Pin, GPIO_PIN_SET);//�̶�cb�ĵ�ƽ �������ٸı�
  HAL_GPIO_WritePin(AD_RESET_GPIO_Port, AD_RESET_Pin, GPIO_PIN_RESET);//��reset��������
	
	
  // ��λ AD7606
  HAL_GPIO_WritePin(AD_RESET_GPIO_Port, AD_RESET_Pin, GPIO_PIN_SET);//����reset ��ʱ��ʾ��λоƬ
  HAL_Delay(1);//��Ҫ
  HAL_GPIO_WritePin(AD_RESET_GPIO_Port, AD_RESET_Pin, GPIO_PIN_RESET);//������reset оƬ��������״̬
}

void AD7606_ReadData(void) //�ú��� ��busyΪ�͵�ƽ ��оƬ���ڷ�ת��״̬ʱ �������� 
{
  uint16_t result;

  // ��ȡ����
  HAL_GPIO_WritePin(AD_CS_GPIO_Port, AD_CS_Pin, GPIO_PIN_RESET);//����cs��ƽ ��ʱ�ſ��Կ�ʼ��ȡ����

  HAL_SPI_Receive(&hspi2,(uint8_t *)dataaaa,2,10000);

  HAL_GPIO_WritePin(AD_CS_GPIO_Port, AD_CS_Pin, GPIO_PIN_SET);//���ݶ�ȡ��Ϻ� ����cs ��ʱ���ܽ������ݴ���
  // �����յ����ֽ�����ת��Ϊ16λ���
	//result = ((uint16_t)dataaaa[0] << 8) | dataaaa[1];
}

void movedata1(uint16_t q){//�����ε����ݴ�������� �ù��̳������� �����κ��ж�Ӱ��
	 AD7606_ReadData();//�����β��������ݴ��������
	 data1[q*2]=dataaaa[0];//spi�������� �����һλ��ʾ1ͨ�������� ���丳ֵ��data1�����ż��λ
	 data1[q*2+1]=dataaaa[1];//spi�������� ����ڶ�λ��ʾ1ͨ�������� ���丳ֵ��data2���������λ
}

