#ifndef _CALENDAR_H_
#define _CALENDAR_H_
#include "habit.h"

extern uchar century;

extern void Conversion(bit c,uchar year,uchar month,uchar day);
extern uchar CalculateWeek(u8 century,u8 year, u8 month, u8 day);
extern uchar Calculate_Totaldays_Ofmonth(uchar year,uchar month);
extern void cale_data_transfer();//������������ϵͳ����
extern data uchar moon_year,moon_month,moon_day,week;
extern data uchar  cale_moon_year,cale_moon_month,cale_moon_day;
extern uchar century;//20xx
//extern uchar Gredata[4];// ���꣬������
extern uchar sun_week;
extern code uchar NLyear_dat[];
extern uchar code year_code[];
/*****************************************************************************

����Ҫ�ܼ�סʮ����ɺ�ʮ����֧��
ʮ��ɣ�  �ס��ҡ����������졢�������������ɡ��
ʮ����֧���ӡ�������î�������ȡ��硢δ���ꡢ�ϡ��硢����

��ɵ�֧���귨�����������ǰ����֧�ں󣬱������2005��Ϊ�������꣬����������ɣ�
�и���ʽ��
4�� 5�� 6�� 7�� 8�� 9�� 0�� 1�� 2�� 3 ��Ӧ��ʮ��ɾ���
�ס��ҡ����������졢�������������ɡ��
����Ϊ���������һλ���֣����������2005�����һλ��5����Ӧ����ɾ����ң�

��֧���㷨�������������12������������ʹ���ĳ����֧��
�����ֱ�Ϊ��4�� 5�� 6�� 7�� 8�� 9�� 10�� 11�� 0������������1�� 2��3.
�����֧Ϊ���ӡ�������î�������ȡ��硢 δ�� �ꡢ�ϡ��硢��.
����2005��Ϊ�������ĩβ��Ϊ5����Ӧ�����Ϊ�ң�2005����12������Ϊ1����Ӧ�ĵ�֧Ϊ�ϣ�����2005��Ϊ�����ꡣ

******************************************************************************/

#endif