#include "ds1302.h"

uchar ds_dat_time[7];
uchar ds_dat_time_set[7]={0,0,0,10,3,1,20};
uchar ds_dat_time_set16[7]={0,0,0,0,0,0,0};
//---DS1302写入和读取时分秒的地址命令---//
//---秒分时日月周年 最低位读写位;-------//
uchar code READ_RTC_ADDR[7] = {0x81, 0x83, 0x85, 0x87, 0x89, 0x8b, 0x8d}; 
uchar code WRITE_RTC_ADDR[7] = {0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c};

void Ds1302_wByte(uchar addr, uchar dat)
{
	uchar n;
	EA=0;
	DS_CE = 0;		_nop_();
	DS_SCLK = 0;	_nop_();
	DS_CE = 1;		_nop_();
	for (n=0; n<8; n++)			//开始传送八位地址命令
	{
		DS_IO = addr & 0x01;	//数据从低位开始传送
		addr >>= 1;
		DS_SCLK = 1;	_nop_();//数据在上升沿时，DS1302读取数据
		DS_SCLK = 0;	_nop_();
	}
	for (n=0; n<8; n++)			//写入8位数据
	{
		DS_IO = dat & 0x01;
		dat >>= 1;
		DS_SCLK = 1;	_nop_();//数据在上升沿时，DS1302读取数据		
		DS_SCLK = 0;	_nop_();			
	}	
		 
	DS_CE = 0;	_nop_();		//传送数据结束
	EA=1;
	
}

uchar Ds1302_rByte(uchar addr)
{
	uchar n,dat;
	EA=0;
	DS_CE = 0;		_nop_();
	DS_SCLK = 0;	_nop_();
	DS_CE = 1;		_nop_();
	for (n=0; n<8; n++)		//传送地址
	{
		DS_IO = addr & 0x01;	
		addr >>= 1;
		DS_SCLK = 1;	_nop_();
		DS_SCLK = 0;	_nop_();
	}
	for (n=0; n<8; n++)			//读取8位数据
	{
		dat=(dat>>1)|((uchar)DS_IO<<7);
		DS_SCLK = 1;	_nop_();//数据在上升沿时，DS1302读取数据		
		DS_SCLK = 0;	_nop_();			
	}	
		 
	DS_CE = 0;			_nop_();		//传送数据结束
	DS_SCLK = 1;		_nop_();
	DS_IO = 0;			_nop_();
	DS_IO = 1;			_nop_();
	EA=1;
	return dat;	
	
}
void Ds1302_init() 
{
	DS_CE=0;			//RST脚置低
	DS_SCLK=0;			//SCK脚置低
}
void Ds1302_Set_time()
{
	uchar n;
	Ds1302_wByte(0x8E,0X00);		 //禁止写保护，就是关闭写保护功能
	for (n=0; n<7; n++)//写入7个字节的时钟信号：分秒时日月周年
	{
		Ds1302_wByte(WRITE_RTC_ADDR[n],ds_dat_time_set[n]);	
	}
	Ds1302_wByte(0x8E,0x80);		 //打开写保护功能
}

void Ds1302_Read_time()
{
	uchar n;
	for (n=0; n<7; n++)//读取7个字节的时钟信号：分秒时日月周年
	{
		ds_dat_time[n] = Ds1302_rByte(READ_RTC_ADDR[n]);
	}
	//Ds1302_tDat_BCD();
		
}
void Ds1302_time_Copy(uchar sign)//复制时间信息
{
	uchar n;
	for (n=0; n<7; n++)
	{
		if(sign==0)ds_dat_time_set[n]=ds_dat_time[n];
		if(sign==1)ds_dat_time_set16[n]=(ds_dat_time_set[n]>>4)*10+(ds_dat_time_set[n]&0x0f);
		if(sign==2)ds_dat_time_set[n]=ds_dat_time_set16[n]/10*16+ds_dat_time_set16[n]%10;
	}
}

