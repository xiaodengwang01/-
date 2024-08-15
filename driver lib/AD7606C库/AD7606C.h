#ifndef __AD7606C_H
#define __AD7606C_H

#include "main.h"

#define STATUS_Adress		0x0100
#define Range1_Adress		0x0300
#define Range2_Adress		0x0400
#define Range3_Adress		0x0500
#define Range4_Adress		0x0600
#define Bandwidth_Adress	0x0700
#define Oversampling_Adress	0x0800
#define Name_ID_Adress		0x2F00

#define SINGLE_2V5			0X0000
#define SINGLE_5V			0X0011
#define SINGLE_6V25			0X0022
#define SINGLE_10V			0X0033
#define SINGLE_12V5			0X0044
#define DIFF_5V				0X0088
#define DIFF_10V			0X0099
#define DIFF_12V5			0X00AA
#define DIFF_20V			0X00BB

#define HighBand			0x00FF

#define OverSamp_off		0x0000
#define OverSamp2			0x0001
#define OverSamp4			0x0002
#define OverSamp8			0x0003
#define OverSamp16			0x0004
#define OverSamp32			0x0005
#define OverSamp64			0x0006
#define OverSamp128			0x0007
#define OverSamp256			0x0008


uint16_t AD7606C_Init(uint16_t* p, uint16_t Range, uint16_t Bandwidth, uint16_t OverSamp);
#endif /* __AD7606C_H */
