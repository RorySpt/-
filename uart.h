#ifndef _Uart_H_
#define _Uart_H_

#include "habit.h"
#include <STC89C5xRC.H>

#define Fosc       11059200L        /*晶振*/
#define Baudrate   9600           /*波特率*/
#define SysPeriodMode 12					//机器周期
#define Uart_Timer1_Reload        (256 - Fosc / 16/ SysPeriodMode / Baudrate)       //Calculate the timer1 reload value at 12T mode and need SMOD=1
//#define T1_TimerReload        (256 - Fosc / 16 / Baudrate)      /*Calculate the timer1 reload value ar 1T mode  */
//#define TimeOut    (28800 / (unsigned long)Baudrate + 2)


//#define Self_Define_ISP_Download_Command 0x3D
#define Message_StartCode 0xAA

void Uart_Init 	( void );
void UartPutStr ( uchar *Uc_Str,uchar Uc_Len );

//extern xdata uchar Receive_buffer[16];
extern xdata uchar Send_buffer[40];
extern uchar Send_buffer_Str;

extern uchar TiBUSY;
extern uchar RiBUSY;
extern uchar xdata Receive_buffer[16];
#endif /* _Uart_H_ */

