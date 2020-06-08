#ifndef _DS1302_H_
#define _DS1302_H_

#include <STC89C5xRC.H>
#include <habit.h>
#include <intrins.h>
/***测试**///外接ds1302
sbit DS_SCLK=P2^2;
sbit DS_IO	=P2^1;
sbit DS_CE	=P2^0;
/*//内置ds1302
sbit DS_SCLK=P3^6;
sbit DS_IO	=P3^4;
sbit DS_CE	=P3^5;
*/
void 	Ds1302_wByte			(uchar addr, uchar dat);
uchar Ds1302_rByte			(uchar addr);
void 	Ds1302_Set_time		();
void 	Ds1302_Read_time	();
void  Ds1302_tDat_BCD		();
void 	Ds1302_time_Copy	(uchar sign);//复制时间信息
void 	Ds1302_init				() ;
extern uchar ds_dat_time[7];
extern uchar ds_dat_time_set[7];
extern uchar ds_dat_time_set16[7];


#endif