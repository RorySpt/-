#ifndef _TEMPS_H_
#define _TEMPS_H_
#include <STC89C5xRC.H>
#include "delay.h"
#include "habit.h"


sbit 	TEMP_IO=P2^4;

uchar Ds18b20_Init				();
void 	Ds18b20_wByte				(uchar dat);
uchar Ds18b20_rByte				();
void  Ds18b20_ChangTemp		();
void  Ds18b20_rTempcom		();
void 	Ds18b20_rTemp				();
void 	temp_data_collation	();

extern uchar temp_data_Integer;
extern uchar  temp_data_Decimal_r;//保留两位有效数字
extern uchar temp_data_sign;//1负，0正
extern uchar error;
extern uchar temp_High;
extern uchar temp_Low;

#endif