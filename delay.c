#include "delay.h"

void d_10us()
{
	unsigned char i;

	_nop_();
	i = 6;
	while (--i);
}
void d_100us()
{
	unsigned char i;

	_nop_();
	i = 89;
	while (--i);
}
void d_100ms()
{
	unsigned char i, j, k;

	_nop_();
	i = 2;
	j = 103;
	k = 147;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
void d_1s()
{
	unsigned char i, j, k;

	_nop_();
	i = 15;
	j = 2;
	k = 235;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void d_1ms()
{
unsigned char i, j;

	_nop_();
	i = 4;
	j = 146;
	do
	{
		while (--j);
	} while (--i);
}

void d_n_ms(uint n)
{
	
	while(n--)
	{
		d_1ms();
	}
	
}

	