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
//float32_t* pAdcBuff��ָ�����ADC�������ݵĻ�������ָ�롣��Щ������ʱ�����е�ʵ�����С�
//uint32_t adcBuffSize��ADC�������Ĵ�С����ʾ�����������ݵ������������������У�adcBuffSize ��ָ��Ϊ4096����ζ��FFT������4096�����ݵ㡣
//fft_point_t *pResult��ָ��һ���ṹ�������ָ�룬���ڴ洢FFT�任�Ľ����fft_point_t ������һ���Զ�������ͣ����ڱ�ʾFFT�����һ���㣬ͨ������Ƶ�ʡ����Ⱥ���λ��Ϣ��
//uint8_t harmonixNum����ʾҪ�����г����FFT���������������ֵͨ��С�ڻ����adcBuffSize��


//����� 
//static float32_t Amplitude_harmonic[MAX_HARMONIC_NUM];
///static float32_t Phase_harmonic[MAX_HARMONIC_NUM];
//static float32_t Fre_harmonic[MAX_HARMONIC_NUM];
//Amplitude_harmonic��һ�����������飬���ڴ洢ÿ��г���ķ��ȡ�
//Phase_harmonic��һ�����������飬���ڴ洢ÿ��г������λ��
//Fre_harmonic��һ�����������飬���ڴ洢ÿ��г����Ƶ�ʡ�


#ifdef __cplusplus
}
#endif

#endif
/*! end of the file */
