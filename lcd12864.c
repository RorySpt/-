#include "lcd.h"
#include <intrins.h>
#include <stdlib.h>
#define LCD_IO P1
//#include "delay.h"
uchar code BitSelection[7][8]=
{
	{0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80},
	{0x03,0x06,0x0c,0x18,0x30,0x60,0xc0,0x00},
	{0x07,0x0e,0x1c,0x38,0x70,0xe0,0x00,0x00},
	{0x0f,0x1e,0x3c,0x78,0xf0,0x00,0x00,0x00},
	{0x1f,0x3e,0x7c,0xf8,0x00,0x00,0x00,0x00},
	{0x3f,0x7e,0xfc,0x00,0x00,0x00,0x00,0x00},
	{0x7f,0xfe,0x00,0x00,0x00,0x00,0x00,0x00},
};
bit lcd_busy()
{
	bit result;
	LCD_IO=0xff;
	LCD_RS=0;
	LCD_RW=1;
	//LCD_E=0;
	LCD_E=1;//_nop_();//_nop_();
	result=(bit)(LCD_IO&0x80);
	LCD_E=0;
	return(result);
}

 void lcd_init()
{
	/*RST=0;delay_10um(4);
	RST=1; */
	//LCD_PSB=1; 
	lcd_wcmd(0x30);
	lcd_wcmd(0x34);
	lcd_wcmd(0x0c);
	//lcd_wcmd(0x01);  //清屏
	d_100us();
}

uchar lcd_rdat()
{
	uchar rdat;
	while(lcd_busy());
	LCD_IO=0xff;
	LCD_RW=1;
	LCD_RS=1;
	LCD_E=1;//_nop_();
	rdat=LCD_IO;
	LCD_E=0;
	return rdat;
}

void  lcd_wdat(u8 data dat)
{	while(lcd_busy());
	LCD_RW=0;
	LCD_RS=1;
	//LCD_E=0;//_nop_();
	LCD_E=1;//_nop_();_nop_();
	LCD_IO=dat;_nop_();
	LCD_E=0;//_nop_();
}
void lcd_wcmd(u8 data dat)
{
	while(lcd_busy());
  LCD_RW=0;
	LCD_RS=0;
	//LCD_E=0;//_nop_();
	LCD_E=1;//_nop_();
	LCD_IO=dat;_nop_();
	LCD_E=0;//_nop_();
}


//void lcd_Draw_PM(const unsigned  char *ptr){}
void lcd_Draw_PM_Any(uchar x,uchar y,uchar wide,uchar hight,uchar *ptr)
{
	uchar line,row,movx,lcdx;
	uchar addx,addy;
	uchar dat0,dat1;
	lcd_wcmd(0x36);
	if(wide%8==0)wide=wide/8;else wide=wide/8+1;
	lcdx=x/16;
	
	for(line=0;line<hight;line++)
	{
		movx=x&0x0f;
		if(y+line>31)
		{
			addy=0x80+y+line-32;
			addx=0x80+lcdx+8;
	
		}
		else
		{
			addy=0x80+y+line;
			addx=0x80+lcdx;

		}
		lcd_wcmd(addy);
		lcd_wcmd(addx);
		if(movx==0)
		{
			for(row=0;row<wide;row++)
			{
				lcd_wdat(ptr[line*wide+row]);
			}
			lcd_wdat(0x00);
			//lcd_DrawLineY(x+wide,y,hight,0);
		}
		else if(movx<8)
		{
			lcd_rdat();
			dat0=lcd_rdat();
			//dat1=lcd_rdat();
			dat0=(dat0>>movx)<<movx;
			lcd_wcmd(addy);
			lcd_wcmd(addx);
			for(row=0;row<wide;row++)
			{
				if(row==0)lcd_wdat((ptr[line*wide+row]>>movx)|dat0);
				else 
					{
						if(row==(wide-1)){lcd_wdat((ptr[line*wide+row]>>movx)|(ptr[line*wide+row-1]<<(8-movx)));if(wide)lcd_wdat(ptr[line*wide+row]<<(8-movx));}
						else lcd_wdat((ptr[line*wide+row]>>movx)|(ptr[line*wide+row-1]<<(8-movx)));
					}
			}
			lcd_wdat(0x00);
		}
		else
		{
//			lcd_wcmd(addy);
//			lcd_wcmd(addx);
			lcd_rdat();
			dat0=lcd_rdat();
			dat1=lcd_rdat();
			dat1=(dat1>>movx)<<movx;
			movx-=8;
			
			lcd_wcmd(addy);
			lcd_wcmd(addx);
			lcd_wdat(dat0);
			for(row=0;row<wide;row++)
			{
				if(row==0)lcd_wdat((ptr[line*wide+row]>>movx)|dat1);
				else 
					{
						if(row==(wide-1)){lcd_wdat((ptr[line*wide+row]>>movx)|(ptr[line*wide+row-1]<<(8-movx)));if(wide)lcd_wdat(ptr[line*wide+row]<<(8-movx));}
						else lcd_wdat((ptr[line*wide+row]>>movx)|(ptr[line*wide+row-1]<<(8-movx)));
					}
			}
			lcd_wdat(0x00);
		}
	}
	
}
void lcd_Draw_PM_Any_plus(uchar startX,uchar startY,uchar wide,uchar hight,uchar *ptr)//x,wide:0~127;y,hight:0~63;
{
	uchar addx,addy,endx;
	uchar dat[4];
	uchar offsetx;
	uchar x,y,wideByte,widebyte;
	lcd_wcmd(0x36);
	
//	startaddx=startX>>4;
	wideByte=(wide-1>>4);
	widebyte=(wide-1>>3)+1;
//	if()
//	widebyte_p=(wide-1>>3)+1;
	offsetx=startX&0x07;
	for(y=0;y<hight;y++)//
	{
		if(y+startY<32){addx=0x80+(startX>>4);addy=y+startY+0x80;endx=0x80+(startX+wide>>4);}
		else 		{addx=0x88+(startX>>4);addy=y+startY+0x80-32;endx=0x88+(startX+wide>>4);}
		
		
		lcd_wcmd(addy);lcd_wcmd(addx);lcd_rdat();dat[0]=lcd_rdat();dat[1]=lcd_rdat();
		if(startX+wide-1<255){lcd_wcmd(addy);lcd_wcmd(endx);lcd_rdat();dat[2]=lcd_rdat();dat[3]=lcd_rdat();}
		
		lcd_wcmd(addy);lcd_wcmd(addx);
		
		for(x=0;x<widebyte;x++)
		{
			if((startX&0x0f)<8)//起始位置在高字节
			{
				if(x==0)//第一个字节
					{
						if(offsetx+wide<8)lcd_wdat(((dat[0]>>(8-offsetx))<<(8-offsetx))|((ptr[y*widebyte+x])>>offsetx)|(dat[0]&(0xff>>(offsetx+wide))));//|((dat[0]<<(offsetx+wide))>>(offsetx+wide))
							
						else {lcd_wdat(((dat[0]>>(8-offsetx))<<(8-offsetx))|((ptr[y*widebyte+x])>>offsetx));lcd_wdat(((dat[1]<<offsetx)>>offsetx)|((ptr[y*widebyte+x])<<(8-offsetx)));}
					}
				else if(x==widebyte-1)
					{
						lcd_wdat((ptr[y*widebyte+x-1]<<(8-offsetx))|((ptr[y*widebyte+x])>>(offsetx)));
						if(((startX+wide-1)&0x0f)!=0x0f)
						{
							if(!(widebyte%2))
							{
							lcd_wdat(((dat[2]<<offsetx)>>offsetx)|((ptr[y*widebyte+x])<<(8-offsetx)));
							lcd_wdat(dat[3]);
							}
							else
							{
								lcd_wdat(((dat[3]<<offsetx)>>offsetx)|((ptr[y*widebyte+x])<<(8-offsetx)));
							}
						}
					}
				else {lcd_wdat((ptr[y*widebyte+x-1]<<(8-offsetx))|((ptr[y*widebyte+x])>>(offsetx)));}
			}
			else
			{
				if(x==0)
					{
						lcd_wdat(dat[0]);
						if(offsetx+wide<8)lcd_wdat(((dat[1]>>(8-offsetx))<<(8-offsetx))|((ptr[y*widebyte+x])>>offsetx)|(dat[1]&(0xff>>(offsetx+wide))));
						else {lcd_wdat(((dat[1]>>(8-offsetx))<<(8-offsetx))|((ptr[y*widebyte+x])>>offsetx));lcd_wdat(((dat[2]<<offsetx)>>offsetx)|((ptr[y*widebyte+x])<<(8-offsetx)));}
					}
				else if(x==widebyte-1)
					{
						lcd_wdat((ptr[y*widebyte+x-1]<<(8-offsetx))|((ptr[y*widebyte+x])>>(offsetx)));
						if(((startX+wide-1)&0x0f)!=0x0f)
						{
							if((widebyte%2))
							{
							lcd_wdat(((dat[2]<<offsetx)>>offsetx)|((ptr[y*widebyte+x])<<(8-offsetx)));
							lcd_wdat(dat[3]);
							}
							else
							{
								lcd_wdat(((dat[3]<<offsetx)>>offsetx)|((ptr[y*widebyte+x])<<(8-offsetx)));
							}
						}
					}
				else {lcd_wdat((ptr[y*widebyte+x-1]<<(8-offsetx))|((ptr[y*widebyte+x])>>(offsetx)));}
			}
			
		}
		
	}
	
	
	
}
void lcd_Draw_PM(uchar x,uchar y,uchar wide,uchar hight,uchar *ptr)
{
	uchar line,row;
	lcd_wcmd(0x36);
	//if(wide%8==0)wide=wide/8;else wide=wide/8+1;
	for(line=0;line<hight;line++)
	{
		if(y+line>31)
		{
			lcd_wcmd(0x80+y+line-32);
			lcd_wcmd(0x80+x+8);			
		}
		else
		{
			lcd_wcmd(0x80+y+line);
			lcd_wcmd(0x80+x);
		}
		for(row=0;row<wide;row++)
		{
			lcd_wdat(ptr[line*wide+row]);
		}
	}
	
}
void lcd_Draw_PM_dat(uchar x,uchar y,uchar wide,uchar hight,uchar dat)//x:8,y:64,wide:16,hight:64
{
	uchar line,row;
	lcd_wcmd(0x36);
	//if(wide%8==0)wide=wide/8;else wide=wide/8+1;
	for(line=0;line<hight;line++)
	{
		if(y+line>31)
		{
			lcd_wcmd(0x80+y+line-32);
			lcd_wcmd(0x80+x+8);			
		}
		else
		{
			lcd_wcmd(0x80+y+line);
			lcd_wcmd(0x80+x);
		}
		for(row=0;row<wide;row++)
		{
			lcd_wdat(dat);
		}
	}
}
void lcd_Draw_PM_dat2(uchar x,uchar y,uchar wide,uchar hight,uchar dat0,uchar dat1)//给指定区域填入指定内容
{
	uchar line,row;
	lcd_wcmd(0x36);
	//if(wide%8==0)wide=wide/8;else wide=wide/8+1;
	for(line=0;line<hight;line++)
	{
		if(y+line>31)
		{
			lcd_wcmd(0x80+y+line-32);
			lcd_wcmd(0x80+x+8);			
		}
		else
		{
			lcd_wcmd(0x80+y+line);
			lcd_wcmd(0x80+x);
		}
		for(row=0;row<wide;row++)
		{
			lcd_wdat(dat0);lcd_wdat(dat1);
		}
	}
}

void lcd_ch_display(uchar y,uchar x,uchar *z)
{ 
 	uchar Address;
	lcd_wcmd(0x30);
 	if(y==0){Address=0x80+x;} 	          //Y判断第几行,X判断第几列,0x80为液晶行初始地址
 	if(y==1){Address=0x90+x;} 	   
 	if(y==2){Address=0x88+x;} 
 	if(y==3){Address=0x98+x;} 
	lcd_wcmd(Address);	          //写入地址命令到LCD12864	
	while(*z!='\0')					      //写入显示数据的大小
	{		
 		lcd_wdat(*z++); 	          //写入显示数据到LCD12864					  
	}  
}
//void lcd_clr_GD(void)
//{
//	uchar i, j, k;
//	lcd_wcmd(0x34);        //打开扩展指令集  
//	for(j=0,i=0x80; j<32; j++)
//	{
//		lcd_wcmd(i++);
//		lcd_wcmd(0x80);
//		for(k=0; k<16; k++)lcd_wdat(0x00);
//	}
//	for(j=0,i=0x80; j<32; j++)
//	{
//		lcd_wcmd(i++);
//		lcd_wcmd(0x88);	   
//		for(k=0; k<16; k++)lcd_wdat(0x00);
//	}  
//}
void lcd_clr_GD(void)
{
	uchar i,j;
	lcd_wcmd(0x34);
	for(j=0;j<64;j++)
	{
		lcd_wcmd(0x80+j);
		for(i=0;i<32;i++)lcd_wdat(0x00);
	}
}

void lcd_clr_DDram()
{
	uchar i;
	lcd_wcmd(0x30);
	lcd_wcmd(0x80);
	for(i=0;i<32;i++)lcd_wdat(' ');
	lcd_wcmd(0x90);
	for(i=0;i<32;i++)lcd_wdat(' ');
}

void lcd_scrollBar(uchar x,uchar y,uchar hight,uchar Max,uchar num)
{
	uchar line,bar_l,bar_y;
	lcd_wcmd(0x36);
	bar_l=(hight)/Max;
	bar_y=1+bar_l*num;
	hight=hight-hight%Max+1;
	for(line=0;line<=hight;line++)
	{
		if(y+line>31)
		{
			lcd_wcmd(0x80+y+line-32);
			lcd_wcmd(0x80+x+8);			
		}
		else
		{
			lcd_wcmd(0x80+y+line);
			lcd_wcmd(0x80+x);
		}
			if(line==0){lcd_wdat(0x00);lcd_wdat(0x1f);}
			
			else if((line<bar_y+bar_l)&&(line>=bar_y)){lcd_wdat(0x00);lcd_wdat(0x11);}
			else if(line<=hight){lcd_wdat(0x00);lcd_wdat(0x1f);}
		
	}
}
//uchar lcd_ReadPoint(uchar X,uchar Y)
//{
//	
//}
uchar lcd_ReadPoint(uchar X,uchar Y)
{
	uchar rDat_H,rDat_L;
  lcd_wcmd(0x36);
	if(Y<32){lcd_wcmd(0x80+Y);lcd_wcmd(0x80+(X>>4));}
	else {lcd_wcmd(0x80+Y-32);lcd_wcmd(0x88+(X>>4));}
	lcd_rdat();//丢掉第一次读数，0x80忙标志
	rDat_H=lcd_rdat();
	rDat_L=lcd_rdat();
	if((X&0x0f)<8)return rDat_H&(0x80>>(X&0x0f));
	else return rDat_L&(0x80>>((X&0x0f)-8));
}
void lcd_DrawPoint(uchar X,uchar Y,uchar Color)
{
  uchar addx,addy,wordx;
	uchar rDat_H,rDat_L;
  lcd_wcmd(0x36);
	addx=0x80+(X>>4);
	if(Y<32)addy=0x80+Y;else {addy=0x80+Y-32;addx+=8;}

	lcd_wcmd(addy);lcd_wcmd(addx);
	lcd_rdat();//丢掉第一次读数，0x80忙标志
	rDat_H=lcd_rdat();
	rDat_L=lcd_rdat();
	wordx=X&0x0f;
	lcd_wcmd(addy);lcd_wcmd(addx);
	if(wordx<8)
	{switch(Color)
		{
			case 0:rDat_H&=(~(0x01<<(7-wordx)));break;//清点
			case 1:rDat_H|=((0x01<<(7-wordx)));break;//或
			case 2:rDat_H^=((0x01<<(7-wordx)));break;//异或
			default:break;
		}
		lcd_wdat(rDat_H);lcd_wdat(rDat_L);
	}
	else
	{
		//wordx=wordx&0x07;
		switch(Color)
		{
			case 0:rDat_L&=(~(0x01<<(15-wordx)));break;
			case 1:rDat_L|=((0x01<<(15-wordx)));break;
			case 2:rDat_L^=((0x01<<(15-wordx)));break;
			default:break;
		}
		
		lcd_wdat(rDat_H);lcd_wdat(rDat_L);
	}
	
	
}
//画水平线：
void lcd_DrawLineX( uchar X0, uchar X1, uchar Y, uchar Color )
{   uchar Temp ;
    if( X0 > X1 )
    {
        Temp = X1 ;
        X1 = X0 ;
        X0 = Temp ;
    }
    for( ; X0 <= X1 ; X0++ )
    lcd_DrawPoint( X0, Y, Color ) ;   
}
//画垂直线：
void lcd_DrawLineY( uchar X, uchar Y0, uchar Y1, uchar Color )
{
    uchar Temp ;
    if( Y0 > Y1 )
    {
        Temp = Y1 ;
        Y1 = Y0 ;
        Y0 = Temp ;
    }
    for(; Y0 <= Y1 ; Y0++)
    lcd_DrawPoint( X, Y0, Color)    ;
}

void lcd_DrawLine( uchar StartX, uchar StartY, uchar EndX, uchar EndY, uchar Color )
{
    int t, distance;      /*根据屏幕大小改变变量类型(如改为int型)*/
    int x = 0 , y = 0 , delta_x, delta_y ;
    char incx, incy ;

    delta_x = EndX - StartX ;
    delta_y = EndY - StartY ;

    if( delta_x > 0 )
    {
        incx = 1;
    }
    else if( delta_x == 0 )
    {
        lcd_DrawLineY( StartX, StartY, EndY, Color ) ;
        return ;
    }
    else
    {
        incx = -1 ;
    }
    if( delta_y > 0 )
    {
        incy = 1 ;
    }
    else if(delta_y == 0 )
    {
        lcd_DrawLineX( StartX, EndX, StartY, Color ) ;   
        return ;
    }
    else
    {
        incy = -1 ;
    }

    delta_x = abs( delta_x );   
    delta_y = abs( delta_y );
    if( delta_x > delta_y )
    {
        distance = delta_x ;
    }
    else
    {
        distance = delta_y ;
    }
    lcd_DrawPoint( StartX, StartY, Color ) ;   
    /* Draw Line*/
    for( t = 0 ; t <= distance+1 ; t++ )
    {
        lcd_DrawPoint( StartX, StartY, Color ) ;
        x += delta_x ;
        y += delta_y ;
        if( x > distance )
        {
            x -= distance ;
            StartX += incx ;
        }
        if( y > distance )
        {
            y -= distance ;
            StartY += incy ;
        }
    }
}