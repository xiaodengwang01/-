#include "main.h"
#include <stdlib.h>
#include "arm_math.h"
#include <stdio.h>

#define datelen1 4096 //�궨�� �����һ��adc�����ݳ���Ϊ4096
#define datelen2 4096 //�궨�� ����ڶ���adc�����ݳ���Ϊ4096

uint16_t data_bit=0;//adc���β���������
uint16_t data1[datelen1];
arm_cfft_radix4_instance_f32 cfft_instance;

float voltage_pre[datelen1];
float voltage_next[datelen2];

float * v1=voltage_pre;//����ָ��Ŵ�ǰ�Ĳ��β������������ָ��,ָ��ָ��Ŵ�ǰ�ĵ�ѹ����
float * v2=voltage_next;//����ָ��Ŵ��Ĳ��β������������ָ��,ָ��ָ��Ŵ��ĵ�ѹ����

float voltage_backpre[datelen1*2];
float voltage_backnext[datelen2*2];

float mo1[datelen1];
float mo2[datelen2];

float amp1,amp2;//����1�ķ�ֵ�Ͳ���2�ķ�ֵ
float f1,f2;//����1��Ƶ�ʺͲ���2��Ƶ��
float max,min;
float indicate=0;//��������ָʾ���� Ϊ1�������Ҳ� Ϊ2�������ǲ� Ϊ3������
uint32_t m1=0,m2=0;;
float high;//�������θߵ�ƽ��ֵ
float low;//�������ε͵�ƽ��ֵ

void data_pros1(float * buf)	//���ݴ����� 
	{	
			
	for(int qq=0;qq<datelen1;qq++)	{	
			//����һ�����ݴ�����ɺ� ���ܽ�����һ�ֵĸ�ֵ
			buf[qq] = data1[qq]*0.24-5;
			//voltage_backpre[ii*2]=buf[ii];//ʵ����ֵ
			//voltage_backpre[ii*2+1]=0;//�鲿��ֵ				
	}    
			//amp1=get_amp(buf);
			//arm_cfft_radix4_init_f32(&cfft_instance, datelen1, 0, 1);
			//arm_cfft_radix4_f32(&cfft_instance, voltage_backpre);//��back1���п��ٸ���Ҷ�任 �õ��Ľ�� ���ո����ṹ�����ʽ �洢��back1��
			//arm_cmplx_mag_f32(voltage_backpre,&mo1[0],datelen1);//��ģ Ȼ��洢��mo1����ĵ�һ�� �����͵õ���ģ
			//m1=findMax1(mo1);
			//analyzeWaveform(v1);
			//f1=m1*12.207;//��λ��hz
			 
}

int findMax1(float *array ) {//��������ݵ��±�
    // ��ʼ����Сֵ�����ֵΪ����ĵ�һ��Ԫ��
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

float sumMax20(float *array, int length) {//������20����ƽ��ֵ
    float temp;
		float arr[length];//����һ���µ��м����� ��ֹ�ı�ԭ�����ֵ
		for(int q =0;q<length;q++){//���м�����arr��ֵ
			arr[q]=array[q];
		}
		
    // ʹ��ѡ�������㷨�ҳ�ǰʮ�������� ��ÿ����������ȫ�������бȽ� �ҳ�����
    for (int i = 0; i < 20; i++) {
        for (int j = i + 1; j < length; j++) {
            if (arr[j] > arr[i]) {
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }

    // ����ǰ20��������֮��
    float sum = 0;
    for (int i = 0; i < 20; i++) {
        sum += arr[i];
    }
		float ave=sum/20;
    return ave;
}


float sumMin20(float *array, int length) {//����С��20������ƽ��ֵ
		float temp;
		float arr[length];//����һ���µ��м����� ��ֹ�ı�ԭ�����ֵ
		for(int q =0;q<length;q++){//���м�����arr��ֵ
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

    // ����ǰʮ����С����֮��
    float sum = 0;
    for (int i = 0; i < 20; i++) {
        sum += arr[i];
    }
		float ave=sum/20;
    return ave;
 
}


float get_amp(float *buf){
		float amp;//�����ֵ
		high=sumMax20(buf,datelen1);
		low=sumMin20(buf,datelen1);
		amp=high-low;
		return amp;
}


int analyzeWaveform(float *v) {//�жϲ������ͺ��� 1�������Ҳ� 2�������ǲ� 3������
		float yuzhi=0.75*high+0.25*low;//��ֵ�ڸߴ�1/4��
		float count=0;//�������� ��������ֵ�ĵ���
		int i;//ѭ������
		for(i=0;i<datelen1;i++){
			if(v[i]>=yuzhi) count++;
		}
		float q=count/4096;//q���ڱ�ʾ��ֵ
		if(count==4096){
		count=0;
		}
		if(q>0.24&&q<0.26) {
					indicate=2;//indicateΪ2��ʾ���ǲ���
					//printf("type.txt=\"���ǲ�\"\xff\xff\xff");
		}
		else if(q>0.32&&q<0.34) {
					indicate=1;//indicateΪ1��ʾ���Ҳ�
					//printf("type.txt=\"���Ҳ�\"\xff\xff\xff");
		}
		else if (q>0.49&&q<0.51){
					indicate=3;//indicateΪ3��ʾ����
					//printf("type.txt=\"����\"\xff\xff\xff");
		} // ����г�������жϲ�������
}
