#ifndef _BUTTON_H_
#define _BETTON_H_
#include <STC89C5xRC.H>
#include "habit.h"

sbit KEY0=P3^7;
sbit KEY1=P3^6;
sbit KEY2=P3^4;
sbit KEY3=P3^5;

void recognition_key();

extern uchar data Command_key[4];
extern uchar data Prestate_key[4];//
extern uchar data Instruction_code_key[4];
extern uint data Double_Press_counter[4];//
extern uchar OptionCode[];
extern uchar OptionCode_Max[];



#endif