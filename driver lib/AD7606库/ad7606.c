#include "main.h"
#include "ad7606.h"

extern uint16_t flag;
extern SPI_HandleTypeDef hspi2;

void AD7606_Init(void)//ad7606初始化函数
{	
	HAL_GPIO_WritePin(AD_OS0_GPIO_Port, AD_OS0_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(AD_OS1_GPIO_Port, AD_OS1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AD_OS2_GPIO_Port, AD_OS2_Pin, GPIO_PIN_RESET);//设置过采样为无
	
	HAL_GPIO_WritePin(AD_RANGE_GPIO_Port, AD_RANGE_Pin, GPIO_PIN_RESET);//设置采样范围为-5v-+5v
	
  HAL_GPIO_WritePin(AD_CS_GPIO_Port, AD_CS_Pin, GPIO_PIN_SET);//拉高cs引脚 锁定芯片状态 此时不能进行数据传输
  //HAL_GPIO_WritePin(AD_CA_GPIO_Port, AD_CA_Pin, GPIO_PIN_SET);//将ca置为高电平 此时转换未开始 本题中 只采用a通道 cb恒定接入高电平
	//HAL_GPIO_WritePin(AD_CB_GPIO_Port, AD_CB_Pin, GPIO_PIN_SET);//固定cb的电平 后续不再改变
  HAL_GPIO_WritePin(AD_RESET_GPIO_Port, AD_RESET_Pin, GPIO_PIN_RESET);//将reset引脚拉低
	
	
  // 复位 AD7606
  HAL_GPIO_WritePin(AD_RESET_GPIO_Port, AD_RESET_Pin, GPIO_PIN_SET);//拉低reset 此时表示复位芯片
  HAL_Delay(1);//必要
  HAL_GPIO_WritePin(AD_RESET_GPIO_Port, AD_RESET_Pin, GPIO_PIN_RESET);//再拉低reset 芯片进入正常状态
}

void AD7606_ReadData(void) //该函数 在busy为低电平 即芯片处于非转换状态时 可以运作 
{
  uint16_t result;

  // 读取数据
  HAL_GPIO_WritePin(AD_CS_GPIO_Port, AD_CS_Pin, GPIO_PIN_RESET);//拉低cs电平 此时才可以开始读取数据

  HAL_SPI_Receive(&hspi2,(uint8_t *)dataaaa,2,10000);

  HAL_GPIO_WritePin(AD_CS_GPIO_Port, AD_CS_Pin, GPIO_PIN_SET);//数据读取完毕后 拉高cs 此时不能进行数据传输
  // 将接收到的字节数据转换为16位结果
	//result = ((uint16_t)dataaaa[0] << 8) | dataaaa[1];
}

void movedata1(uint16_t q){//将单次的数据存进数组中 该过程持续进行 不受任何中断影响
	 AD7606_ReadData();//将单次采样的数据存入该数组
	 data1[q*2]=dataaaa[0];//spi接收数组 数组第一位表示1通道的数据 给其赋值到data1数组的偶数位
	 data1[q*2+1]=dataaaa[1];//spi接收数组 数组第二位表示1通道的数据 给其赋值到data2数组的奇数位
}

