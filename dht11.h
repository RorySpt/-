#ifndef _DHT11_H_
#define _DHT11_H_
#include <STC89C5xRC.H>
#include "habit.h"


sbit DHT_IO=P2^3;
void dht_byte(uchar n);
void dht_rdat();

extern uchar Dht_RhData[5];
extern u8 Dht_error;
#endif