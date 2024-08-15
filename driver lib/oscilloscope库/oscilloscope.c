#include"oscilloscope.h"
#include "oled.h"
#include "font.h"

uint16_t adcBuf[BUFFER_LEN];//�洢��ģ��ת������ADC����ȡ�Ĳ������ݡ� 
int atten = 1;//atten ��ʾ�ź�˥��ϵ�������ڽ�ADC�����ֶ���ת��Ϊʵ�ʵ�ѹֵ��˥����ͨ��������ADC�����뷶Χ�ڵ����źŵ�ƽ��
float vdiv = 2.0;//vdiv �Ǵ�ֱ��ѹ����������ȷ��ʾ������ÿ����ֱ��λ������ٵ�ѹ������attenһ�����ڽ�ADC����ת��Ϊ��ѹ
float trigVoltage = 0.0;//trigVoltage ��ʾ������ѹˮƽ������ȷ��ʾ���������źŵ�ʱ��㡣���źŴﵽ�����ѹˮƽʱ���ᴥ�����ݲɼ���
uint8_t trig = RISING, trigged;//trig ��һ���޷���8λ���������ڴ洢�������ͣ�����RISING�������ش�������FALLING���½��ش�������
int trigPoint;//trigged ��ʾ�����¼��Ƿ��Ѿ���������ͨ�����ڿ������ݲɼ��Ŀ�ʼ��
float tdiv = 10000.0;//trigPoint �Ǵ��������������ʾ��ADC�������д����¼�������λ�á�
uint32_t sampRate;//tdiv ��ˮƽʱ�����������ȷ��ʾ������ÿ��ˮƽ��λ�������ʱ�䡣�����ڽ�ADC������ʱ����ת��Ϊʵ��ʱ�䡣
float sampPer;//sampRate �ǲ����ʣ���ʾADCÿ��ɼ�������������
float maxVoltage, minVoltage;
float measuredFreq, sigPer;//��ʾ���������ź�Ƶ��,�źŵ�����ʱ��

// Convert ADC value to volts
float adcToVoltage(uint16_t samp)
{
  return atten * 2 * (((3.3 * samp) / 4096.0) - 1.65);
}

//Init
void oscilloscope_init(){
  OLED_NewFrame();
  OLED_PrintString(0, 0, "Oscilloscope", &font16x16, OLED_COLOR_NORMAL);
  OLED_ShowFrame();
  HAL_Delay(1000); 
}
  
// Draw the waveform on the screen
void drawWave()
{
  drawGraticule(64, 96, 16, 4, 4); // Draw the graticule

  maxVoltage = LOWER_VOLTAGE;
  minVoltage = UPPER_VOLTAGE;

  for (int i = 0; i <= 94; i++)
  {
    // If we're looping through the buffer, let's compute the minimum and maximum voltage values while we're at it
    float voltage1 = adcToVoltage(adcBuf[i + trigPoint]);
    float voltage2 = adcToVoltage(adcBuf[i + trigPoint + 1]);
    if (voltage2 > maxVoltage)
      maxVoltage = voltage2;
    if (voltage2 < minVoltage)
      minVoltage = voltage2;

    // Draw lines between sample points
    OLED_DrawLine(i, 31 - (voltage1 * 16 / vdiv), i + 1, 31 - (voltage2 * 16 / vdiv), OLED_COLOR_NORMAL);
  }
}

// This function finds the trigger point and also computes the frequency of thge signal
void findTrigger()
{
  int trigLevel = (4096.0 * (trigVoltage / (2.0 * atten) + 1.65)) / 3.3; // ADC level at which we should trigger
  int trigPoint2;                                                        // another trigger point, this will help us determine the period of the signal

  trigPoint = 0;
  trigged = 0;
  measuredFreq = 0;

  // The trigged variable will be 0 if we're not triggering, 1 if we only found 1 trigger point and 2 if we have at least two trigger points

  for (int i = 1; i < BUFFER_LEN / 2 && trigged != 2; i++) // we're looking for trigger points in the first half of the buffer
    if ((trig == RISING && adcBuf[i] >= trigLevel && adcBuf[i - 1] < trigLevel) || (trig == FALLING && adcBuf[i] <= trigLevel && adcBuf[i - 1] > trigLevel))
    {
      if (!trigged) // Looking for the first trigger point
      {
        trigPoint = i;
        trigged = 1;
      }
      else // Looking for the second one
      {
        trigPoint2 = i;
        trigged = 2;
      }
    }

  if (trigged == 2) // If we found at least two trigger points
  {
    // sigPer = sampPer * (trigPoint2 - trigPoint); // we compute the period of the signal in uS
    // measuredFreq = 1000000.0 / sigPer;           // and then we convert it into frequency, in Hz
  }
}

void dottedHLine(int x, int y, int l)
{
  uint8_t col = 1;
  for (int i = 0; i <= l; i++)
  {
    OLED_SetPixel(x + i, y, col);
    col = !col;
  }
}
void dottedVLine(int x, int y, int l)
{
  uint8_t col = 1;
  for (int i = 0; i <= l; i++)
  {
    OLED_SetPixel(x, y + i, col);
    col = !col;
  }
}
void drawGraticule(int hei, int wit, int pix, int divx, int divy)
{
  for (int i = 0; i <= wit; i += pix)
    dottedVLine(i, 0, hei);

  for (int i = 0; i <= hei + 1; i += pix)
    dottedHLine(0, i, wit);
}

