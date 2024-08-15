/*
  ******************************************************************************
  * File Name          : FFT.c
  * Description        : This file provides code for the FFT Function.
  * Author             : xiaodengwang 
  * Date               : 2024-07-05
  ******************************************************************************
*/
/*! -------------------------------------------------------------------------- */
/*! Include headers */
#include "arm_math.h"
#include "FFT.h"

/*! -------------------------------------------------------------------------- */
/*! Private macros definition */
#define MAX_HARMONIC_NUM    50
#define MAX_ADC_POINTS      4096
#define ADC_FS              20480 
#define FRE_BASIC           50    //基波频率
#define MAX_NEIGHBOR_AREA   2     //邻域数
#define DPI                 (ADC_FS/MAX_ADC_POINTS)    //FFT分辨率
#define MY_ABS(x)           ( ((x)>0) ? (x) : (0 - (x)) )

/*! -------------------------------------------------------------------------- */
/*! Private variable definition */
static float32_t fft_dataBuff[MAX_ADC_POINTS * 2] ;
static float32_t Amplitude_harmonic[MAX_HARMONIC_NUM];
static float32_t Phase_harmonic[MAX_HARMONIC_NUM];
static float32_t Fre_harmonic[MAX_HARMONIC_NUM];

//Amplitude_harmonic：一个浮点数数组，用于存储每个谐波的幅度。
//Phase_harmonic：一个浮点数数组，用于存储每个谐波的相位。
//Fre_harmonic：一个浮点数数组，用于存储每个谐波的频率。
//MAX_HARMONIC_NUM 是定义了数组大小的宏，表示可以分析的最大谐波数量。

/*!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*! Private function list */
static void FFT_data_pre_handle(float32_t* pDataSrc, float32_t* pDataDist, uint32_t dataSize);
//pDataSrc：指向ADC原始数据的指针。
//pDataDist：指向用于存储预处理后数据的数组的指针。
//dataSize：数据的大小，通常是采样点的数量。

static void FFT_cfft_radix4_handle(float32_t* pDataSrc, uint32_t dataSize);
//pDataSrc：指向要进行FFT处理的数据的指针。
//dataSize：数据的大小，必须是2的幂次，因为FFT算法通常使用快速算法处理这种数据。

//FFT_data_pre_handle：一个用于FFT数据预处理的函数，它可能包括窗口函数的乘法、归一化等操作。
//FFT_cfft_radix4_handle：一个用于执行FFT计算的函数，使用复数FFT，基-4（radix-4）算法进行实现。


/*!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*! Public function list */
void FFT_caculate_adc_buff(float32_t* pAdcBuff, uint32_t adcBuffSize, fft_point_t *pResult, uint8_t harmonixNum);

int Down(float x)
{
    int y;
    y=(int)(x);
    return y;
}
//这个函数将浮点数 x 向下取整为整数 y 并返回。
 
/*!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void 
FFT_data_pre_handle(float32_t* pDataSrc, float32_t* pDataDist, uint32_t dataSize) 
{
    for (int iSize = 0; iSize < dataSize; ++iSize) {
        pDataDist[2*iSize]     = pDataSrc[iSize];
        pDataDist[2*iSize + 1] = 0;
    }
	
    for (int iSize = 0; iSize < dataSize; ++iSize) {
        #ifdef ARM_MATH_CM4
            pDataDist[2*iSize] = pDataDist[2*iSize]*0.5*(1-arm_cos_f32(2*PI*iSize/(MAX_ADC_POINTS-1)));
        #else
            pDataDist[2*iSize] = pDataDist[2*iSize]*0.5*(1-cos(2*PI*i/(Sample_Num-1)));		
        #endif
    }
}
//用于FFT数据的预处理。
//它将实数输入数据 pDataSrc 复制到 pDataDist，同时将偶数索引的元素设置为原数据，奇数索引的元素设置为0，为复数FFT输入做准备。

/*!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void 
FFT_cfft_radix4_handle(float32_t* pDataSrc, uint32_t dataSize)
{
    
    /* FFT处理 */
//    #ifndef ARM_MATH_CM4
//        # error "not define ARM_MATH_CM4 in FFT_cfftRadix4Handle"
//    #else
//        memset(fft_dataBuff, 0, sizeof(fft_dataBuff));
    
        arm_cfft_radix4_instance_f32 scfft;
        arm_cfft_radix4_init_f32(&scfft,dataSize,0,1);
        arm_cfft_radix4_f32(&scfft,pDataSrc);
	
//    #endif
}
/*!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


void 
FFT_caculate_adc_buff(float32_t* pAdcBuff, uint32_t adcBuffSize, fft_point_t *pResult, uint8_t harmonixNum) //adcBuffSize=4096
{
    FFT_data_pre_handle(pAdcBuff, fft_dataBuff, adcBuffSize);
		
    FFT_cfft_radix4_handle(fft_dataBuff, adcBuffSize);
   
    float32_t DC = fft_dataBuff[0] / adcBuffSize * 2;  // 直流分量 
    
    float32_t Amp_neighbor[MAX_NEIGHBOR_AREA*2+1];    //邻域幅值数组
    float32_t Pha_neighbor[MAX_NEIGHBOR_AREA*2+1];    //邻域相位数组

    uint32_t  BasicNum;     //谐波谱线的理论下标
    uint8_t   Harmonic_num; //谐波次数
    float32_t Max_amp;
    uint32_t  ka,km;
    float32_t Alpha_m;
    float32_t Delta_km;
    float32_t Amp_XH5_km;  //XH5在km处amp
    float32_t Pha_XH5_km;  //XH5在km处pha
    
    for (uint8_t iHarNum=0; iHarNum < MAX_HARMONIC_NUM; ++iHarNum) {

		if(iHarNum==0){
        
        BasicNum = (iHarNum+1)*FRE_BASIC/DPI;  //基波下标
        
        //邻域内的幅度谱和相位谱
        for (int k = BasicNum - MAX_NEIGHBOR_AREA; k <= BasicNum + MAX_NEIGHBOR_AREA; k++) {     //BasicNum=200    200-8 ~ 200+8  邻域处理
						
		    float XH5_real=fft_dataBuff[2*k]/25920.-(fft_dataBuff[2*(k+1)]+fft_dataBuff[2*(k-1)])/32400.+(fft_dataBuff[2*(k+2)]+fft_dataBuff[2*(k-2)])/64800.-(fft_dataBuff[2*(k+3)]+fft_dataBuff[2*(k-3)])/226800.+(fft_dataBuff[2*(k+4)]+fft_dataBuff[2*(k-4)])/1814400.;            //实部
		    
			float XH5_imag=fft_dataBuff[2*k+1]/25920.-(fft_dataBuff[2*(k+1)+1]+fft_dataBuff[2*(k-1)+1])/32400.+(fft_dataBuff[2*(k+2)+1]+fft_dataBuff[2*(k-2)+1])/64800.-(fft_dataBuff[2*(k+3)+1]+fft_dataBuff[2*(k-3)+1])/226800.+(fft_dataBuff[2*(k+4)+1]+fft_dataBuff[2*(k-4)+1])/1814400.;  //虚部
						
            Amp_neighbor[k - BasicNum + MAX_NEIGHBOR_AREA]=sqrt(XH5_real*XH5_real+XH5_imag*XH5_imag);//幅值 没有加衰减系数
            Pha_neighbor[k - BasicNum + MAX_NEIGHBOR_AREA]=atan2(XH5_imag,XH5_real)/PI*180;          //相位
        }
		       
        //计算中心频率 
        Max_amp = 0;  //比较值
        for (int k = BasicNum - MAX_NEIGHBOR_AREA; k <= BasicNum + MAX_NEIGHBOR_AREA; k++) { //找最大的ka
            if (Amp_neighbor[k - BasicNum + MAX_NEIGHBOR_AREA] > Max_amp) {
                ka = k;
                Max_amp = Amp_neighbor[k - BasicNum + MAX_NEIGHBOR_AREA];
            }
        }
        
        if (Amp_neighbor[ka-1-BasicNum+MAX_NEIGHBOR_AREA] < Amp_neighbor[ka+1-BasicNum+MAX_NEIGHBOR_AREA]) {
            km = ka;
        }
        else { km = ka-1; }
        
        Amp_XH5_km = Amp_neighbor[km-BasicNum+MAX_NEIGHBOR_AREA];  //XH-5(km)幅值            
        Pha_XH5_km = Pha_neighbor[km-BasicNum+MAX_NEIGHBOR_AREA];  //XH-5(km)相位
        Alpha_m    = MY_ABS(Amp_XH5_km)/MY_ABS(Amp_neighbor[km+1-BasicNum+MAX_NEIGHBOR_AREA]); //幅值比 
        Delta_km   = (6-5*Alpha_m)/(1+Alpha_m);                                         //偏移值
	}
		
		else  {
			float XH5_realm,XH5_imagm;
			float temp=Fre_harmonic[0]/DPI*(iHarNum+1); //float temp=Harmonic_num*F_basic/DPI;
			km=Down(temp);                                  
			Delta_km=temp-km;

			XH5_realm=fft_dataBuff[2*km]/25920.-(fft_dataBuff[2*(km+1)]+fft_dataBuff[2*(km-1)])/32400.+(fft_dataBuff[2*(km+2)]+fft_dataBuff[2*(km-2)])/64800.-(fft_dataBuff[2*(km+3)]+fft_dataBuff[2*(km-3)])/226800.+(fft_dataBuff[2*(km+4)]+fft_dataBuff[2*(km-4)])/1814400.;            //实部
			XH5_imagm=fft_dataBuff[2*km+1]/25920.-(fft_dataBuff[2*(km+1)+1]+fft_dataBuff[2*(km-1)+1])/32400.+(fft_dataBuff[2*(km+2)+1]+fft_dataBuff[2*(km-2)+1])/64800.-(fft_dataBuff[2*(km+3)+1]+fft_dataBuff[2*(km-3)+1])/226800.+(fft_dataBuff[2*(km+4)+1]+fft_dataBuff[2*(km-4)+1])/1814400.;  //虚部

			Amp_XH5_km=sqrt(XH5_realm*XH5_realm+XH5_imagm*XH5_imagm);    
			Pha_XH5_km=atan2(XH5_imagm,XH5_realm)/PI*180;
		}
        
        Fre_harmonic[iHarNum]=(km+Delta_km)*DPI;  //频率修正
        float tempVal;
        tempVal = Delta_km*(Delta_km*Delta_km-1)*(Delta_km*Delta_km-4)*(Delta_km*Delta_km-9)*(Delta_km*Delta_km-16)*(Delta_km*Delta_km-25);
        #ifdef ARM_MATH_CM4
            Amplitude_harmonic[iHarNum] = MY_ABS(tempVal)/arm_sin_f32(PI*Delta_km)/adcBuffSize*4*PI*MY_ABS(Amp_XH5_km);
        #else
            Amplitude_harmonic[Harmonic_num] = MY_ABS(tempVal)/sin(PI*Delta_km)/Sample_Num*4*PI*myabs(Amp_XH5_km);          
        #endif
        Phase_harmonic[iHarNum] = Pha_XH5_km - Delta_km*180 + 90;          //相位
    }
    
    for (uint8_t iHarNum = 1; iHarNum < harmonixNum; ++iHarNum) {  //非基波的相位
        Phase_harmonic[iHarNum] -= (iHarNum+1)*Phase_harmonic[0];
	
        while(Phase_harmonic[iHarNum]<=-180||Phase_harmonic[iHarNum]>180) {
            if (Phase_harmonic[iHarNum] <= -180) { Phase_harmonic[iHarNum] += 360; }
            if (Phase_harmonic[iHarNum] >   180) { Phase_harmonic[iHarNum] -= 360; }
        }
    }
    
    Phase_harmonic[0]=0;  //基波相位认定为0
    
    /*!  fft out */
    for(uint8_t iFFTout = 0; iFFTout < harmonixNum; ++iFFTout) {
        pResult[iFFTout].amp = Amplitude_harmonic[iFFTout];
        pResult[iFFTout].pha = Phase_harmonic[iFFTout];
    }
}


