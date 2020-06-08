#include "jq8900.h"
#include "uart.h"
#include "ds1302.h"
#include "calendar.h"

uchar RiPlaybackStatus=0;//播放状态
uchar RiTotalTrackTlaybackTime[3]={0,0,0};//曲目总时间，时分秒
uchar RiCurrentPlayingTime[3]={0,0,0};//曲目已播放时间
uint RiTotalTrack=0;//总曲目
uint RiCurrentTrack=0;//当前曲目

//uchar code JqCmdCode_groupPlay[]=
//{
//	0xAA,0x1B
//};//
extern uchar Receive_buffer_num;
//extern uchar xdata Receive_buffer[][];//第一位
//uchar code JqCmdCode_;
//uchar code NaoLing0[]={
//0xAA, 0x07, 0x02, 0x00, 0x21, 0xD4, 
//};
//uchar code NaoLing1[]={
//0xAA, 0x07, 0x02, 0x00, 0x20, 0xD3, 
//};
//uchar code NaoLing2[]={
//0xAA, 0x07, 0x02, 0x00, 0x19, 0xD2, 
//};
//uchar code NaoLing3[]={
//0xAA, 0x07, 0x02, 0x00, 0x18, 0xD1, 
//};
//void JpAlarmMusic(uchar dat)
//{
//	uchar n=0,i;
//	if(dat==0)
//	{
//	for(i=0;i<6;i++)Send_buffer[i]=NaoLing0[i];UartPutStr(Send_buffer,6);
//	}
//	if(dat==1)
//	{
//	for(i=0;i<6;i++)Send_buffer[i]=NaoLing1[i];UartPutStr(Send_buffer,6);
//	}
//	if(dat==2)
//	{
//	for(i=0;i<6;i++)Send_buffer[i]=NaoLing2[i];UartPutStr(Send_buffer,6);
//	}
//	if(dat==3)
//	{
//	for(i=0;i<6;i++)Send_buffer[i]=NaoLing3[i];UartPutStr(Send_buffer,6);
//	}

//}

void JqPlaySpecifiedTrack(uint dat)//播放指定曲目0~65535
{
	uchar n,i;
	n=0;
	Send_buffer[n++]=0xAA;
	Send_buffer[n++]=0x07;//组合播放指令码;
	Send_buffer[n++]=0x02;//长度
	Send_buffer[n++]=(uchar)(dat/0xff);
	Send_buffer[n++]=(uchar)(dat%0xff);
	Send_buffer[n]=0;
	for(i=0;i<n;i++)
	Send_buffer[n]+=Send_buffer[i];
	UartPutStr(Send_buffer,n+1);
}
void JqSpecify_volume(uchar dat)//指定音量
{
	uchar n,i;
	n=0;
	Send_buffer[n++]=0xAA;
	Send_buffer[n++]=0x13;
	Send_buffer[n++]=0x01;//长度
	Send_buffer[n++]=dat;
	Send_buffer[n]=0;
	for(i=0;i<n;i++)
	Send_buffer[n]+=Send_buffer[i];
	UartPutStr(Send_buffer,n+1);
}
//***************************************
//函数功能：设置循环模式
//指令		：00：全盘循环，01：单曲循环，02：单曲停止
//					03：全盘随机，04：目录循环，05：目录随机
//					06：目录顺序，07：全盘顺序
//***************************************
void JqSetLoopMode(uchar dat)//
{
	uchar n,i;
	n=0;
	Send_buffer[n++]=0xAA;
	Send_buffer[n++]=0x18;//组合播放指令码;
	Send_buffer[n++]=0x01;//长度
	Send_buffer[n++]=dat;
	Send_buffer[n]=0;
	for(i=0;i<n;i++)
	Send_buffer[n]+=Send_buffer[i];
	UartPutStr(Send_buffer,n+1);
}
void JqPlayCmd2(uchar dat)//02:播放，03：暂停，04：停止，05：上一曲，06：下一曲
{
	uchar n,i;
	n=0;
	Send_buffer[n++]=0xAA;
	switch(dat)
	{
		case 0:{Send_buffer[n++]=0x02;break;}//播放
		case 1:{Send_buffer[n++]=0x03;break;}//暂停
		case 2:{Send_buffer[n++]=0x04;break;}//停止
		case 3:{Send_buffer[n++]=0x05;break;}//上一曲
		case 4:{Send_buffer[n++]=0x06;break;}//下一曲
		case 5:{Send_buffer[n++]=0x07;break;}//结束播放
		case 6:{Send_buffer[n++]=0x14;break;}//音量加
		case 7:{Send_buffer[n++]=0x15;break;}//音量减
		case 8:{Send_buffer[n++]=0x25;break;}//开始查询时间
		default  :return;
	}
	Send_buffer[n++]=0x00;
	Send_buffer[n]=0;
	for(i=0;i<n;i++)
	Send_buffer[n]+=Send_buffer[i];
	UartPutStr(Send_buffer,n+1);
}

void JqPlayCmd(uchar dat)//02:播放，03：暂停，04：停止，05：上一曲，06：下一曲////for task
{
	uchar n,i;
	n=0;
	Send_buffer[n++]=0xAA;
	switch(dat)
	{
		case 0:{Send_buffer[n++]=0x02;break;}//播放
		case 1:{Send_buffer[n++]=0x03;break;}//暂停
		case 2:{Send_buffer[n++]=0x04;break;}//停止
		case 3:{Send_buffer[n++]=0x05;break;}//上一曲
		case 4:{Send_buffer[n++]=0x06;break;}//下一曲
		case 5:{Send_buffer[n++]=0x26;break;}//停止查询时间
		case 6:{Send_buffer[n++]=0x14;break;}//音量加
		case 7:{Send_buffer[n++]=0x15;break;}//音量减
		
		default  :return;
	}
	Send_buffer[n++]=0x00;
	Send_buffer[n]=0;
	for(i=0;i<n;i++)
	Send_buffer[n]+=Send_buffer[i];
	UartPutStr(Send_buffer,n+1);
}

void JqQueryCmd(uchar dat)//0~7
{
	uchar n,i;
	n=0;
	Send_buffer[n++]=0xAA;
	//组合播放指令码;
	switch(dat)
	{
		case 0:{Send_buffer[n++]=0x01;break;}//查询播放状态
		case 1:{Send_buffer[n++]=0x24;break;}//曲目播放总时间
		case 2:{Send_buffer[n++]=0x25;break;}//当前播放时间
//		case 3:{Send_buffer[n++]=0x1E;break;}//歌曲名称
		case 4:{Send_buffer[n++]=0x11;break;}//文件夹首曲曲目
		case 5:{Send_buffer[n++]=0x12;break;}//文件目录总曲目
		case 6:{Send_buffer[n++]=0x0C;break;}//总曲目
		case 7:{Send_buffer[n++]=0x0D;break;}//当前曲目
		default  :return;
	}
	Send_buffer[n++]=0x00;
	Send_buffer[n]=0;
	for(i=0;i<n;i++)
	Send_buffer[n]+=Send_buffer[i];
	UartPutStr(Send_buffer,n+1);
}

void JqPlaybackStatus()
{
	uchar n;
	n=0;
	Send_buffer[n++]=0xAA;
	Send_buffer[n++]=0x01;//组合播放指令码;
	Send_buffer[n++]=0x00;//长度
	Send_buffer[n]=0xAB;
	UartPutStr(Send_buffer,n+1);
	
}

void Tell_theData()
{
					uchar i,n;
					n=0;
					Send_buffer[n++]=0xAA;
					Send_buffer[n++]=0x1B;//组合播放指令码;
					Send_buffer[n++]=0x00;//长度
					Send_buffer[n++]='1';Send_buffer[n++]='3';
					Send_buffer[n++]='X';Send_buffer[n++]='2';
					Send_buffer[n++]='X';Send_buffer[n++]='0';
					Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[6]>>4)+48;
					Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[6]&0x0f)+48;
					Send_buffer[n++]='0';Send_buffer[n++]='5';//年
					
					if((ds_dat_time[4]>>4)==0)
					{Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[4]&0x0f)+48;}
					if((ds_dat_time[4]>>4)==1)
					{
						Send_buffer[n++]='X';Send_buffer[n++]='A';
						if(ds_dat_time[4]&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[4]&0x0f)+48;}
					}
					if((ds_dat_time[4]>>4)>1)
					{
						Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[4]>>4)+48;
						Send_buffer[n++]='X';Send_buffer[n++]='A';
						if(ds_dat_time[4]&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[4]&0x0f)+48;}
					}
					Send_buffer[n++]='0';Send_buffer[n++]='6';//月
					
					if((ds_dat_time[3]>>4)==0)
					{Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[3]&0x0f)+48;}
					if((ds_dat_time[3]>>4)==1)
					{
						Send_buffer[n++]='X';Send_buffer[n++]='A';
						if(ds_dat_time[3]&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[3]&0x0f)+48;}//确保不是零
					}
					if((ds_dat_time[3]>>4)>1)
					{
						Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[3]>>4)+48;
						Send_buffer[n++]='X';Send_buffer[n++]='A';
						if(ds_dat_time[3]&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[3]&0x0f)+48;}
					}
					Send_buffer[n++]='0';Send_buffer[n++]='7';//日
					
					Send_buffer[n++]='1';Send_buffer[n++]='2';//周
					if(((ds_dat_time[5]&0x0f)==7)||((ds_dat_time[5]&0x0f)==0)){Send_buffer[n++]='0';Send_buffer[n++]='7';}
					else {Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[5]&0x0f)+48;}
					
					Send_buffer[n++]='0';Send_buffer[n++]='8';//农历
					Send_buffer[n++]='A';Send_buffer[n++]=(moon_year%10)+48;
					Send_buffer[n++]='B';Send_buffer[n++]=(moon_year%12)+48;
					Send_buffer[n++]='0';Send_buffer[n++]='5';//年
					if(moon_month==0x01){Send_buffer[n++]='1';Send_buffer[n++]='4';}//  正月
					else if(moon_month==0x11){Send_buffer[n++]='0';Send_buffer[n++]='9';Send_buffer[n++]='0';Send_buffer[n++]='6';}//冬月
					else if(moon_month==0x12){Send_buffer[n++]='1';Send_buffer[n++]='0';Send_buffer[n++]='0';Send_buffer[n++]='6';}//腊月
					else if(moon_month){Send_buffer[n++]='X';Send_buffer[n++]=moon_month+48;Send_buffer[n++]='0';Send_buffer[n++]='6';}
					
					if(((moon_day>>4)==0)||((moon_day>>4)==1)&&((moon_day&0x0f)==0)){Send_buffer[n++]='1';Send_buffer[n++]='1';}//初
					if((moon_day>>4)==1){Send_buffer[n++]='X';Send_buffer[n++]='A';}//十
					if(((moon_day>>4)==2)&&((moon_day&0x0f)==0)){Send_buffer[n++]='X';Send_buffer[n++]='2';Send_buffer[n++]='X';Send_buffer[n++]='A';}//二十
					else if((moon_day>>4)==2){Send_buffer[n++]='1';Send_buffer[n++]='5';}//廿
					if((moon_day>>4)==3){Send_buffer[n++]='X';Send_buffer[n++]='3';Send_buffer[n++]='X';Send_buffer[n++]='A';}
					if(moon_day&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(moon_day&0x0f)+48;}
					
					Send_buffer[2]=n-3;
					Send_buffer[n]=0;
					for(i=0;i<n;i++)
					Send_buffer[n]+=Send_buffer[i];
					UartPutStr(Send_buffer,n+1);
}
void Tell_theTime()
{
					uchar i,n;
					n=0;
					Send_buffer[n++]=0xAA;
					Send_buffer[n++]=0x1B;//组合播放指令码;
					Send_buffer[n++]=0x00;//长度
	
					Send_buffer[n++]='0';
					Send_buffer[n++]='1';
					if((ds_dat_time[2]>>4)==0)
					{Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[2]&0x0f)+48;}
					if((ds_dat_time[2]>>4)==1)
					{
						Send_buffer[n++]='X';Send_buffer[n++]='A';
						if(ds_dat_time[2]&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[2]&0x0f)+48;}
					}
					if((ds_dat_time[2]>>4)==2)
					{
						Send_buffer[n++]='X';Send_buffer[n++]='2';
						Send_buffer[n++]='X';Send_buffer[n++]='A';
						if(ds_dat_time[2]&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[2]&0x0f)+48;}
					}
					Send_buffer[n++]='1';Send_buffer[n++]='6';//点
					
					if(ds_dat_time[1]||ds_dat_time[0])
					{
					if((ds_dat_time[1]>>4)==0)
					{Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[1]&0x0f)+48;}
					if((ds_dat_time[1]>>4)==1)
					{
						Send_buffer[n++]='X';Send_buffer[n++]='A';
						if(ds_dat_time[1]&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[1]&0x0f)+48;}
					}
					if((ds_dat_time[1]>>4)>1)
					{
						Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[1]>>4)+48;
						Send_buffer[n++]='X';Send_buffer[n++]='A';
						if(ds_dat_time[1]&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[1]&0x0f)+48;}
					}
					Send_buffer[n++]='0';Send_buffer[n++]='3';// 分
					if((ds_dat_time[0]>>4)==0)
					{Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[0]&0x0f)+48;}
					if((ds_dat_time[0]>>4)==1)
					{
						Send_buffer[n++]='X';Send_buffer[n++]='A';
						if(ds_dat_time[0]&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[0]&0x0f)+48;}
					}
					if((ds_dat_time[0]>>4)>1)
					{
						Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[0]>>4)+48;
						Send_buffer[n++]='X';Send_buffer[n++]='A';
						if(ds_dat_time[0]&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[0]&0x0f)+48;}
					}
					Send_buffer[n++]='0';Send_buffer[n++]='4';//秒
					}
					else
					{
					Send_buffer[n++]='1';Send_buffer[n++]='7';//整
					}
//					Send_buffer[n++]='1';Send_buffer[n++]='3';
//					Send_buffer[n++]='X';Send_buffer[n++]='2';
//					Send_buffer[n++]='X';Send_buffer[n++]='0';
//					Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[6]>>4)+48;
//					Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[6]&0x0f)+48;
//					Send_buffer[n++]='0';Send_buffer[n++]='5';//年
//					
//					if((ds_dat_time[4]>>4)==0)
//					{Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[4]&0x0f)+48;}
//					if((ds_dat_time[4]>>4)==1)
//					{
//						Send_buffer[n++]='X';Send_buffer[n++]='A';
//						if(ds_dat_time[4]&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[4]&0x0f)+48;}
//					}
//					if((ds_dat_time[4]>>4)>1)
//					{
//						Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[4]>>4)+48;
//						Send_buffer[n++]='X';Send_buffer[n++]='A';
//						if(ds_dat_time[4]&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[4]&0x0f)+48;}
//					}
//					Send_buffer[n++]='0';Send_buffer[n++]='6';//月
//					
//					if((ds_dat_time[3]>>4)==0)
//					{Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[3]&0x0f)+48;}
//					if((ds_dat_time[3]>>4)==1)
//					{
//						Send_buffer[n++]='X';Send_buffer[n++]='A';
//						if(ds_dat_time[3]&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[3]&0x0f)+48;}//确保不是零
//					}
//					if((ds_dat_time[3]>>4)>1)
//					{
//						Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[3]>>4)+48;
//						Send_buffer[n++]='X';Send_buffer[n++]='A';
//						if(ds_dat_time[3]&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[3]&0x0f)+48;}
//					}
//					Send_buffer[n++]='0';Send_buffer[n++]='7';//日
//					
//					Send_buffer[n++]='1';Send_buffer[n++]='2';//周
//					Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[5]&0x0f)+48;
//					
//					Send_buffer[n++]='0';Send_buffer[n++]='8';//周
//					Send_buffer[n++]='A';Send_buffer[n++]=(moon_year/12)+48;
//					Send_buffer[n++]='B';Send_buffer[n++]=(moon_year%12)+48;
//					Send_buffer[n++]='0';Send_buffer[n++]='5';//年
//					if(moon_month==0x01){Send_buffer[n++]='1';Send_buffer[n++]='4';}//  正月
//					else if(moon_month==0x11){Send_buffer[n++]='1';Send_buffer[n++]='0';Send_buffer[n++]='0';Send_buffer[n++]='6';}//腊月
//					else if(moon_month==0x12){Send_buffer[n++]='0';Send_buffer[n++]='9';Send_buffer[n++]='0';Send_buffer[n++]='6';}//冬月
//					else if(moon_month){Send_buffer[n++]='X';Send_buffer[n++]=moon_month+48;Send_buffer[n++]='0';Send_buffer[n++]='6';}
//					
//					if((moon_day>>4)==0){Send_buffer[n++]='1';Send_buffer[n++]='1';}//初
//					if((moon_day>>4)==1){Send_buffer[n++]='X';Send_buffer[n++]='A';}//十
//					if((moon_day>>4)==2){Send_buffer[n++]='1';Send_buffer[n++]='5';}//廿
//					if((moon_day>>4)==3){Send_buffer[n++]='X';Send_buffer[n++]='3';Send_buffer[n++]='X';Send_buffer[n++]='A';}
//					if(moon_day&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(moon_day&0x0f)+48;}
										
					Send_buffer[2]=n-3;
					Send_buffer[n]=0;
					for(i=0;i<n;i++)
					Send_buffer[n]+=Send_buffer[i];
					UartPutStr(Send_buffer,n+1);
					
}