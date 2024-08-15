/*
  ******************************************************************************
  * File Name          : FFT.h
  * Description        : This file provides code for the FFT.
  * Author             : xiaodengwang
  * Date               : 2024-07-05
  ******************************************************************************
*/
#ifndef __FFT_H_
#define __FFT_H_

#ifdef __cplusplus
 extern "C" {
#endif
/*! include headers */
/*!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef ARM_MATH_CM4
#include "arm_math.h"  
#include "arm_const_structs.h"
#else
#include <math.h>
typedef float float32_t;
typedef double float64_t;
#endif

#pragma pack(1)
typedef struct 
{
    float32_t amp;
    float32_t pha;/*! note, should be angle, not radians */
}fft_point_t;
#pragma pack()

/*!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*! Public function list */
int Down(float x);
void FFT_caculate_adc_buff(float32_t* pAdcBuff, uint32_t adcBuffSize, fft_point_t *pResult, uint8_t harmonixNum);
//float32_t* pAdcBuff：指向包含ADC采样数据的缓冲区的指针。这些数据是时间域中的实数序列。
//uint32_t adcBuffSize：ADC缓冲区的大小，表示缓冲区中数据点的数量。在这个例子中，adcBuffSize 被指定为4096，意味着FFT将处理4096个数据点。
//fft_point_t *pResult：指向一个结构或数组的指针，用于存储FFT变换的结果。fft_point_t 可能是一个自定义的类型，用于表示FFT输出的一个点，通常包括频率、幅度和相位信息。
//uint8_t harmonixNum：表示要计算的谐波或FFT输出点的数量。这个值通常小于或等于adcBuffSize。


//输出是 
//static float32_t Amplitude_harmonic[MAX_HARMONIC_NUM];
///static float32_t Phase_harmonic[MAX_HARMONIC_NUM];
//static float32_t Fre_harmonic[MAX_HARMONIC_NUM];
//Amplitude_harmonic：一个浮点数数组，用于存储每个谐波的幅度。
//Phase_harmonic：一个浮点数数组，用于存储每个谐波的相位。
//Fre_harmonic：一个浮点数数组，用于存储每个谐波的频率。


#ifdef __cplusplus
}
#endif

#endif
/*! end of the file */
