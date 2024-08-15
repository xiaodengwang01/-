#include"oscilloscope.h"
#include "oled.h"
#include "font.h"

uint16_t adcBuf[BUFFER_LEN];//存储从模数转换器（ADC）读取的采样数据。 
int atten = 1;//atten 表示信号衰减系数，用于将ADC的数字读数转换为实际电压值。衰减器通常用于在ADC的输入范围内调整信号电平。
float vdiv = 2.0;//vdiv 是垂直电压除数，用于确定示波器上每个垂直单位代表多少电压。它与atten一起用于将ADC读数转换为电压
float trigVoltage = 0.0;//trigVoltage 表示触发电压水平，用于确定示波器捕获信号的时间点。当信号达到这个电压水平时，会触发数据采集。
uint8_t trig = RISING, trigged;//trig 是一个无符号8位整数，用于存储触发类型，例如RISING（上升沿触发）或FALLING（下降沿触发）。
int trigPoint;//trigged 表示触发事件是否已经发生。这通常用于控制数据采集的开始。
float tdiv = 10000.0;//trigPoint 是触发点的索引，表示在ADC缓冲区中触发事件发生的位置。
uint32_t sampRate;//tdiv 是水平时间除数，用于确定示波器上每个水平单位代表多少时间。它用于将ADC样本的时间间隔转换为实际时间。
float sampPer;//sampRate 是采样率，表示ADC每秒采集的样本数量。
float maxVoltage, minVoltage;
float measuredFreq, sigPer;//表示测量到的信号频率,信号的周期时间

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

