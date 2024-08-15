#include "main.h"
#include <stdlib.h>
#include "arm_math.h"
#include <stdio.h>

#define datelen1 4096 //宏定义 定义第一个adc的数据长度为4096
#define datelen2 4096 //宏定义 定义第二个adc的数据长度为4096

uint16_t data_bit=0;//adc单次采样的数据
uint16_t data1[datelen1];
arm_cfft_radix4_instance_f32 cfft_instance;

float voltage_pre[datelen1];
float voltage_next[datelen2];

float * v1=voltage_pre;//定义指向放大前的波形采样数据数组的指针,指针指向放大前的电压数组
float * v2=voltage_next;//定义指向放大后的波形采样数据数组的指针,指针指向放大后的电压数组

float voltage_backpre[datelen1*2];
float voltage_backnext[datelen2*2];

float mo1[datelen1];
float mo2[datelen2];

float amp1,amp2;//采样1的幅值和采样2的幅值
float f1,f2;//采样1的频率和采样2的频率
float max,min;
float indicate=0;//波形类型指示函数 为1代表正弦波 为2代表三角波 为3代表方波
uint32_t m1=0,m2=0;;
float high;//两个波形高电平的值
float low;//两个波形低电平的值

void data_pros1(float * buf)	//数据处理函数 
	{	
			
	for(int qq=0;qq<datelen1;qq++)	{	
			//当上一轮数据处理完成后 才能进行新一轮的赋值
			buf[qq] = data1[qq]*0.24-5;
			//voltage_backpre[ii*2]=buf[ii];//实部赋值
			//voltage_backpre[ii*2+1]=0;//虚部赋值				
	}    
			//amp1=get_amp(buf);
			//arm_cfft_radix4_init_f32(&cfft_instance, datelen1, 0, 1);
			//arm_cfft_radix4_f32(&cfft_instance, voltage_backpre);//对back1进行快速傅里叶变换 得到的结果 按照复数结构体的形式 存储在back1中
			//arm_cmplx_mag_f32(voltage_backpre,&mo1[0],datelen1);//求模 然后存储在mo1数组的第一行 这样就得到了模
			//m1=findMax1(mo1);
			//analyzeWaveform(v1);
			//f1=m1*12.207;//单位是hz
			 
}

int findMax1(float *array ) {//找最大数据的下标
    // 初始化最小值和最大值为数组的第一个元素
    float max1 = array[1];
    int m,i;
    for (i = 1; i < datelen1/2; i++) {
        if (array[i] > max1) {
           max1 = array[i];
					 m=i;
        }  
    }
		return m;
}

float sumMax20(float *array, int length) {//求最大的20个数平均值
    float temp;
		float arr[length];//定义一个新的中间数组 防止改变原数组的值
		for(int q =0;q<length;q++){//将中间数组arr赋值
			arr[q]=array[q];
		}
		
    // 使用选择排序算法找出前十个最大的数 将每个数与后面的全部数进行比较 找出大数
    for (int i = 0; i < 20; i++) {
        for (int j = i + 1; j < length; j++) {
            if (arr[j] > arr[i]) {
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }

    // 计算前20个最大的数之和
    float sum = 0;
    for (int i = 0; i < 20; i++) {
        sum += arr[i];
    }
		float ave=sum/20;
    return ave;
}


float sumMin20(float *array, int length) {//求最小的20个数的平均值
		float temp;
		float arr[length];//定义一个新的中间数组 防止改变原数组的值
		for(int q =0;q<length;q++){//将中间数组arr赋值
			arr[q]=array[q];
		}
		 for (int i = 0; i < 20; i++) {
     for (int j = i + 1; j < length; j++) {
            if (arr[j] < arr[i]) {
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }

    // 计算前十个最小的数之和
    float sum = 0;
    for (int i = 0; i < 20; i++) {
        sum += arr[i];
    }
		float ave=sum/20;
    return ave;
 
}


float get_amp(float *buf){
		float amp;//定义幅值
		high=sumMax20(buf,datelen1);
		low=sumMin20(buf,datelen1);
		amp=high-low;
		return amp;
}


int analyzeWaveform(float *v) {//判断波形类型函数 1代表正弦波 2代表三角波 3代表方波
		float yuzhi=0.75*high+0.25*low;//阈值在高处1/4处
		float count=0;//点数变量 代表超过阈值的点数
		int i;//循环变量
		for(i=0;i<datelen1;i++){
			if(v[i]>=yuzhi) count++;
		}
		float q=count/4096;//q用于表示比值
		if(count==4096){
		count=0;
		}
		if(q>0.24&&q<0.26) {
					indicate=2;//indicate为2表示三角波波
					//printf("type.txt=\"三角波\"\xff\xff\xff");
		}
		else if(q>0.32&&q<0.34) {
					indicate=1;//indicate为1表示正弦波
					//printf("type.txt=\"正弦波\"\xff\xff\xff");
		}
		else if (q>0.49&&q<0.51){
					indicate=3;//indicate为3表示方波
					//printf("type.txt=\"方波\"\xff\xff\xff");
		} // 根据谐波特征判断波形类型
}
