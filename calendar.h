#ifndef _CALENDAR_H_
#define _CALENDAR_H_
#include "habit.h"

extern uchar century;

extern void Conversion(bit c,uchar year,uchar month,uchar day);
extern uchar CalculateWeek(u8 century,u8 year, u8 month, u8 day);
extern uchar Calculate_Totaldays_Ofmonth(uchar year,uchar month);
extern void cale_data_transfer();//将计算结果传至系统参数
extern data uchar moon_year,moon_month,moon_day,week;
extern data uchar  cale_moon_year,cale_moon_month,cale_moon_day;
extern uchar century;//20xx
//extern uchar Gredata[4];// 百年，年月日
extern uchar sun_week;
extern code uchar NLyear_dat[];
extern uchar code year_code[];
/*****************************************************************************

首先要能记住十大天干和十二地支，
十天干：  甲、乙、丙、丁、戊、己、庚、辛、壬、癸；
十二地支：子、丑、寅、卯、辰、巳、午、未、申、酉、戌、亥；

天干地支纪年法首先是天干在前，地支在后，比如今年2005就为－乙酉年，先来算算天干，
有个公式：
4、 5、 6、 7、 8、 9、 0、 1、 2、 3 对应的十天干就是
甲、乙、丙、丁、戊、己、庚、辛、壬、癸，
数字为年代的最后的一位数字，比如今年是2005，最后一位是5，对应的天干就是乙；

地支的算法：用年代数除以12，后面的余数就代表某个地支，
余数分别为：4、 5、 6、 7、 8、 9、 10、 11、 0（能整除）、1、 2、3.
代表地支为：子、丑、寅、卯、辰、巳、午、 未、 申、酉、戌、亥.
比如2005年为例：年代末尾数为5，对应的天干为乙，2005除以12，余数为1，对应的地支为酉，所以2005年为乙酉年。

******************************************************************************/

#endif