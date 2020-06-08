#ifndef _DELAY_H_
#define _DELAY_H_
#include <intrins.h>
#include "habit.h"
#include <STC89C5xRC.H>

void d_10us();
void d_100us();
void d_100ms();
void d_1s();
void d_1ms();
void d_n_ms(uint n);

#endif