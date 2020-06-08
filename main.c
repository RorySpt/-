#include <STC89C5xRC.H>
#include "lcd.h"
#include "habit.h"
#include "delay.h"
#include "bmp.h"
#include "temps.h"
#include "test.h"
#include "ds1302.h"
#include "button.h"
#include "dht11.h"
#include "rot.h"
#include "zk.h"
//#include "uart.h"

//extern u8 data Dht_FLAG;
void main()
{
	//uchar i;
	//while(1)test_lcd12864_displayPM0();
	//while(1){P0=0x00;}
	
	//d_1s();d_1s();P0=0x0f;d_1s();d_1s();}
//	Uart_Init();
//	
//while(1)	
//	{
//		
////		//while(!RI);RI=0;
////		SBUF=0xAA;
////		while(!TI);TI=0;
////		SBUF=0x06;
////		while(!TI);TI=0;
////		SBUF=0x00;
////		while(!TI);TI=0;
////		SBUF=0xB0;
////		while(!TI);TI=0;
//		d_1s();
//		d_1s();
//		d_1s();
//	}
//	lcd_init();
//	lcd_clr_DDram	();
//	lcd_clr_GD ();
//	while(1)
//	{
//		while(!RI);RI=0;
//		lcd_wcmd(0x30);
//		lcd_wcmd(0x80);
//		lcd_wdat(SBUF/16+48);
//		lcd_wdat(SBUF%16+48);
//		SBUF=SBUF;
//		while(!TI);TI=0;
//		
//	}
//while(DHT_IO&&Dht_FLAG++);
//while(1)test_dht();
rot_init();while(1)rot_taskSchedule();

//			lcd_DrawLineX( 5,110, 6, 2 );//横线
//			lcd_DrawLineY( 52, 2,77, 2 );//竖线
//			lcd_DrawLineY( 54, 2,77, 2 );//竖线
//			lcd_DrawLineY( 58, 2,77, 2 );//竖线
//while(1);
//test_display_temps();
//	lcd_init();
//	lcd_wcmd(0x36);
//	while(1)
//	{
//	for(i=0;i<11;i++){lcd_Draw_PM(i,0,2,32,num16321[i]);d_1s();}
//	for(i=0;i<11;i++){lcd_Draw_PM(i,0,2,32,num16322[i]);d_1s();}
//	for(i=0;i<11;i++){lcd_Draw_PM(i,0,2,32,num16323[i]);d_1s();}
//	for(i=0;i<11;i++){lcd_Draw_PM(i,0,2,32,num16324[i]);d_1s();}
//	}
}