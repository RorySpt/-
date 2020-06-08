#include "jq8900.h"
#include "uart.h"
#include "ds1302.h"
#include "calendar.h"

uchar RiPlaybackStatus=0;//����״̬
uchar RiTotalTrackTlaybackTime[3]={0,0,0};//��Ŀ��ʱ�䣬ʱ����
uchar RiCurrentPlayingTime[3]={0,0,0};//��Ŀ�Ѳ���ʱ��
uint RiTotalTrack=0;//����Ŀ
uint RiCurrentTrack=0;//��ǰ��Ŀ

//uchar code JqCmdCode_groupPlay[]=
//{
//	0xAA,0x1B
//};//
extern uchar Receive_buffer_num;
//extern uchar xdata Receive_buffer[][];//��һλ
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

void JqPlaySpecifiedTrack(uint dat)//����ָ����Ŀ0~65535
{
	uchar n,i;
	n=0;
	Send_buffer[n++]=0xAA;
	Send_buffer[n++]=0x07;//��ϲ���ָ����;
	Send_buffer[n++]=0x02;//����
	Send_buffer[n++]=(uchar)(dat/0xff);
	Send_buffer[n++]=(uchar)(dat%0xff);
	Send_buffer[n]=0;
	for(i=0;i<n;i++)
	Send_buffer[n]+=Send_buffer[i];
	UartPutStr(Send_buffer,n+1);
}
void JqSpecify_volume(uchar dat)//ָ������
{
	uchar n,i;
	n=0;
	Send_buffer[n++]=0xAA;
	Send_buffer[n++]=0x13;
	Send_buffer[n++]=0x01;//����
	Send_buffer[n++]=dat;
	Send_buffer[n]=0;
	for(i=0;i<n;i++)
	Send_buffer[n]+=Send_buffer[i];
	UartPutStr(Send_buffer,n+1);
}
//***************************************
//�������ܣ�����ѭ��ģʽ
//ָ��		��00��ȫ��ѭ����01������ѭ����02������ֹͣ
//					03��ȫ�������04��Ŀ¼ѭ����05��Ŀ¼���
//					06��Ŀ¼˳��07��ȫ��˳��
//***************************************
void JqSetLoopMode(uchar dat)//
{
	uchar n,i;
	n=0;
	Send_buffer[n++]=0xAA;
	Send_buffer[n++]=0x18;//��ϲ���ָ����;
	Send_buffer[n++]=0x01;//����
	Send_buffer[n++]=dat;
	Send_buffer[n]=0;
	for(i=0;i<n;i++)
	Send_buffer[n]+=Send_buffer[i];
	UartPutStr(Send_buffer,n+1);
}
void JqPlayCmd2(uchar dat)//02:���ţ�03����ͣ��04��ֹͣ��05����һ����06����һ��
{
	uchar n,i;
	n=0;
	Send_buffer[n++]=0xAA;
	switch(dat)
	{
		case 0:{Send_buffer[n++]=0x02;break;}//����
		case 1:{Send_buffer[n++]=0x03;break;}//��ͣ
		case 2:{Send_buffer[n++]=0x04;break;}//ֹͣ
		case 3:{Send_buffer[n++]=0x05;break;}//��һ��
		case 4:{Send_buffer[n++]=0x06;break;}//��һ��
		case 5:{Send_buffer[n++]=0x07;break;}//��������
		case 6:{Send_buffer[n++]=0x14;break;}//������
		case 7:{Send_buffer[n++]=0x15;break;}//������
		case 8:{Send_buffer[n++]=0x25;break;}//��ʼ��ѯʱ��
		default  :return;
	}
	Send_buffer[n++]=0x00;
	Send_buffer[n]=0;
	for(i=0;i<n;i++)
	Send_buffer[n]+=Send_buffer[i];
	UartPutStr(Send_buffer,n+1);
}

void JqPlayCmd(uchar dat)//02:���ţ�03����ͣ��04��ֹͣ��05����һ����06����һ��////for task
{
	uchar n,i;
	n=0;
	Send_buffer[n++]=0xAA;
	switch(dat)
	{
		case 0:{Send_buffer[n++]=0x02;break;}//����
		case 1:{Send_buffer[n++]=0x03;break;}//��ͣ
		case 2:{Send_buffer[n++]=0x04;break;}//ֹͣ
		case 3:{Send_buffer[n++]=0x05;break;}//��һ��
		case 4:{Send_buffer[n++]=0x06;break;}//��һ��
		case 5:{Send_buffer[n++]=0x26;break;}//ֹͣ��ѯʱ��
		case 6:{Send_buffer[n++]=0x14;break;}//������
		case 7:{Send_buffer[n++]=0x15;break;}//������
		
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
	//��ϲ���ָ����;
	switch(dat)
	{
		case 0:{Send_buffer[n++]=0x01;break;}//��ѯ����״̬
		case 1:{Send_buffer[n++]=0x24;break;}//��Ŀ������ʱ��
		case 2:{Send_buffer[n++]=0x25;break;}//��ǰ����ʱ��
//		case 3:{Send_buffer[n++]=0x1E;break;}//��������
		case 4:{Send_buffer[n++]=0x11;break;}//�ļ���������Ŀ
		case 5:{Send_buffer[n++]=0x12;break;}//�ļ�Ŀ¼����Ŀ
		case 6:{Send_buffer[n++]=0x0C;break;}//����Ŀ
		case 7:{Send_buffer[n++]=0x0D;break;}//��ǰ��Ŀ
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
	Send_buffer[n++]=0x01;//��ϲ���ָ����;
	Send_buffer[n++]=0x00;//����
	Send_buffer[n]=0xAB;
	UartPutStr(Send_buffer,n+1);
	
}

void Tell_theData()
{
					uchar i,n;
					n=0;
					Send_buffer[n++]=0xAA;
					Send_buffer[n++]=0x1B;//��ϲ���ָ����;
					Send_buffer[n++]=0x00;//����
					Send_buffer[n++]='1';Send_buffer[n++]='3';
					Send_buffer[n++]='X';Send_buffer[n++]='2';
					Send_buffer[n++]='X';Send_buffer[n++]='0';
					Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[6]>>4)+48;
					Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[6]&0x0f)+48;
					Send_buffer[n++]='0';Send_buffer[n++]='5';//��
					
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
					Send_buffer[n++]='0';Send_buffer[n++]='6';//��
					
					if((ds_dat_time[3]>>4)==0)
					{Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[3]&0x0f)+48;}
					if((ds_dat_time[3]>>4)==1)
					{
						Send_buffer[n++]='X';Send_buffer[n++]='A';
						if(ds_dat_time[3]&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[3]&0x0f)+48;}//ȷ��������
					}
					if((ds_dat_time[3]>>4)>1)
					{
						Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[3]>>4)+48;
						Send_buffer[n++]='X';Send_buffer[n++]='A';
						if(ds_dat_time[3]&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[3]&0x0f)+48;}
					}
					Send_buffer[n++]='0';Send_buffer[n++]='7';//��
					
					Send_buffer[n++]='1';Send_buffer[n++]='2';//��
					if(((ds_dat_time[5]&0x0f)==7)||((ds_dat_time[5]&0x0f)==0)){Send_buffer[n++]='0';Send_buffer[n++]='7';}
					else {Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[5]&0x0f)+48;}
					
					Send_buffer[n++]='0';Send_buffer[n++]='8';//ũ��
					Send_buffer[n++]='A';Send_buffer[n++]=(moon_year%10)+48;
					Send_buffer[n++]='B';Send_buffer[n++]=(moon_year%12)+48;
					Send_buffer[n++]='0';Send_buffer[n++]='5';//��
					if(moon_month==0x01){Send_buffer[n++]='1';Send_buffer[n++]='4';}//  ����
					else if(moon_month==0x11){Send_buffer[n++]='0';Send_buffer[n++]='9';Send_buffer[n++]='0';Send_buffer[n++]='6';}//����
					else if(moon_month==0x12){Send_buffer[n++]='1';Send_buffer[n++]='0';Send_buffer[n++]='0';Send_buffer[n++]='6';}//����
					else if(moon_month){Send_buffer[n++]='X';Send_buffer[n++]=moon_month+48;Send_buffer[n++]='0';Send_buffer[n++]='6';}
					
					if(((moon_day>>4)==0)||((moon_day>>4)==1)&&((moon_day&0x0f)==0)){Send_buffer[n++]='1';Send_buffer[n++]='1';}//��
					if((moon_day>>4)==1){Send_buffer[n++]='X';Send_buffer[n++]='A';}//ʮ
					if(((moon_day>>4)==2)&&((moon_day&0x0f)==0)){Send_buffer[n++]='X';Send_buffer[n++]='2';Send_buffer[n++]='X';Send_buffer[n++]='A';}//��ʮ
					else if((moon_day>>4)==2){Send_buffer[n++]='1';Send_buffer[n++]='5';}//إ
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
					Send_buffer[n++]=0x1B;//��ϲ���ָ����;
					Send_buffer[n++]=0x00;//����
	
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
					Send_buffer[n++]='1';Send_buffer[n++]='6';//��
					
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
					Send_buffer[n++]='0';Send_buffer[n++]='3';// ��
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
					Send_buffer[n++]='0';Send_buffer[n++]='4';//��
					}
					else
					{
					Send_buffer[n++]='1';Send_buffer[n++]='7';//��
					}
//					Send_buffer[n++]='1';Send_buffer[n++]='3';
//					Send_buffer[n++]='X';Send_buffer[n++]='2';
//					Send_buffer[n++]='X';Send_buffer[n++]='0';
//					Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[6]>>4)+48;
//					Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[6]&0x0f)+48;
//					Send_buffer[n++]='0';Send_buffer[n++]='5';//��
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
//					Send_buffer[n++]='0';Send_buffer[n++]='6';//��
//					
//					if((ds_dat_time[3]>>4)==0)
//					{Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[3]&0x0f)+48;}
//					if((ds_dat_time[3]>>4)==1)
//					{
//						Send_buffer[n++]='X';Send_buffer[n++]='A';
//						if(ds_dat_time[3]&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[3]&0x0f)+48;}//ȷ��������
//					}
//					if((ds_dat_time[3]>>4)>1)
//					{
//						Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[3]>>4)+48;
//						Send_buffer[n++]='X';Send_buffer[n++]='A';
//						if(ds_dat_time[3]&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[3]&0x0f)+48;}
//					}
//					Send_buffer[n++]='0';Send_buffer[n++]='7';//��
//					
//					Send_buffer[n++]='1';Send_buffer[n++]='2';//��
//					Send_buffer[n++]='X';Send_buffer[n++]=(ds_dat_time[5]&0x0f)+48;
//					
//					Send_buffer[n++]='0';Send_buffer[n++]='8';//��
//					Send_buffer[n++]='A';Send_buffer[n++]=(moon_year/12)+48;
//					Send_buffer[n++]='B';Send_buffer[n++]=(moon_year%12)+48;
//					Send_buffer[n++]='0';Send_buffer[n++]='5';//��
//					if(moon_month==0x01){Send_buffer[n++]='1';Send_buffer[n++]='4';}//  ����
//					else if(moon_month==0x11){Send_buffer[n++]='1';Send_buffer[n++]='0';Send_buffer[n++]='0';Send_buffer[n++]='6';}//����
//					else if(moon_month==0x12){Send_buffer[n++]='0';Send_buffer[n++]='9';Send_buffer[n++]='0';Send_buffer[n++]='6';}//����
//					else if(moon_month){Send_buffer[n++]='X';Send_buffer[n++]=moon_month+48;Send_buffer[n++]='0';Send_buffer[n++]='6';}
//					
//					if((moon_day>>4)==0){Send_buffer[n++]='1';Send_buffer[n++]='1';}//��
//					if((moon_day>>4)==1){Send_buffer[n++]='X';Send_buffer[n++]='A';}//ʮ
//					if((moon_day>>4)==2){Send_buffer[n++]='1';Send_buffer[n++]='5';}//إ
//					if((moon_day>>4)==3){Send_buffer[n++]='X';Send_buffer[n++]='3';Send_buffer[n++]='X';Send_buffer[n++]='A';}
//					if(moon_day&0x0f){Send_buffer[n++]='X';Send_buffer[n++]=(moon_day&0x0f)+48;}
										
					Send_buffer[2]=n-3;
					Send_buffer[n]=0;
					for(i=0;i<n;i++)
					Send_buffer[n]+=Send_buffer[i];
					UartPutStr(Send_buffer,n+1);
					
}