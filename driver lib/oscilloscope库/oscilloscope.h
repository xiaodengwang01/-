#include"main.h"

#define UPPER_VOLTAGE 3.3
#define LOWER_VOLTAGE 0.0

float adcToVoltage(uint16_t samp);

void drawWave();
void findTrigger();

void dottedHLine(int x, int y, int l);
void dottedVLine(int x, int y, int l);
void drawGraticule(int hei, int wit, int pix, int divx, int divy);


