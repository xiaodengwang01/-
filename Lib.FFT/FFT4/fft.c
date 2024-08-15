#include "fft.h"



float32_t fft_outputdata_1[FFT_length] ;
float32_t fft_outputdata_2[FFT_length] ;
float32_t fft_inputbuff_1[FFT_length] ;
float32_t fft_inputbuff_2[FFT_length] ;
float32_t fft_mag_outputdata_1[FFT_halflength] ;
float32_t fft_mag_outputdata_2[FFT_halflength] ;
float32_t fft_frequent_1[FFT_halflength]; 
float32_t fft_frequent_2[FFT_halflength];

float32_t HanNing[FFT_length]={0};                                           //汉宁窗数组
float32_t fft_conv[2*FFT_length-1]={0};                                      //FFT卷积结果

float32_t move1 = 0 ;
float32_t move2 = 0 ;


extern uint32_t buff_0[2*FFT_length-1+8];
extern float32_t buff_1[2*FFT_length-1];
extern float32_t buff_2[2*FFT_length-1];

float aMax =0.0,aSecondMax = 0.0,aThirdMax = 0.0,aFourthMax=0.0;
float fMax =0.0,fSecondMax = 0.0,fThirdMax = 0.0,fFourthMax=0.0;

uint32_t Amax_pos_1 ;
uint32_t Amax_pos_2 ;
float32_t Fmax_1 ;
float32_t Fmax_2 ;
float32_t Phase_1 ;
float32_t Phase_2 ;
float32_t Phase_1minus2 ;

void fft_function(void)
{
	
	arm_rfft_fast_instance_f32 S;
    arm_rfft_fast_init_f32(&S ,FFT_length) ;
    arm_rfft_fast_f32 (&S , fft_inputbuff_1 , fft_outputdata_1 ,0) ; 
    arm_cmplx_mag_f32 (fft_outputdata_1 , fft_mag_outputdata_1 , FFT_length) ;
	
	
    arm_rfft_fast_init_f32(&S ,FFT_length) ;
    arm_rfft_fast_f32 (&S , fft_inputbuff_2 , fft_outputdata_2 ,0) ; 
    arm_cmplx_mag_f32 (fft_outputdata_2 , fft_mag_outputdata_2 , FFT_length) ;
	
    for( int I = 0 ; I < FFT_length  ; I++)
	{
		 //fft_frequent[I] = sample_frequent_low*(I+1) / FFT_length ;
		//fft_frequent[I] = sample_frequent_mid_1*(I+1) / FFT_length ;
		//fft_frequent[I] = sample_frequent_mid_2*(I+1) / FFT_length ;
		fft_frequent_1[I] = sample_frequent_high*(I+1) / FFT_length ;
	}
	
	move1 = fft_mag_outputdata_1[0] /FFT_length;
	move2 = fft_mag_outputdata_2[0] /FFT_length;
	fft_mag_outputdata_1[0] = 0;
    fft_mag_outputdata_2[0] = 0;
	for(int h=0;h<FFT_length;h++)
    {
        fft_mag_outputdata_1[h]/=FFT_length/2;
        fft_mag_outputdata_2[h]/=FFT_length/2;
    }

	arm_max_f32( fft_mag_outputdata_1 , FFT_length/2, &Fmax_1, &Amax_pos_1);     
    arm_max_f32( fft_mag_outputdata_2 , FFT_length/2, &Fmax_2, &Amax_pos_2); 
	
	Phase_1=Phase_atan2(fft_inputbuff_1,Amax_pos_1);
	Phase_2=Phase_atan2(fft_inputbuff_2,Amax_pos_2);
	Phase_1minus2=Phase_1-Phase_2;
    if(Phase_1minus2>180) 
	{
		Phase_1minus2 = -360 + Phase_1minus2;
	}
    else if(Phase_1minus2<-180)
	{
		Phase_1minus2 = 360 + Phase_1minus2;
	}
   // Phase_1minus2+=0.77929f;   1k
	//Phase_1minus2+=7.24581f+0.213321f;  //10k
	  Phase_1minus2+=7.24581f+0.213321f+8.7235851f-0.17f;  //20k
}




//找最大值，次大值……对应的频率，分析波形
void select_max(float *f,float *a )
{
   int i,j;
   float k,k1,m;
   int nMax=0,nSecondMax=0,nThirdMax=0,nFourthMax=0;
	
	
   for ( i = 1; i < FFT_length/2; i++)//i必须是1，是0的话，会把直流分量加进去！！！！
    {
        if (a[i]>aMax)
        {
            aMax = a[i]; 
       nMax=i;
       fMax=f[nMax];
        }
    }
  for ( i=1; i < FFT_length/2; i++)
    {
    if (nMax == i)
    {
      continue;//跳过原来最大值的下标，直接开始i+1的循环
    }
        if (a[i]>aSecondMax&&a[i]>a[i+1]&&a[i]>a[i-1])
        {
            aSecondMax = a[i]; 
            nSecondMax=i;
            fSecondMax=f[nSecondMax];
        }
    }
  for ( i=1; i < FFT_length/2; i++)
    {
    if (nMax == i||nSecondMax==i)
    {
      continue;//跳过原来最大值的下标，直接开始i+1的循环
    }
        if (a[i]>aThirdMax&&a[i]>a[i+1]&&a[i]>a[i-1])
        {
            aThirdMax = a[i]; 
       nThirdMax=i;
       fThirdMax=f[nThirdMax];
        }
    }
  for ( i=1; i < FFT_length/2; i++)
    {
    if (nMax == i||nSecondMax==i||nThirdMax==i)
    {
      continue;//跳过原来最大值的下标，直接开始i+1的循环
    }
        if (a[i]>aFourthMax&&a[i]>a[i+1]&&a[i]>a[i-1])
        {
            aFourthMax = a[i]; 
       nFourthMax=i;
       fFourthMax=f[nFourthMax];
        }
    }
  k=fabs(2*fMax-fSecondMax);
  k1=fabs(3*fMax-fSecondMax);
  m=fabs((float)(aMax-3.0*aSecondMax)); 
  if(k<=5);
    // LCD_ShowString(275,230,12*4,12,12,"JvChi  ");
  else if(k1<=5&&m<0.4)  ;
   //  LCD_ShowString(275,230,12*4,12,12,"Fang   ");
  else if(k1<=5&&m>=0.4) ;
     //LCD_ShowString(275,230,12*4,12,12,"SanJiao");
  else ;
 // LCD_ShowString(275,230,12*4,12,12,"Sin    ");
}






void hanning_window(float *w)   //生成汉宁窗数组
{
    uint16_t n;
    for (n = 0; n < FFT_length; n++)
    {
        w[n] = 0.5 * (1 - cos(2 * PI * n / FFT_length));
    }
}
 
void convolve(float *w, float *y) //卷积
{
    int n, m;
    for (n = 0; n < 2 * FFT_length - 1; n++) {
        y[n] = 0;
        for (m = 0; m < FFT_length; m++) {
            if (n - m >= 0 && n - m < FFT_length) {
                y[n] += w[m] * w[n - m];
            }
        }
    }
}
 

float sum(float *x, int n)  // 计算序列的和
{
    int I;
    float s = 0;
    for (I = 0; I < n; I++) {
        s += x[I];
    }
    return s;
}
 
 
void normalize(float *x, int n)  // 归一化序列
{
    float s = sum(x, n);
    int I;
    for (I = 0; I < n; I++) {
        x[I] /= s;
    }
}


void AP_Prepration_for_FFT(void)
{
 
	hanning_window(HanNing);
    convolve(HanNing,fft_conv);
    normalize(fft_conv,2*FFT_length-1); //生成窗
	
	
    for(int I = 0; I < 2*FFT_length-1; I++)
    {
        buff_1[I] = buff_1[I] * fft_conv[I];    //加窗
        buff_2[I] = buff_2[I] * fft_conv[I];    //加窗
    }

    for(int I = 0; I < FFT_length-1; I++)
    {
        buff_1[I] = buff_1[I] + buff_1[FFT_length+I];
        buff_2[I] = buff_2[I] + buff_2[FFT_length+I];
    }
	
}


//相位计算函数
float Phase_atan2(float32_t *inputSignal,uint32_t index)
{
    if(inputSignal[2*index+1] >= 0 && inputSignal[2*index] >= 0)
        return   0 + atan(inputSignal[2*index+1] / inputSignal[2*index]) / PI * 180;
	
    else if(inputSignal[2*index+1] >= 0 && inputSignal[2*index] <= 0)
        return 180 + atan(inputSignal[2*index+1] / inputSignal[2*index]) / PI * 180;
	
    else if(inputSignal[2*index+1] <= 0 && inputSignal[2*index] <= 0)
        return 180 + atan(inputSignal[2*index+1] / inputSignal[2*index]) / PI * 180;
	
    else if(inputSignal[2*index+1] <= 0 && inputSignal[2*index] >= 0)
        return 360 + atan(inputSignal[2*index+1] / inputSignal[2*index]) / PI * 180;
//	if(inputSignal[index+1] >= 0 && inputSignal[index] >= 0)
//        return   0 + atan(inputSignal[index+1] / inputSignal[index]) / PI * 180;
//	
//    else if(inputSignal[index+1] >= 0 && inputSignal[index] <= 0)
//        return 180 + atan(inputSignal[index+1] / inputSignal[index]) / PI * 180;
//	
//    else if(inputSignal[index+1] <= 0 && inputSignal[index] <= 0)
//        return 180 + atan(inputSignal[index+1] / inputSignal[index]) / PI * 180;
//	
//    else if(inputSignal[index+1] <= 0 && inputSignal[index] >= 0)
//        return 360 + atan(inputSignal[index+1] / inputSignal[index]) / PI * 180;
//	else
//		return 0.0;
	
}










