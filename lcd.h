#include <STC89C5xRC.h>
#ifndef _LCD_H_
#define _LCD_H_

#include "habit.h"
#include "delay.h"
//bit get_cale_moon_day(uchar month_p,uint table_addr)                                                                                        
//#define LCD_CMD_exp lcd_wcmd(0x34)  //Use extended instructions
#define LCD_cmd_clrPrint 									lcd_wcmd(0x01)
#define LCD_cmd_homeAddress 							lcd_wcmd(0x02)
#define LCD_cmd_entryPointSet_cursorLeft 	lcd_wcmd(0x04)//�α����ƣ�DDRAM ��ַ��������AC���� 1
#define LCD_cmd_entryPointSet_cursorRight lcd_wcmd(0x06)
#define LCD_cmd_entryPointSet_screenleft 	lcd_wcmd(0x05)//��������
#define LCD_cmd_entryPointSet_screenRight lcd_wcmd(0x07)
#define LCD_cmd_showStatus_Off						lcd_wcmd(0x08)//����ʾ
#define LCD_cmd_showStatus_On 						lcd_wcmd(0x0c)
#define LCD_cmd_cursorDispShift(DISPLAY,SHIFT) lcd_wcmd(0x04|(DISPLAY<<3)|(SHIFT<<2))//on:1,off:0
#define LCD_cmd_functionSet(DL,RE) lcd_wcmd(0x20|(DL<<1)&0x08|(RE<<2))	//DL:8/4λ���ݿڣ���8/4��ָ�ѡ�����λ RE��1������ָ���RE��0������ָ�
#define LCD_cmd_functionSet_Extended			lcd_wcmd(0x34) //8λ���ݿڣ���չָ�
#define LCD_cmd_functionSet_DrawingOn			lcd_wcmd(0x36)	//����ָ�
#define LCD_cmd_functionSet_Basic					lcd_wcmd(0x30)	//����ָ�
#define LCD_cmd_cgRamAddress(addr)				lcd_wcmd(addr) //�趨 CGRAM��ַ,AC:00H~3FH
#define LCD_cmd_ddRamAddress(addr)				lcd_wcmd(0x80+(((((addr&0x18)<<1)|((addr&0x18)>>1))&0x18)+(addr&0x07)))//��ַ��������



sbit LCD_E=P2^7;
sbit LCD_RW=P2^6;
sbit LCD_RS=P2^5;
//sbit LCD_RST=P3^4;//lcd����
//sbit LCD_PSB=P3^2;//����ѡ��0����1����

//void 	d_100us			();
bit 	lcd_busy			();//æµ���
void 	lcd_init			(void);
void 	lcd_wdat			(u8 data dat);//д������
void 	lcd_wcmd			(u8 data dat);//д��ָ��
uchar lcd_rdat			();
//void 	lcd_Draw_PM		(const unsigned  char *ptr);//��ͼ
void lcd_Draw_PM		(uchar x,uchar y,uchar wide,uchar hight,uchar *ptr);
void 	lcd_clr_GD 		(void);//��������cg
void 	lcd_clr_DDram	();//��������dd������0x01
void lcd_ch_display	(uchar y,uchar x,uchar *z);
void lcd_Draw_PM_dat(uchar x,uchar y,uchar wide,uchar hight,uchar dat);//��ָ���ռ�����dat,������
void lcd_Draw_PM_dat2(uchar x,uchar y,uchar wide,uchar hight,uchar dat0,uchar dat1);////��ָ���ռ�����dat0,dat1;��ͼ
void lcd_DrawPoint(uchar X,uchar Y,uchar Color);//���㣬X:0~127,Y:0~64
void lcd_DrawLineX( uchar X0, uchar X1, uchar Y, uchar Color );//����
void lcd_DrawLineY( uchar X, uchar Y0, uchar Y1, uchar Color );//����
void lcd_DrawLine( uchar StartX, uchar StartY, uchar EndX, uchar EndY, uchar Color );//����
void lcd_scrollBar(uchar x,uchar y,uchar hight,uchar Max,uchar num);//������
void lcd_Draw_PM_Any(uchar x,uchar y,uchar wide,uchar hight,uchar *ptr);
void lcd_Draw_PM_Any_plus(uchar startX,uchar startY,uchar wide,uchar hight,uchar *ptr);//x,wide:0~127;y,hight:0~63;
uchar lcd_ReadPoint(uchar X,uchar Y);
#endif