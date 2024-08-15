#ifndef __FFT_H
#define __FFT_H		

#include "main.h"
#include <stdio.h>
#include "arm_math.h" 
#include "arm_const_structs.h"

#define FFT_length  4096
#define FFT_halflength FFT_length/2

#define sample_frequent_low 25.600001257 //KHZ
#define sample_frequent_mid_1 42.666667  
#define sample_frequent_mid_2 853.33334
#define sample_frequent_high 265.97402124721



extern float aMax ;
extern float aSecondMax ;
extern float aThirdMax ;
extern float aFourthMax ;
extern float fMax ;
extern float fSecondMax ;
extern float fThirdMax ;
extern float fFourthMax ;


extern float32_t fft_outputdata_1[FFT_length] ;
extern float32_t fft_outputdata_2[FFT_length] ;
extern float32_t fft_inputbuff_1[FFT_length] ;
extern float32_t fft_inputbuff_2[FFT_length] ;


extern float32_t fft_mag_outputdata_1[FFT_halflength] ;
extern float32_t fft_mag_outputdata_2[FFT_halflength] ;
extern float32_t fft_frequent_1[FFT_halflength]; 
extern float32_t fft_frequent_2[FFT_halflength]; 

extern uint32_t Amax_pos_1 ;
extern uint32_t Amax_pos_2 ;
extern float32_t Fmax_1 ;
extern float32_t Fmax_2 ;

void fft_function(void) ;
void select_max(float *f,float *a );
void AP_Prepration_for_FFT(void);
float Phase_atan2(float32_t *inputSignal,uint32_t index);


#endif  
