#include "ads1274.h"
void ads1274_init(void){
			ADS1274_SYNC_HIGH;
	ADS1274_SYNC_LOW;
	ADS1274_SYNC_HIGH;

}

void ads1274_Data(uint16_t num,uint32_t* dat1,uint32_t* dat2,uint32_t* dat3,uint32_t* dat4)
{

	uint16_t i;
	int state = 0;
	for (i=0;i<num;i++)
	{
		dat1[i]=0;
		dat2[i]=0;
		//dat3[i]=0;
		//dat4[i]=0
	}
		i = 0;
		while(i < num){
			while(ADS1274_DRDY_VALUE==1){
				if(state == 0){
						state = state + 1;
				}
			}
			if(state != 0){
				for(uint8_t j=0;j<(24);j++){
					ADS1274_SCLK_HIGH;
					dat1[i]=(dat1[i]<<1)+ADS1274_DOUT1_VALUE;
					dat2[i]=(dat2[i]<<1)+ADS1274_DOUT2_VALUE;
					//dat3[i]=(dat3[i]<<1)+GPIO_Read(device->DOUT3);
					//dat4[i]=(dat4[i]<<1)+GPIO_Read(device->DOUT4);
					ADS1274_SCLK_LOW;
				}
				
			i++;
			
		}
	}
}


void 	Method_Data_Binary_to_Float(uint32_t* dat_before,float* dat_after,uint16_t num)
{
	uint16_t i;
	for(i=0;i<num;i++)
	{
		if(dat_before[i]<=0x7fffff)
		{
			dat_before[i]=dat_before[i]+0x800000;
		}
		else
		{
			dat_before[i]=dat_before[i]-0x800000;
		}

		dat_after[i]=(2.50-(dat_before[i]*5.00/16777215.00))*1000;
	}
}





















