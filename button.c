#include "button.h"
#include "ds1302.h"
#include "calendar.h"
#include "uart.h"
#include "jq8900.h"
#include "game.h"
//#define KEY(i) KEY##i
uchar data Command_key[4];//
uchar data Prestate_key[4];//按键前状态
uchar data Curstatus_key[4];//按键当前状态
uchar data Instruction_code_key[4];
uint data Double_Press_counter[4];//长按计数器
uint data Double_Idle_counter[4];//空闲计数器
//uint data Double_idle_counter[4];//空闲标志

uchar DoubleClick_trigger[4]={0,0,0,0};

uchar bdata Refill_sign;//如果按键抬起，标志位置1，按下置零。可以用于完成短按。
sbit Refill_sign_0=Refill_sign^0;
sbit Refill_sign_1=Refill_sign^1;
sbit Refill_sign_2=Refill_sign^2;
sbit Refill_sign_3=Refill_sign^3;
sbit Refill_sign_4=Refill_sign^4;
sbit Refill_sign_5=Refill_sign^5;
sbit Refill_sign_6=Refill_sign^6;
sbit Refill_sign_7=Refill_sign^7;

#define button_BurstSpeed 4//按键连发的速率，计算公式为按键，采样率/button_BurstSpeed，默认采样率为10次/秒。

#define In 									(!Curstatus_key[0])	//确认触发中
#define Back 								(!Curstatus_key[1])	//返回触发中
#define Up 									(!Curstatus_key[2])	//上触发中
#define Down 								(!Curstatus_key[3])	//下触发中

#define goIn 								(Instruction_code_key[0]==0x80)									//确认键单击
#define goBack 							(Instruction_code_key[1]==0x80)									//返回键单击
#define	goUp								(Instruction_code_key[2]==0x80)									//上键单击
#define	goDown							(Instruction_code_key[3]==0x80)									//下键单击
#define BurstGoin  					((Double_Press_counter[0]%button_BurstSpeed)==3)//确认键连发，速度5次每秒
#define BurstGoback 				((Double_Press_counter[1]%button_BurstSpeed)==3)//返回键连发
#define BurstGoup 					((Double_Press_counter[2]%button_BurstSpeed)==3)//上键连发
#define BurstGodown					((Double_Press_counter[3]%button_BurstSpeed)==3)//下键连发

#define DoubleClickIn				(DoubleClick(0))	//确认双击
#define DoubleClickBack			(DoubleClick(1))	
#define DoubleClickUp				(DoubleClick(2))	
#define DoubleClickDown			(DoubleClick(3))	
	
extern uchar data menuCode;
uchar OptionCode[]={0,5,6};			//子菜单选项码，0为菜单1，1为菜单10，2为菜单11.
uchar OptionCode_Max[]={6,5,4};	//子菜单选项上限。0~n。

extern uchar SwitchFlag[];			


extern uchar Sensor_switch[2];
extern uint rot_taskFrequency[];
extern uint Sensor_display_switchingSpeed;
extern uint Sensor_display_switchingTimer;
uchar Change_sign=0;						//用于触发修改确认选项			

//uchar SoundPlay_sign=0;					

//闹钟
#define TotalNumber_alarms 4		//闹钟个数
extern uchar AlarmClockNum;
//extern uchar AlarmClockNum_old=0;
extern uchar AlarmOptionSite;
//extern uchar AlarmOptionSite_old=0;
//extern uchar AlarmDataOptionSite_old=0;
extern uchar AlarmDat[TotalNumber_alarms][9];
//uchar Sensor_switch[2];

extern uchar JqSoundOption;
extern uchar AlarmClockMusicSet[4];
extern uchar JqHourlyTimeSwitch;
#define AlarmClockMusicMax 30
extern uchar DsHourOld;

extern uchar gameSpeed;
extern uchar gameSpeedSet;
uchar GreedySnakeMovementDirection_change;

extern uchar moreMenuCode;

extern uchar moreFunctionsOption;

extern uchar gameSnakeOption;
extern uchar gameSnakeSetOption;

uchar DoubleClick(uchar n)
{
	switch(DoubleClick_trigger[n])
	{
		case 0:if((Instruction_code_key[n]==0x80)/*||(Double_Press_counter[n]<(button_BurstSpeed+12))*/)DoubleClick_trigger[n]=1;return 0;
		
		case 1:if(Double_Press_counter[n]<(button_BurstSpeed+2)){if((Instruction_code_key[n]==0x81))DoubleClick_trigger[n]=2;}					
						else DoubleClick_trigger[n]=0;	return 0;
		case 2:if(Double_Idle_counter[n] <(button_BurstSpeed+2)){if(Instruction_code_key[n]==0x80){DoubleClick_trigger[n]=0;return 1;}}
						else DoubleClick_trigger[n]=0;	return 0;
		default :return 0;
	}
}	

void control_Section()					//控制函数				
{
	uchar n=0;
	switch(menuCode)
	{
		case 0:	
		{	
			if(goIn||goDown||goUp||goBack)
			{
				if(AlarmDat[0][0]>1){AlarmDat[0][0]-=2;}
				if(AlarmDat[1][0]>1){AlarmDat[1][0]-=2;}
				if(AlarmDat[2][0]>1){AlarmDat[2][0]-=2;}
				if(AlarmDat[3][0]>1){AlarmDat[3][0]-=2;}
			}
			if(goIn&&(!goBack)) {menuCode=1;Refill_sign_0=0;}	
			if(goDown&&Refill_sign_3&&(!goBack))
			{	
				if(SwitchFlag[0]<2)SwitchFlag[0]++;else SwitchFlag[0]=0;
				if((Sensor_switch[0]==0)&&(SwitchFlag[0]==0))SwitchFlag[0]++;
				if((Sensor_switch[1]==0)&&(SwitchFlag[0]==1))SwitchFlag[0]++;
				Sensor_display_switchingTimer=Sensor_display_switchingSpeed;//重装
				
			}
			if(goUp&&Refill_sign_2&&(!goDown)){if(SwitchFlag[1]<3)SwitchFlag[1]++;else SwitchFlag[1]=0;}
			if(goBack&&Refill_sign_1&&(!goDown))
				{
//					Send_buffer[0]=0xAA;
//					Send_buffer[1]=0x06;
//					Send_buffer[2]=0x00;
//					Send_buffer[3]=0xB0;
//					UartPutStr(Send_buffer,4);
					Tell_theTime();
//					//*JpAlarmMusic(n);
//					if(n<4)n++;else n=0;*/
					
				}
				if(goBack&&goDown&&Refill_sign_3)
				{
					Tell_theData();
				}
				if(goBack&&goUp&&Refill_sign_3)JqPlayCmd(2);//JqPlaySpecifiedTrack(1);
				//if(goIn&&goBack)
			//if(goUp&Refill_sign_2){if(ds_dat_time_set[3]<29)ds_dat_time_set[3]++;else ds_dat_time_set[3]=0;Ds1302_Set_time();}
		break;}
		case 1:
		{	if(goBack&Refill_sign_1){menuCode=0;Refill_sign_1=0;}
			if(goUp&Refill_sign_2){if(OptionCode[0]<OptionCode_Max[0])OptionCode[0]++;else OptionCode[0]=0;Refill_sign_2=0;}
			if(goDown&Refill_sign_3){if(OptionCode[0]>0)OptionCode[0]--; else OptionCode[0]=OptionCode_Max[0];Refill_sign_3=0;}
			if(goIn&Refill_sign_0){if(OptionCode[0]<7)menuCode=OptionCode[0]+10;else menuCode=10; Refill_sign_0=0;}
			break;
		}
		case 10:
		{
			if(goBack&Refill_sign_1){if(Change_sign==0)menuCode=1;if(Change_sign==1){menuCode=100;Change_sign=0;}}
			if(goIn&Refill_sign_0){if(OptionCode[1]<OptionCode_Max[1])OptionCode[1]++;else OptionCode[1]=0;}
			if(BurstGoup||(goUp&Refill_sign_2))
			{
				if(OptionCode[1]<6)Change_sign=1;
				 Ds1302_time_Copy(1);
				switch(OptionCode[1])
				{
					case 3:{if(ds_dat_time_set16[6]<99)ds_dat_time_set16[6]++;else ds_dat_time_set16[6]=0;}break;
					case 4:{if(ds_dat_time_set16[4]<12)ds_dat_time_set16[4]++;else ds_dat_time_set16[4]=1;}break;
					case 5:{if(ds_dat_time_set16[3]<Calculate_Totaldays_Ofmonth(ds_dat_time_set16[6],ds_dat_time_set16[4]))ds_dat_time_set16[3]++;else ds_dat_time_set16[3]=1;}break;
					case 0:{if(ds_dat_time_set16[2]<23)ds_dat_time_set16[2]++;else ds_dat_time_set16[2]=0;}break;
					case 1:{if(ds_dat_time_set16[1]<59)ds_dat_time_set16[1]++;else ds_dat_time_set16[1]=0;}break;
					case 2:{if(ds_dat_time_set16[0]<59)ds_dat_time_set16[0]++;else ds_dat_time_set16[0]=0;}break;
				}
				 Ds1302_time_Copy(2);
			}
			if(BurstGodown||(goDown&Refill_sign_3))
			{
				if(OptionCode[1]<6)Change_sign=1;//确保当Option处在正确的值时触发
				 Ds1302_time_Copy(1);
				switch(OptionCode[1])
				{
					case 3:{if(ds_dat_time_set16[6]>0)ds_dat_time_set16[6]--;else ds_dat_time_set16[6]=99;}break;
					case 4:{if(ds_dat_time_set16[4]>1)ds_dat_time_set16[4]--;else ds_dat_time_set16[4]=12;}break;
					case 5:{if(ds_dat_time_set16[3]>1)ds_dat_time_set16[3]--;else ds_dat_time_set16[3]=Calculate_Totaldays_Ofmonth(ds_dat_time_set16[6],ds_dat_time_set16[4]);}break;
					case 0:{if(ds_dat_time_set16[2]>0)ds_dat_time_set16[2]--;else ds_dat_time_set16[2]=23;}break;
					case 1:{if(ds_dat_time_set16[1]>0)ds_dat_time_set16[1]--;else ds_dat_time_set16[1]=59;}break;
					case 2:{if(ds_dat_time_set16[0]>0)ds_dat_time_set16[0]--;else ds_dat_time_set16[0]=59;}break;
				}
				 Ds1302_time_Copy(2);
			}
			
			break;
		}	
		case 100:
		{
			if(goIn&Refill_sign_0){Ds1302_Set_time();DsHourOld=ds_dat_time[2];menuCode=1;}
			if(goBack&Refill_sign_1){menuCode=1;}
			break;
		}
		case 11:
		{
			if(goBack&Refill_sign_1){menuCode=1;OptionCode[2]=10;if(Sensor_display_switchingTimer>Sensor_display_switchingSpeed)Sensor_display_switchingTimer=Sensor_display_switchingSpeed;}
			if(goIn&Refill_sign_0){if(OptionCode[2]<OptionCode_Max[2])OptionCode[2]++;else OptionCode[2]=0;}
			if(OptionCode[2]>5)
			{
				if(goUp&&Refill_sign_2)JqSetLoopMode(4);//目录循环
				if(goDown&&Refill_sign_3)JqSetLoopMode(5);//目录随机
			}
			if(BurstGoup||(goUp&Refill_sign_2))
			{
			switch(OptionCode[2])
				{
					case 0:if(Sensor_switch[0]<1)Sensor_switch[0]++;else Sensor_switch[0]=0;break;
					case 1:if(Sensor_switch[1]<1)Sensor_switch[1]++;else Sensor_switch[1]=0;break;
					case 2:if(rot_taskFrequency[0]<6000)rot_taskFrequency[0]+=100;else rot_taskFrequency[0]=100;break;
					case 3:if(rot_taskFrequency[1]<6000)rot_taskFrequency[1]+=100;else rot_taskFrequency[1]=100;break;
					case 4:if(Sensor_display_switchingSpeed<6000)Sensor_display_switchingSpeed+=100;else Sensor_display_switchingSpeed=100;break;
				}
			}
			if(BurstGodown||(goDown&Refill_sign_3))
			switch(OptionCode[2])
				{
					case 0:if(Sensor_switch[0]>0)Sensor_switch[0]--;else Sensor_switch[0]=1;break;
					case 1:if(Sensor_switch[1]>0)Sensor_switch[1]--;else Sensor_switch[1]=1;break;
					case 2:if(rot_taskFrequency[0]>100)rot_taskFrequency[0]-=100;else rot_taskFrequency[0]=6000;break;
					case 3:if(rot_taskFrequency[1]>100)rot_taskFrequency[1]-=100;else rot_taskFrequency[1]=6000;break;
					case 4:if(Sensor_display_switchingSpeed>200)Sensor_display_switchingSpeed-=100;else Sensor_display_switchingSpeed=6000;break;
				}
			break;
		}
		case 12:
		{
				if(AlarmOptionSite==0)
				{
					if(goBack&Refill_sign_1){menuCode=1;}
					if(goIn&&Refill_sign_0)if(AlarmClockNum<3)AlarmClockNum++;else AlarmClockNum=0;
					if((goUp&&Refill_sign_2||goDown&&Refill_sign_3)&&(AlarmClockNum<4)){Refill_sign_2=0;Refill_sign_3=0;AlarmOptionSite++;}
					if(AlarmClockNum>4)
					{
						if(goUp&&Refill_sign_2)JqSetLoopMode(1);//单曲循环
						if(goDown&&Refill_sign_3)JqSetLoopMode(2);//单曲停止
					}
				}
				if((AlarmOptionSite)>0)
				{
					if(goBack&&Refill_sign_1)AlarmOptionSite=0;
					if(goIn&&Refill_sign_0)
					{	Refill_sign_0=0;
						if(!AlarmDat[AlarmClockNum][8])if((AlarmOptionSite)<8)AlarmOptionSite++;else AlarmOptionSite=1;
						if(AlarmDat[AlarmClockNum][8])if((AlarmOptionSite)<12)AlarmOptionSite++;else AlarmOptionSite=1;
					}
					if(goUp&&Refill_sign_2||BurstGoup)
						switch(AlarmOptionSite)
						{
							case 1:if(goUp&&Refill_sign_2)AlarmDat[AlarmClockNum][0]=1-AlarmDat[AlarmClockNum][0];break;
							case 2:if(AlarmDat[AlarmClockNum][1]<23)AlarmDat[AlarmClockNum][1]++;else AlarmDat[AlarmClockNum][1]=0;break;
							case 3:if(AlarmDat[AlarmClockNum][2]<59)AlarmDat[AlarmClockNum][2]++;else AlarmDat[AlarmClockNum][2]=0;break;
							case 4:if(AlarmDat[AlarmClockNum][3]<59)AlarmDat[AlarmClockNum][3]++;else AlarmDat[AlarmClockNum][3]=0;break;
							case 5:if(goUp&&Refill_sign_2)AlarmDat[AlarmClockNum][8]=1-AlarmDat[AlarmClockNum][8];break;
							case 6:
							{
								if(!AlarmDat[AlarmClockNum][8])if(AlarmDat[AlarmClockNum][4]<99)AlarmDat[AlarmClockNum][4]++;else AlarmDat[AlarmClockNum][4]=0;
								if(AlarmDat[AlarmClockNum][8])if(goUp&&Refill_sign_2)if((AlarmDat[AlarmClockNum][7]&0x80)==0)AlarmDat[AlarmClockNum][7]|=0x80;else AlarmDat[AlarmClockNum][7]&=(0xff-0x80);
								break;
							}
							case 7:
							{
								if(!AlarmDat[AlarmClockNum][8])if(AlarmDat[AlarmClockNum][5]<12)AlarmDat[AlarmClockNum][5]++;else AlarmDat[AlarmClockNum][5]=1;
								if(AlarmDat[AlarmClockNum][8])if(goUp&&Refill_sign_2)if((AlarmDat[AlarmClockNum][7]&(0x80>>1))==0)AlarmDat[AlarmClockNum][7]|=(0x80>>1);else AlarmDat[AlarmClockNum][7]&=(0xff-(0x80>>1));
								break;
							}
							case 8:
							{	
								if(!AlarmDat[AlarmClockNum][8])if(AlarmDat[AlarmClockNum][6]<Calculate_Totaldays_Ofmonth(AlarmDat[AlarmClockNum][4],AlarmDat[AlarmClockNum][5]))AlarmDat[AlarmClockNum][6]++;else AlarmDat[AlarmClockNum][6]=1;
								if(AlarmDat[AlarmClockNum][8])if(goUp&&Refill_sign_2)if((AlarmDat[AlarmClockNum][7]&(0x80>>2))==0)AlarmDat[AlarmClockNum][7]|=(0x80>>2);else AlarmDat[AlarmClockNum][7]&=(0xff-(0x80>>2));
								break;
							}
							case 9:if(goUp&&Refill_sign_2)if((AlarmDat[AlarmClockNum][7]&(0x80>>3))==0)AlarmDat[AlarmClockNum][7]|=(0x80>>3);else AlarmDat[AlarmClockNum][7]&=(0xff-(0x80>>3));break;
							case 10:if(goUp&&Refill_sign_2)if((AlarmDat[AlarmClockNum][7]&(0x80>>4))==0)AlarmDat[AlarmClockNum][7]|=(0x80>>4);else AlarmDat[AlarmClockNum][7]&=(0xff-(0x80>>4));break;
							case 11:if(goUp&&Refill_sign_2)if((AlarmDat[AlarmClockNum][7]&(0x80>>5))==0)AlarmDat[AlarmClockNum][7]|=(0x80>>5);else AlarmDat[AlarmClockNum][7]&=(0xff-(0x80>>5));break;
							case 12:if(goUp&&Refill_sign_2)if((AlarmDat[AlarmClockNum][7]&(0x80>>6))==0)AlarmDat[AlarmClockNum][7]|=(0x80>>6);else AlarmDat[AlarmClockNum][7]&=(0xff-(0x80>>6));break;
						}
						if(goDown&&Refill_sign_3||BurstGodown)
						switch(AlarmOptionSite)
						{
							case 1:if(goDown&&Refill_sign_3)AlarmDat[AlarmClockNum][0]=1-AlarmDat[AlarmClockNum][0];break;
							case 2:if(AlarmDat[AlarmClockNum][1]>0)AlarmDat[AlarmClockNum][1]--;else AlarmDat[AlarmClockNum][1]=23;break;
							case 3:if(AlarmDat[AlarmClockNum][2]>0)AlarmDat[AlarmClockNum][2]--;else AlarmDat[AlarmClockNum][2]=59;break;
							case 4:if(AlarmDat[AlarmClockNum][3]>0)AlarmDat[AlarmClockNum][3]--;else AlarmDat[AlarmClockNum][3]=59;break;
							case 5:if(goDown&&Refill_sign_3)AlarmDat[AlarmClockNum][8]=1-AlarmDat[AlarmClockNum][8];break;
							case 6:
							{
								if(!AlarmDat[AlarmClockNum][8])if(AlarmDat[AlarmClockNum][4]>0)AlarmDat[AlarmClockNum][4]--;else AlarmDat[AlarmClockNum][4]=99;
								if(AlarmDat[AlarmClockNum][8])if(goDown&&Refill_sign_3)AlarmOptionSite--;//if((AlarmDat[AlarmClockNum][7]&0x80)==0)AlarmDat[AlarmClockNum][7]|=0x80;else AlarmDat[AlarmClockNum][7]&=(0xff-0x80);
								break;
							}
							case 7:
							{
								if(!AlarmDat[AlarmClockNum][8])if(AlarmDat[AlarmClockNum][5]>0)AlarmDat[AlarmClockNum][5]--;else AlarmDat[AlarmClockNum][5]=12;
								if(AlarmDat[AlarmClockNum][8])if(goDown&&Refill_sign_3)AlarmOptionSite--;//if((AlarmDat[AlarmClockNum][7]&(0x80>>1))==0)AlarmDat[AlarmClockNum][7]|=(0x80>>1);else AlarmDat[AlarmClockNum][7]&=(0xff-(0x80>>1));
								break;
							}
							case 8:
							{	
								if(!AlarmDat[AlarmClockNum][8])if(AlarmDat[AlarmClockNum][6]>0)AlarmDat[AlarmClockNum][6]--;else AlarmDat[AlarmClockNum][6]=Calculate_Totaldays_Ofmonth(AlarmDat[AlarmClockNum][4],AlarmDat[AlarmClockNum][5]);
								if(AlarmDat[AlarmClockNum][8])if(goDown&&Refill_sign_3)AlarmOptionSite--;//if((AlarmDat[AlarmClockNum][7]&(0x80>>2))==0)AlarmDat[AlarmClockNum][7]|=(0x80>>2);else AlarmDat[AlarmClockNum][7]&=(0xff-(0x80>>2));
								break;
							}
							case 9:AlarmOptionSite--;break;//if(goDown&&Refill_sign_3)if((AlarmDat[AlarmClockNum][7]&(0x80>>3))==0)AlarmDat[AlarmClockNum][7]|=(0x80>>3);else AlarmDat[AlarmClockNum][7]&=(0xff-(0x80>>3));break;
							case 10:AlarmOptionSite--;break;//if(goDown&&Refill_sign_3)if((AlarmDat[AlarmClockNum][7]&(0x80>>4))==0)AlarmDat[AlarmClockNum][7]|=(0x80>>4);else AlarmDat[AlarmClockNum][7]&=(0xff-(0x80>>4));break;
							case 11:AlarmOptionSite--;break;//if(goDown&&Refill_sign_3)if((AlarmDat[AlarmClockNum][7]&(0x80>>5))==0)AlarmDat[AlarmClockNum][7]|=(0x80>>5);else AlarmDat[AlarmClockNum][7]&=(0xff-(0x80>>5));break;
							case 12:AlarmOptionSite--;break;//if(goDown&&Refill_sign_3)if((AlarmDat[AlarmClockNum][7]&(0x80>>6))==0)AlarmDat[AlarmClockNum][7]|=(0x80>>6);else AlarmDat[AlarmClockNum][7]&=(0xff-(0x80>>6));break;
						}
						
				}
				break;
		}
		case 13:
		{
			if(DoubleClickBack&&(!In)&&(!Up)&&(!Down)){menuCode=1;JqPlayCmd(5);PS=0;}//双击返回键退出
			//if(goIn&&(!goBack)&&(!goDown)&&(!goUp)) {if(RiPlaybackStatus!=1)JqPlayCmd(0);else JqPlayCmd(1);}//确认键播放或者暂停	
			if(goIn&&(!Back)&&(!Up)&&(!Down)) {if(RiPlaybackStatus!=1)JqPlayCmd(0);else JqPlayCmd(1);}
//			if(goUp&&(!goIn)&&(!goDown)&&(!goBack)){JqPlayCmd(1);}
//			
//			if(goDown&&(!goIn)&&(!goUp)){JqPlayCmd(2);}
//			if(BurstGoin&&BurstGoup)JqPlaySpecifiedTrack(1);
//			if(BurstGoback&&BurstGodown)JqPlaySpecifiedTrack(1);
			if(goUp&&(In))JqPlaySpecifiedTrack(1);//确认键和上键同时按下回滚曲目
			//if(goDown&&goIn){JqPlayCmd(4);}
			
//			if(goBack&&goIn){JqPlayCmd(7);}
//			if(goUp&&goDown){JqPlayCmd(6);}
			if((goUp||BurstGoup)&&(!In))JqPlayCmd(4);//上键曲目加
			if(goDown||BurstGodown)JqPlayCmd(3);//下键曲目减
			
			if((!Curstatus_key[1])&&(goUp||BurstGoup)){JqPlayCmd(6);}//{JqPlayCmd(4);}//返回键+上键音量加
			if((!Curstatus_key[1])&&(goDown||BurstGodown)){JqPlayCmd(7);}//{JqPlayCmd(3);}//返回键+下键音量减
			if((!Curstatus_key[1])&&(goIn)){JqPlayCmd(2);}//返回键+确认键停止播放
//			if(BurstGoback&&BurstGoup||goBack&&goUp){JqPlayCmd(3);}
//			if(BurstGoin&&BurstGodown||goDown&&goIn){JqPlayCmd(4);}
			break;
		}
		case 14:
		{
			if(goBack&&(!goIn)&&(!goDown)){menuCode=1;}

			if(goIn){if(JqSoundOption<4)JqSoundOption++;else JqSoundOption=0;}
			if(goUp)
				switch(JqSoundOption)
				{
					case 0:JqHourlyTimeSwitch=~JqHourlyTimeSwitch;break;
					case 1:if(AlarmClockMusicSet[0]<AlarmClockMusicMax)AlarmClockMusicSet[0]++;else AlarmClockMusicSet[0]=1;break;
					case 2:if(AlarmClockMusicSet[1]<AlarmClockMusicMax)AlarmClockMusicSet[1]++;else AlarmClockMusicSet[1]=1;break;
					case 3:if(AlarmClockMusicSet[2]<AlarmClockMusicMax)AlarmClockMusicSet[2]++;else AlarmClockMusicSet[2]=1;break;
					case 4:if(AlarmClockMusicSet[3]<AlarmClockMusicMax)AlarmClockMusicSet[3]++;else AlarmClockMusicSet[3]=1;break;
					default:break;
				}
			if(goDown)
				switch(JqSoundOption)
				{
					case 0:JqHourlyTimeSwitch=~JqHourlyTimeSwitch;break;
					case 1:if(AlarmClockMusicSet[0]>1)AlarmClockMusicSet[0]--;else AlarmClockMusicSet[0]=AlarmClockMusicMax;break;
					case 2:if(AlarmClockMusicSet[1]>1)AlarmClockMusicSet[1]--;else AlarmClockMusicSet[1]=AlarmClockMusicMax;break;
					case 3:if(AlarmClockMusicSet[2]>1)AlarmClockMusicSet[2]--;else AlarmClockMusicSet[2]=AlarmClockMusicMax;break;
					case 4:if(AlarmClockMusicSet[3]>1)AlarmClockMusicSet[3]--;else AlarmClockMusicSet[3]=AlarmClockMusicMax;break;
					default:break;
				}
			break;
		}
		case 15:
		{
			if(goBack&&(!goIn)&&(!goDown)){menuCode=1;}
			break;
		}
		case 16:
		{
			switch(moreMenuCode)
			{
				case 1:
				{
					if(goIn){moreMenuCode=10;}
					if(goBack){menuCode=1;}
					break;
				}
				case 10:
				{
					if(goIn){moreMenuCode=100+gameSnakeOption;}
					if(goBack){moreMenuCode=1;}
					if(goUp){if(gameSnakeOption<3)gameSnakeOption++;else gameSnakeOption=0;}
					if(goDown){if(gameSnakeOption>0)gameSnakeOption--;else gameSnakeOption=3;}
					break;
				}
				case 100:
				{
					if(goBack/*&&(!goIn)&&(!goDown)*/){moreMenuCode=10;}
					GreedySnakeMovementDirection_change=GreedySnakeMovementDirection;	
//				if(goUp)if(GreedySnakeMovementDirection>0)GreedySnakeMovementDirection--;else GreedySnakeMovementDirection=3;
//				if(goDown)if(GreedySnakeMovementDirection<3)GreedySnakeMovementDirection++;else GreedySnakeMovementDirection=0;
					if(goUp)if(GreedySnakeMovementDirection_change>0)GreedySnakeMovementDirection_change--;else GreedySnakeMovementDirection_change=3;
					if(goDown)if(GreedySnakeMovementDirection_change<3)GreedySnakeMovementDirection_change++;else GreedySnakeMovementDirection_change=0;
					//防止回头。
					if(greedySnakeHead[0]==greedySnakeBody[0][0])//纵坐标相等
					//if(greedySnakeHead[1]>greedySnakeBody[1])
					if((GreedySnakeMovementDirection_change==1)||(GreedySnakeMovementDirection_change==3))GreedySnakeMovementDirection=GreedySnakeMovementDirection_change;
					if(greedySnakeHead[1]==greedySnakeBody[0][1])//横坐标相等
					if((GreedySnakeMovementDirection_change==0)||(GreedySnakeMovementDirection_change==2))GreedySnakeMovementDirection=GreedySnakeMovementDirection_change;
				
					if(KEY0)gameSpeed=gameSpeedSet;else gameSpeed=0;
					break;
				}
				case 101:
				{
					if(goBack/*&&(!goIn)&&(!goDown)*/){moreMenuCode=10;}
					GreedySnakeMovementDirection_change=GreedySnakeMovementDirection;	
//				if(goUp)if(GreedySnakeMovementDirection>0)GreedySnakeMovementDirection--;else GreedySnakeMovementDirection=3;
//				if(goDown)if(GreedySnakeMovementDirection<3)GreedySnakeMovementDirection++;else GreedySnakeMovementDirection=0;
					if(goUp)if(GreedySnakeMovementDirection_change>0)GreedySnakeMovementDirection_change--;else GreedySnakeMovementDirection_change=3;
					if(goDown)if(GreedySnakeMovementDirection_change<3)GreedySnakeMovementDirection_change++;else GreedySnakeMovementDirection_change=0;
					//防止回头。
					if(greedySnakeHead[0]==greedySnakeBody[0][0])//纵坐标相等
					//if(greedySnakeHead[1]>greedySnakeBody[1])
					if((GreedySnakeMovementDirection_change==1)||(GreedySnakeMovementDirection_change==3))GreedySnakeMovementDirection=GreedySnakeMovementDirection_change;
					if(greedySnakeHead[1]==greedySnakeBody[0][1])//横坐标相等
					if((GreedySnakeMovementDirection_change==0)||(GreedySnakeMovementDirection_change==2))GreedySnakeMovementDirection=GreedySnakeMovementDirection_change;
				
					if(KEY0)gameSpeed=gameSpeedSet;else gameSpeed=0;
					break;
				}
				case 102:{if(goBack){moreMenuCode=10;}break;}
				case 103:
				{
					if(goBack){moreMenuCode=10;}
					if(goIn){if(gameSnakeSetOption>0)gameSnakeSetOption=0;else gameSnakeSetOption++;}
					if(goUp||BurstGoup)switch(gameSnakeSetOption)
					{
						case 0:if(greedySnakeFeedUseNum<10)greedySnakeFeedUseNum++;else greedySnakeFeedUseNum=1;break;
						case 1:if(gameSpeedSet>0)gameSpeedSet--;else gameSpeedSet=10;break;
						default :break;
					}
					if(goDown||BurstGodown)switch(gameSnakeSetOption)
					{
						case 0:if(greedySnakeFeedUseNum>1)greedySnakeFeedUseNum--;else greedySnakeFeedUseNum=10;break;
						case 1:if(gameSpeedSet<10)gameSpeedSet++;else gameSpeedSet=0;break;
						default :break;
					}
					break;
				}
				case 210:
				{
					if(goBack){moreMenuCode=10;rot_taskFrequency[4]=5;}
					if(goIn){moreMenuCode=100;}
					break;
				}
				case 211:
				{
					if(goBack){moreMenuCode=10;rot_taskFrequency[4]=5;}
					if(goIn){moreMenuCode=101;}
					break;
				}
			}
			break;
		}
		case 168:
		{
			if(goBack/*&&(!goIn)&&(!goDown)*/){menuCode=1;}
			GreedySnakeMovementDirection_change=GreedySnakeMovementDirection;	
//			if(goUp)if(GreedySnakeMovementDirection>0)GreedySnakeMovementDirection--;else GreedySnakeMovementDirection=3;
//			if(goDown)if(GreedySnakeMovementDirection<3)GreedySnakeMovementDirection++;else GreedySnakeMovementDirection=0;
			if(goUp)if(GreedySnakeMovementDirection_change>0)GreedySnakeMovementDirection_change--;else GreedySnakeMovementDirection_change=3;
			if(goDown)if(GreedySnakeMovementDirection_change<3)GreedySnakeMovementDirection_change++;else GreedySnakeMovementDirection_change=0;
			//防止回头。
			if(greedySnakeHead[0]==greedySnakeBody[0][0])//纵坐标相等
				//if(greedySnakeHead[1]>greedySnakeBody[1])
				if((GreedySnakeMovementDirection_change==1)||(GreedySnakeMovementDirection_change==3))GreedySnakeMovementDirection=GreedySnakeMovementDirection_change;
			if(greedySnakeHead[1]==greedySnakeBody[0][1])//横坐标相等
				if((GreedySnakeMovementDirection_change==0)||(GreedySnakeMovementDirection_change==2))GreedySnakeMovementDirection=GreedySnakeMovementDirection_change;
				
			if(KEY0)gameSpeed=gameSpeedSet;else gameSpeed=0;
			break;
		}
		case 169:
		{
			if(goBack){menuCode=1;rot_taskFrequency[4]=5;}
			if(goIn){menuCode=16;}
			break;
		}
		case 255:
		{
			if(goIn||goDown||goUp||goBack)menuCode=0;
			break;
		}
	}
}
void recognition_key()			//按键扫描函数
{
	uchar i;
	
	for(i=0;i<4;i++)
	{
		Curstatus_key[i]=(KEY0&(i==0)|KEY1&(i==1)|KEY2&(i==2)|KEY3&(i==3));
		if(Prestate_key[i]!=Curstatus_key[i])
		{
			if(Curstatus_key[i]==0)//0按下，1抬起
			{
				Instruction_code_key[i]=0x80;
			}
			else 
			{
				Instruction_code_key[i]=0x81;
				Refill_sign=0xff;
			}
				Double_Press_counter[i]=0;
				Double_Idle_counter[i]=0;
		}
		else //状态保持
		{
			if(Curstatus_key[i]==0) 
			{if(Instruction_code_key[i]!=0x82){Instruction_code_key[i]=0x82;Double_Press_counter[i]=0;}
				else if(Double_Press_counter[i]<65535)Double_Press_counter[i]++;else Double_Press_counter[i]=100;}
			else 
			{if(Instruction_code_key[i]!=0x83){Instruction_code_key[i]=0x83;Double_Idle_counter[i]=0;}
				else if(Double_Idle_counter[i]<65535)Double_Idle_counter[i]++;else Double_Idle_counter[i]=100;
			//Double_Press_counter[i]=0;
			Refill_sign=0xff;
			}
		}
		Prestate_key[i]=Curstatus_key[i];
	}
	control_Section();
	
}

