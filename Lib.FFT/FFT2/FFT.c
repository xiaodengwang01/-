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
#define FRE_BASIC           50    //����Ƶ��
#define MAX_NEIGHBOR_AREA   2     //������
#define DPI                 (ADC_FS/MAX_ADC_POINTS)    //FFT�ֱ���
#define MY_ABS(x)           ( ((x)>0) ? (x) : (0 - (x)) )

/*! -------------------------------------------------------------------------- */
/*! Private variable definition */
static float32_t fft_dataBuff[MAX_ADC_POINTS * 2] ;
static float32_t Amplitude_harmonic[MAX_HARMONIC_NUM];
static float32_t Phase_harmonic[MAX_HARMONIC_NUM];
static float32_t Fre_harmonic[MAX_HARMONIC_NUM];

//Amplitude_harmonic��һ�����������飬���ڴ洢ÿ��г���ķ��ȡ�
//Phase_harmonic��һ�����������飬���ڴ洢ÿ��г������λ��
//Fre_harmonic��һ�����������飬���ڴ洢ÿ��г����Ƶ�ʡ�
//MAX_HARMONIC_NUM �Ƕ����������С�ĺ꣬��ʾ���Է��������г��������

/*!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*! Private function list */
static void FFT_data_pre_handle(float32_t* pDataSrc, float32_t* pDataDist, uint32_t dataSize);
//pDataSrc��ָ��ADCԭʼ���ݵ�ָ�롣
//pDataDist��ָ�����ڴ洢Ԥ��������ݵ������ָ�롣
//dataSize�����ݵĴ�С��ͨ���ǲ������������

static void FFT_cfft_radix4_handle(float32_t* pDataSrc, uint32_t dataSize);
//pDataSrc��ָ��Ҫ����FFT��������ݵ�ָ�롣
//dataSize�����ݵĴ�С��������2���ݴΣ���ΪFFT�㷨ͨ��ʹ�ÿ����㷨�����������ݡ�

//FFT_data_pre_handle��һ������FFT����Ԥ����ĺ����������ܰ������ں����ĳ˷�����һ���Ȳ�����
//FFT_cfft_radix4_handle��һ������ִ��FFT����ĺ�����ʹ�ø���FFT����-4��radix-4���㷨����ʵ�֡�


/*!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*! Public function list */
void FFT_caculate_adc_buff(float32_t* pAdcBuff, uint32_t adcBuffSize, fft_point_t *pResult, uint8_t harmonixNum);

int Down(float x)
{
    int y;
    y=(int)(x);
    return y;
}
//��������������� x ����ȡ��Ϊ���� y �����ء�
 
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
//����FFT���ݵ�Ԥ����
//����ʵ���������� pDataSrc ���Ƶ� pDataDist��ͬʱ��ż��������Ԫ������Ϊԭ���ݣ�����������Ԫ������Ϊ0��Ϊ����FFT������׼����

/*!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
static void 
FFT_cfft_radix4_handle(float32_t* pDataSrc, uint32_t dataSize)
{
    
    /* FFT���� */
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
   
    float32_t DC = fft_dataBuff[0] / adcBuffSize * 2;  // ֱ������ 
    
    float32_t Amp_neighbor[MAX_NEIGHBOR_AREA*2+1];    //�����ֵ����
    float32_t Pha_neighbor[MAX_NEIGHBOR_AREA*2+1];    //������λ����

    uint32_t  BasicNum;     //г�����ߵ������±�
    uint8_t   Harmonic_num; //г������
    float32_t Max_amp;
    uint32_t  ka,km;
    float32_t Alpha_m;
    float32_t Delta_km;
    float32_t Amp_XH5_km;  //XH5��km��amp
    float32_t Pha_XH5_km;  //XH5��km��pha
    
    for (uint8_t iHarNum=0; iHarNum < MAX_HARMONIC_NUM; ++iHarNum) {

		if(iHarNum==0){
        
        BasicNum = (iHarNum+1)*FRE_BASIC/DPI;  //�����±�
        
        //�����ڵķ����׺���λ��
        for (int k = BasicNum - MAX_NEIGHBOR_AREA; k <= BasicNum + MAX_NEIGHBOR_AREA; k++) {     //BasicNum=200    200-8 ~ 200+8  ������
						
		    float XH5_real=fft_dataBuff[2*k]/25920.-(fft_dataBuff[2*(k+1)]+fft_dataBuff[2*(k-1)])/32400.+(fft_dataBuff[2*(k+2)]+fft_dataBuff[2*(k-2)])/64800.-(fft_dataBuff[2*(k+3)]+fft_dataBuff[2*(k-3)])/226800.+(fft_dataBuff[2*(k+4)]+fft_dataBuff[2*(k-4)])/1814400.;            //ʵ��
		    
			float XH5_imag=fft_dataBuff[2*k+1]/25920.-(fft_dataBuff[2*(k+1)+1]+fft_dataBuff[2*(k-1)+1])/32400.+(fft_dataBuff[2*(k+2)+1]+fft_dataBuff[2*(k-2)+1])/64800.-(fft_dataBuff[2*(k+3)+1]+fft_dataBuff[2*(k-3)+1])/226800.+(fft_dataBuff[2*(k+4)+1]+fft_dataBuff[2*(k-4)+1])/1814400.;  //�鲿
						
            Amp_neighbor[k - BasicNum + MAX_NEIGHBOR_AREA]=sqrt(XH5_real*XH5_real+XH5_imag*XH5_imag);//��ֵ û�м�˥��ϵ��
            Pha_neighbor[k - BasicNum + MAX_NEIGHBOR_AREA]=atan2(XH5_imag,XH5_real)/PI*180;          //��λ
        }
		       
        //��������Ƶ�� 
        Max_amp = 0;  //�Ƚ�ֵ
        for (int k = BasicNum - MAX_NEIGHBOR_AREA; k <= BasicNum + MAX_NEIGHBOR_AREA; k++) { //������ka
            if (Amp_neighbor[k - BasicNum + MAX_NEIGHBOR_AREA] > Max_amp) {
                ka = k;
                Max_amp = Amp_neighbor[k - BasicNum + MAX_NEIGHBOR_AREA];
            }
        }
        
        if (Amp_neighbor[ka-1-BasicNum+MAX_NEIGHBOR_AREA] < Amp_neighbor[ka+1-BasicNum+MAX_NEIGHBOR_AREA]) {
            km = ka;
        }
        else { km = ka-1; }
        
        Amp_XH5_km = Amp_neighbor[km-BasicNum+MAX_NEIGHBOR_AREA];  //XH-5(km)��ֵ            
        Pha_XH5_km = Pha_neighbor[km-BasicNum+MAX_NEIGHBOR_AREA];  //XH-5(km)��λ
        Alpha_m    = MY_ABS(Amp_XH5_km)/MY_ABS(Amp_neighbor[km+1-BasicNum+MAX_NEIGHBOR_AREA]); //��ֵ�� 
        Delta_km   = (6-5*Alpha_m)/(1+Alpha_m);                                         //ƫ��ֵ
	}
		
		else  {
			float XH5_realm,XH5_imagm;
			float temp=Fre_harmonic[0]/DPI*(iHarNum+1); //float temp=Harmonic_num*F_basic/DPI;
			km=Down(temp);                                  
			Delta_km=temp-km;

			XH5_realm=fft_dataBuff[2*km]/25920.-(fft_dataBuff[2*(km+1)]+fft_dataBuff[2*(km-1)])/32400.+(fft_dataBuff[2*(km+2)]+fft_dataBuff[2*(km-2)])/64800.-(fft_dataBuff[2*(km+3)]+fft_dataBuff[2*(km-3)])/226800.+(fft_dataBuff[2*(km+4)]+fft_dataBuff[2*(km-4)])/1814400.;            //ʵ��
			XH5_imagm=fft_dataBuff[2*km+1]/25920.-(fft_dataBuff[2*(km+1)+1]+fft_dataBuff[2*(km-1)+1])/32400.+(fft_dataBuff[2*(km+2)+1]+fft_dataBuff[2*(km-2)+1])/64800.-(fft_dataBuff[2*(km+3)+1]+fft_dataBuff[2*(km-3)+1])/226800.+(fft_dataBuff[2*(km+4)+1]+fft_dataBuff[2*(km-4)+1])/1814400.;  //�鲿

			Amp_XH5_km=sqrt(XH5_realm*XH5_realm+XH5_imagm*XH5_imagm);    
			Pha_XH5_km=atan2(XH5_imagm,XH5_realm)/PI*180;
		}
        
        Fre_harmonic[iHarNum]=(km+Delta_km)*DPI;  //Ƶ������
        float tempVal;
        tempVal = Delta_km*(Delta_km*Delta_km-1)*(Delta_km*Delta_km-4)*(Delta_km*Delta_km-9)*(Delta_km*Delta_km-16)*(Delta_km*Delta_km-25);
        #ifdef ARM_MATH_CM4
            Amplitude_harmonic[iHarNum] = MY_ABS(tempVal)/arm_sin_f32(PI*Delta_km)/adcBuffSize*4*PI*MY_ABS(Amp_XH5_km);
        #else
            Amplitude_harmonic[Harmonic_num] = MY_ABS(tempVal)/sin(PI*Delta_km)/Sample_Num*4*PI*myabs(Amp_XH5_km);          
        #endif
        Phase_harmonic[iHarNum] = Pha_XH5_km - Delta_km*180 + 90;          //��λ
    }
    
    for (uint8_t iHarNum = 1; iHarNum < harmonixNum; ++iHarNum) {  //�ǻ�������λ
        Phase_harmonic[iHarNum] -= (iHarNum+1)*Phase_harmonic[0];
	
        while(Phase_harmonic[iHarNum]<=-180||Phase_harmonic[iHarNum]>180) {
            if (Phase_harmonic[iHarNum] <= -180) { Phase_harmonic[iHarNum] += 360; }
            if (Phase_harmonic[iHarNum] >   180) { Phase_harmonic[iHarNum] -= 360; }
        }
    }
    
    Phase_harmonic[0]=0;  //������λ�϶�Ϊ0
    
    /*!  fft out */
    for(uint8_t iFFTout = 0; iFFTout < harmonixNum; ++iFFTout) {
        pResult[iFFTout].amp = Amplitude_harmonic[iFFTout];
        pResult[iFFTout].pha = Phase_harmonic[iFFTout];
    }
}


