#include "Uart.h "
#include "Jq8900.h"
uchar *Ti_Str=0;
uchar Ti_Len=0;
uchar Ti_datLen=0;
uchar TiBUSY=0;
uchar Send_buffer_Str;
uchar xdata Send_buffer[40];//���ͻ����

uchar RiStr=0;
uchar Ri_len=0;
uchar RiBUSY=0;
uchar xdata Receive_buffer[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//���ջ����
uchar UartError=0;

extern uchar Jq_SmartCorrection_Sec;
void UartPutStr ( uchar *Uc_Str,uchar Uc_Len )
{
	SBUF=Send_buffer[0];
	Ti_Str=Uc_Str+1;	//����ָ��
	Ti_Len=Uc_Len-1;
	ES=1;						//�������ж�
	//TI=1;						//�����ж�.���͵�һ������

}

uchar Uart_JqDataValidation()
{
	uchar i;
	uchar CheckValue=0;
	if(Receive_buffer[2]<6){for(i=0;i<(Receive_buffer[2]+3);i++)CheckValue+=Receive_buffer[i];UartError|=0x04;}
	else {UartError|=0x40;return 0;}//��ֹ���
	if(Receive_buffer[Receive_buffer[2]+3]==CheckValue){UartError|=0x08;return 1;}
	else {UartError|=0x80;return 0;}
}

void Uart_DataSearch()//������������
{
	if(Uart_JqDataValidation())//���Σ��޷�ͨ��
	{
		switch(Receive_buffer[1])
		{
			case 0x01:{RiPlaybackStatus=Receive_buffer[3];break;}//��ѯ����״̬
			case 0x24:
			{
				RiTotalTrackTlaybackTime[0]=Receive_buffer[3];
				RiTotalTrackTlaybackTime[1]=Receive_buffer[4];
				RiTotalTrackTlaybackTime[2]=Receive_buffer[5];
				break;
			}//��Ŀ������ʱ��
			case 0x25:
			{
				RiCurrentPlayingTime[0]=Receive_buffer[3];
				RiCurrentPlayingTime[1]=Receive_buffer[4];
				RiCurrentPlayingTime[2]=Receive_buffer[5];
				Jq_SmartCorrection_Sec=Receive_buffer[5];
				break;
			}//��ǰ����ʱ��
			case 0x0C:
			{
				RiTotalTrack=((uint)Receive_buffer[3]<<8);
				RiTotalTrack|=(uint)Receive_buffer[4];
				break;
			}//����Ŀ
			case 0x0D:
			{
				RiCurrentTrack=(((uint)Receive_buffer[3])<<8);
				RiCurrentTrack|=(uint)Receive_buffer[4];
				break;
			}//��ǰ��Ŀ
			default :break;
		}
	}
//	switch(RiStr)
//		{
//			case 0:if(SBUF==0xAA)RiBUSY=1;else {RiStr=0;RiBUSY=0;}break;//���
//			case 1:
//			{
//				switch(SBUF)
//				{
//					case 0x01:{Receive_buffer_num=0;break;}//��ѯ����״̬
//					case 0x24:{Receive_buffer_num=1;break;}//��Ŀ������ʱ��
//					case 0x25:{Receive_buffer_num=2;break;}//��ǰ����ʱ��
//					case 0x1E:{Receive_buffer_num=3;break;}//��������
//					case 0x11:{Receive_buffer_num=4;break;}//�ļ���������Ŀ
//					case 0x12:{Receive_buffer_num=5;break;}//�ļ�Ŀ¼����Ŀ
//					case 0x0C:{Receive_buffer_num=6;break;}//����Ŀ
//					case 0x0D:{Receive_buffer_num=7;break;}//��ǰ��Ŀ
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
		if(Ti_Len){SBUF=*Ti_Str;Ti_Str++;Ti_Len--;TiBUSY=1;}//*Ti_Str����һ������=0xAAʱ����
		else TiBUSY=0;
	}
	
  if ( RI )
  {
		RI  = 0;
		if(SBUF==0xAA){RiBUSY=1;RiStr=0;UartError|=0x02;}//else {UartError|=0x20;return;}
//		if(RiBUSY==0)
//		{
//			RiStr=0;
//			if(SBUF==0xAA){RiBUSY=1;UartError|=0x02;}else {UartError|=0x20;return;}//��һλ����ʼλ�ſ�ʼ���գ������һ�����ݲ�����ʼλ����
//		}
		if(RiBUSY)
		{
		Receive_buffer[RiStr++]=SBUF;
		if(RiStr>2)if(Receive_buffer[2]<6){if(RiStr>(Receive_buffer[2]+3)){RiBUSY=0;RiStr=0;Uart_DataSearch();}}//���ս�����������������
							 else {RiBUSY=0;RiStr=0;UartError|=0x10;return;}
		if(RiStr>10){RiBUSY=0;RiStr=0;UartError&=0xFE;return;}
		}
	}
		
		
		//if(n>2)if(n==Receive_buffer[Receive_buffer_num][2]+3+1){n=0;RiBUSY=0;}
		//Ri_Adr++;
}

/*****************************************************************************
 �� �� ��  : UartInit
 ��������  : ���ڳ�ʹ��
 �������  : void
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��3��22��
    ��    ��   : Blueice
    �޸�����   : �����ɺ���

*****************************************************************************/
void Uart_Init ( void )
{
//		TMOD |= 0x20;
//    SCON = 0x50;
//   TL1 = 0xFA;		//�趨��ʱ��ֵ
//	TH1 = 0xFA;		//�趨��ʱ����װֵ
//    //PCON = 0x00;
PCON &= 0x7F;		//�����ʲ�����
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR &= 0xBF;		//��ʱ��1ʱ��ΪFosc/12,��12T
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//�����ʱ��1ģʽλ
	TMOD |= 0x20;		//�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
	TL1 = 0xFA;		//�趨��ʱ��ֵ
	TH1 = 0xFA;		//�趨��ʱ����װֵ
    EA = 1;
		ET1=0;
    ES = 1;
    TR1 = 1;
}
