#include "Uart.h "
#include "Jq8900.h"
uchar *Ti_Str=0;
uchar Ti_Len=0;
uchar Ti_datLen=0;
uchar TiBUSY=0;
uchar Send_buffer_Str;
uchar xdata Send_buffer[40];//发送缓冲池

uchar RiStr=0;
uchar Ri_len=0;
uchar RiBUSY=0;
uchar xdata Receive_buffer[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//接收缓冲池
uchar UartError=0;

extern uchar Jq_SmartCorrection_Sec;
void UartPutStr ( uchar *Uc_Str,uchar Uc_Len )
{
	SBUF=Send_buffer[0];
	Ti_Str=Uc_Str+1;	//传递指针
	Ti_Len=Uc_Len-1;
	ES=1;						//开串口中断
	//TI=1;						//触发中断.发送第一个数据

}

uchar Uart_JqDataValidation()
{
	uchar i;
	uchar CheckValue=0;
	if(Receive_buffer[2]<6){for(i=0;i<(Receive_buffer[2]+3);i++)CheckValue+=Receive_buffer[i];UartError|=0x04;}
	else {UartError|=0x40;return 0;}//防止溢出
	if(Receive_buffer[Receive_buffer[2]+3]==CheckValue){UartError|=0x08;return 1;}
	else {UartError|=0x80;return 0;}
}

void Uart_DataSearch()//索检有用数据
{
	if(Uart_JqDataValidation())//屏蔽，无法通过
	{
		switch(Receive_buffer[1])
		{
			case 0x01:{RiPlaybackStatus=Receive_buffer[3];break;}//查询播放状态
			case 0x24:
			{
				RiTotalTrackTlaybackTime[0]=Receive_buffer[3];
				RiTotalTrackTlaybackTime[1]=Receive_buffer[4];
				RiTotalTrackTlaybackTime[2]=Receive_buffer[5];
				break;
			}//曲目播放总时间
			case 0x25:
			{
				RiCurrentPlayingTime[0]=Receive_buffer[3];
				RiCurrentPlayingTime[1]=Receive_buffer[4];
				RiCurrentPlayingTime[2]=Receive_buffer[5];
				Jq_SmartCorrection_Sec=Receive_buffer[5];
				break;
			}//当前播放时间
			case 0x0C:
			{
				RiTotalTrack=((uint)Receive_buffer[3]<<8);
				RiTotalTrack|=(uint)Receive_buffer[4];
				break;
			}//总曲目
			case 0x0D:
			{
				RiCurrentTrack=(((uint)Receive_buffer[3])<<8);
				RiCurrentTrack|=(uint)Receive_buffer[4];
				break;
			}//当前曲目
			default :break;
		}
	}
//	switch(RiStr)
//		{
//			case 0:if(SBUF==0xAA)RiBUSY=1;else {RiStr=0;RiBUSY=0;}break;//检错
//			case 1:
//			{
//				switch(SBUF)
//				{
//					case 0x01:{Receive_buffer_num=0;break;}//查询播放状态
//					case 0x24:{Receive_buffer_num=1;break;}//曲目播放总时间
//					case 0x25:{Receive_buffer_num=2;break;}//当前播放时间
//					case 0x1E:{Receive_buffer_num=3;break;}//歌曲名称
//					case 0x11:{Receive_buffer_num=4;break;}//文件夹首曲曲目
//					case 0x12:{Receive_buffer_num=5;break;}//文件目录总曲目
//					case 0x0C:{Receive_buffer_num=6;break;}//总曲目
//					case 0x0D:{Receive_buffer_num=7;break;}//当前曲目
//					default  :RiStr=0;break;
//				}
//				break;
//			}
//			case 2:{Ri_len=SBUF;Ti_datLen=Ri_len;break;}
//			case 3:{if(Ri_len){Receive_buffer[Receive_buffer_num][0]=SBUF;Ri_len--;}break;}
//			case 4:{if(Ri_len){Receive_buffer[Receive_buffer_num][1]=SBUF;Ri_len--;}break;}
//			case 5:{if(Ri_len){Receive_buffer[Receive_buffer_num][2]=SBUF;Ri_len--;}break;}
//			case 6:{if(Ri_len){Receive_buffer[Receive_buffer_num][3]=SBUF;Ri_len--;}break;}
//			default  :break;
//		}
}

void Uart_TRX_ISR ( void ) interrupt 4
{
	if(TI)
	{
		TI=0;
		if(Ti_Len){SBUF=*Ti_Str;Ti_Str++;Ti_Len--;TiBUSY=1;}//*Ti_Str的下一个数据=0xAA时结束
		else TiBUSY=0;
	}
	
  if ( RI )
  {
		RI  = 0;
		if(SBUF==0xAA){RiBUSY=1;RiStr=0;UartError|=0x02;}//else {UartError|=0x20;return;}
//		if(RiBUSY==0)
//		{
//			RiStr=0;
//			if(SBUF==0xAA){RiBUSY=1;UartError|=0x02;}else {UartError|=0x20;return;}//第一位是起始位才开始接收，如果第一个数据不是起始位丢弃
//		}
		if(RiBUSY)
		{
		Receive_buffer[RiStr++]=SBUF;
		if(RiStr>2)if(Receive_buffer[2]<6){if(RiStr>(Receive_buffer[2]+3)){RiBUSY=0;RiStr=0;Uart_DataSearch();}}//接收结束，进行数据索检
							 else {RiBUSY=0;RiStr=0;UartError|=0x10;return;}
		if(RiStr>10){RiBUSY=0;RiStr=0;UartError&=0xFE;return;}
		}
	}
		
		
		//if(n>2)if(n==Receive_buffer[Receive_buffer_num][2]+3+1){n=0;RiBUSY=0;}
		//Ri_Adr++;
}

/*****************************************************************************
 函 数 名  : UartInit
 功能描述  : 串口初使化
 输入参数  : void
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年3月22日
    作    者   : Blueice
    修改内容   : 新生成函数

*****************************************************************************/
void Uart_Init ( void )
{
//		TMOD |= 0x20;
//    SCON = 0x50;
//   TL1 = 0xFA;		//设定定时初值
//	TH1 = 0xFA;		//设定定时器重装值
//    //PCON = 0x00;
PCON &= 0x7F;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	AUXR &= 0xBF;		//定时器1时钟为Fosc/12,即12T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//清除定时器1模式位
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TL1 = 0xFA;		//设定定时初值
	TH1 = 0xFA;		//设定定时器重装值
    EA = 1;
		ET1=0;
    ES = 1;
    TR1 = 1;
}
