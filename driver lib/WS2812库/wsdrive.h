#ifndef __WS2812_H_
#define __WS2812_H_	
#include "gpio.h"

void ws_load(void);

void ws_light(uint8_t R, uint8_t G, uint8_t B, uint8_t *array);

void ws_rainbow(void);

#endif
