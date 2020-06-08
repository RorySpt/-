#include <STC89C5xRC.h>
#ifndef _LCD_H_
#define _LCD_H_

#include "habit.h"
#include "delay.h"
//bit get_cale_moon_day(uchar month_p,uint table_addr)                                                                                        
//#define LCD_CMD_exp lcd_wcmd(0x34)  //Use extended instructions
#define LCD_cmd_clrPrint 									lcd_wcmd(0x01)
#define LCD_cmd_homeAddress 							lcd_wcmd(0x02)
#define LCD_cmd_entryPointSet_cursorLeft 	lcd_wcmd(0x04)//游标左移，DDRAM 地址计数器（AC）减 1
#define LCD_cmd_entryPointSet_cursorRight lcd_wcmd(0x06)
#define LCD_cmd_entryPointSet_screenleft 	lcd_wcmd(0x05)//画面右移
#define LCD_cmd_entryPointSet_screenRight lcd_wcmd(0x07)
#define LCD_cmd_showStatus_Off						lcd_wcmd(0x08)//关显示
#define LCD_cmd_showStatus_On 						lcd_wcmd(0x0c)
#define LCD_cmd_cursorDispShift(DISPLAY,SHIFT) lcd_wcmd(0x04|(DISPLAY<<3)|(SHIFT<<2))//on:1,off:0
#define LCD_cmd_functionSet(DL,RE) lcd_wcmd(0x20|(DL<<1)&0x08|(RE<<2))	//DL:8/4位数据口，填8/4；指令集选择控制位 RE＝1，扩充指令集；RE＝0，基本指令集
#define LCD_cmd_functionSet_Extended			lcd_wcmd(0x34) //8位数据口，拓展指令集
#define LCD_cmd_functionSet_DrawingOn			lcd_wcmd(0x36)	//基本指令集
#define LCD_cmd_functionSet_Basic					lcd_wcmd(0x30)	//基本指令集
#define LCD_cmd_cgRamAddress(addr)				lcd_wcmd(addr) //设定 CGRAM地址,AC:00H~3FH
#define LCD_cmd_ddRamAddress(addr)				lcd_wcmd(0x80+(((((addr&0x18)<<1)|((addr&0x18)>>1))&0x18)+(addr&0x07)))//地址重新排序，



sbit LCD_E=P2^7;
sbit LCD_RW=P2^6;
sbit LCD_RS=P2^5;
//sbit LCD_RST=P3^4;//lcd重置
//sbit LCD_PSB=P3^2;//串并选择，0串，1并。

//void 	d_100us			();
bit 	lcd_busy			();//忙碌检查
void 	lcd_init			(void);
void 	lcd_wdat			(u8 data dat);//写入数据
void 	lcd_wcmd			(u8 data dat);//写入指令
uchar lcd_rdat			();
//void 	lcd_Draw_PM		(const unsigned  char *ptr);//绘图
void lcd_Draw_PM		(uchar x,uchar y,uchar wide,uchar hight,uchar *ptr);
void 	lcd_clr_GD 		(void);//清屏，清cg
void 	lcd_clr_DDram	();//清屏，清dd，代替0x01
void lcd_ch_display	(uchar y,uchar x,uchar *z);
void lcd_Draw_PM_dat(uchar x,uchar y,uchar wide,uchar hight,uchar dat);//给指定空间填入dat,清屏用
void lcd_Draw_PM_dat2(uchar x,uchar y,uchar wide,uchar hight,uchar dat0,uchar dat1);////给指定空间填入dat0,dat1;绘图
void lcd_DrawPoint(uchar X,uchar Y,uchar Color);//画点，X:0~127,Y:0~64
void lcd_DrawLineX( uchar X0, uchar X1, uchar Y, uchar Color );//横线
void lcd_DrawLineY( uchar X, uchar Y0, uchar Y1, uchar Color );//竖线
void lcd_DrawLine( uchar StartX, uchar StartY, uchar EndX, uchar EndY, uchar Color );//画线
void lcd_scrollBar(uchar x,uchar y,uchar hight,uchar Max,uchar num);//滚动条
void lcd_Draw_PM_Any(uchar x,uchar y,uchar wide,uchar hight,uchar *ptr);
void lcd_Draw_PM_Any_plus(uchar startX,uchar startY,uchar wide,uchar hight,uchar *ptr);//x,wide:0~127;y,hight:0~63;
uchar lcd_ReadPoint(uchar X,uchar Y);
#endif