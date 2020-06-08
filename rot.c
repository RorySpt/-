#include <STC89C5xRC.H>
#include <stdlib.h>
#include "rot.h"
//
#include "lcd.h"
#include "habit.h"
#include "delay.h"
#include "bmp.h"
#include "temps.h"
#include "test.h"
#include "ds1302.h"
#include "button.h"
#include "dht11.h"
#include "calendar.h"
#include "zk.h"
#include "Uart.h"
#include "jq8900.h"
#include "game.h"

//

#define Clr_menuScreen if(menuRefreshSign!=menuCode){lcd_clr_DDram	();lcd_clr_GD ();menuRefreshSign=menuCode;}

#define tasksTotalNum 6

uint rot_timeBase=1 ;//1~10000
//uchar rot_timebutton_ms=0 ;//0~9

uint rot_taskCounter[tasksTotalNum]={0};//任务计时器
uchar rot_taskTrigger[tasksTotalNum]={0};//任务触发器

uint rot_taskFrequency[tasksTotalNum]={100,100,10,100,5,50};//任务间隔

uchar task0_operating_status=0;//任务分步0: 输入,1:等待,2: 读取
uchar task0_operating_status_timing=0;//0~10

uchar rot_character[]={"星期年月日一二\xC8\xFD四五六七八九十"};
extern uchar ver_key[4][8];

uchar c_day=10;

/************MENU***********/
uchar data menuCode=0;

uchar menuRefreshSign=0;

uchar SwitchFlag[]={0,0};
uchar SwitchFlag_sign[]={0,0};

//uchar backDrop[16];

uchar DisplayOffset=0;
//uchar DisplayOffset_old;
uchar OptionCode_old[]={0,10,10};
uchar OptionSite=0;
uchar OptionSite_old=0;
uchar OptionSite_old_Direction=0;
uchar Option_count=0;//用于子菜单检测

uchar Sensor_switch[2]={1,1};
uint Sensor_display_switchingSpeed=520;
uint Sensor_display_switchingTimer=520;

/*闹钟*/
#define TotalNumber_alarms 4
//控制
uchar AlarmClockNum=0;
uchar AlarmClockNum_old=0;
uchar AlarmOptionSite=0;
uchar AlarmOptionSite_old=0;
//数据
uchar AlarmDat[TotalNumber_alarms][9]={
{0,0,0,0,20,1,1,0,0},
{0,0,0,0,20,1,1,0,0},
{0,0,0,0,20,1,1,0,0},
{0,0,0,0,20,1,1,0,0}
};
uint clock_time_delay[TotalNumber_alarms]={0,0,0,0};

uchar AlarmClockMusicSet[TotalNumber_alarms]={1,2,3,4};

//music
uchar Jq_Counter=0;
uchar Jq_Counter_clock=0;
uchar Jq_Shunt=0;
uchar Jq_SecOld=0;
uchar Jq_CheckPermission=1;
uchar  Jq_Counter_clock2=0;
uchar Jq_IntelligentErrorCorrectionCount=0;
uchar Jq_SmartCorrection_Sec=0;
//uchar MusicTaskStatus[4]={0,0,0,0};
//uchar MusicPlayerDat[3]={0,0,20};
//uint 	MusicPlayerSpecifiedDat=0;
//uchar Music_TimingTaskTiming=0;
extern uchar UartError;

uchar JqSoundOption=10;
uchar JqSoundOptionOld=10;
uchar JqHourlyTimeSwitch=0;
uchar DsHourOld;

uchar BootInterfaceDisplayTime=50;//10为1s;

uchar i,j,k;

uchar gameSpeed=10;
uchar gameSpeedSet=10;
uchar gametime=0;
uchar gamedatcache[2];
uchar gameFPS=0;
uchar gameFPS_counter=0;

uchar moreMenuCode=1;
uchar moreMenuRefreshSign=1;
uchar moreFunctionsOption=0;
uchar moreFunctionsOptionOld=0;
uchar gameSnakeOption=0;
uchar gameSnakeOptionOld=0;
uchar gameSnakeSetOption=10;
uchar gameSnakeSetOptionOld=10;
//uchar gameSnakeOptionCode=0;

void Time0_init()
{
	TMOD |=0x01;//设置定时器0为工作方式1（M1 M0为01）
	TL0 = 0x00;		//设置定时初值
	TH0 = 0xB8;		//设置定时初值
  EA=1;//开总中断
  ET0=1;//开定时器0中断
	TR0=1;//启动定时器
	
}

void rot_init()
{
	Time0_init();
	Uart_Init ();
	
	//Ds18b20_Init();
	//Ds1302_Set_time();
	Ds1302_init();
	lcd_init();
	lcd_clr_DDram	();
	lcd_clr_GD ();
	//d_n_ms(200);
	JqSpecify_volume(0x1a);
	menuCode=255;//进入开机界面
}

//void MusicPlayer()
//{
//	if(Music_TimingTaskTiming==0){JqQueryCmd(2);Music_TimingTaskTiming=1;}//进入执行
//	
//	
//	
//	//触发执行
//	if(MusicTaskStatus[3]&&(TiBUSY==0)&&(RiBUSY==0)){JqPlaySpecifiedTrack(MusicPlayerSpecifiedDat);MusicTaskStatus[3]=0;}//播放指定曲目0~65535
//	if(MusicTaskStatus[2]&&(TiBUSY==0)&&(RiBUSY==0)){JqSpecify_volume(MusicPlayerDat[2]);MusicTaskStatus[2]=0;}//指定音量
//	if(MusicTaskStatus[1]&&(TiBUSY==0)&&(RiBUSY==0)){JqSetLoopMode(MusicPlayerDat[1]);MusicTaskStatus[1]=0;}//指令		：00：全盘循环，01：单曲循环，02：单曲停止	03：全盘随机，04：目录循环，05：目录随机	06：目录顺序，07：全盘顺序
//	if(MusicTaskStatus[0]&&(TiBUSY==0)&&(RiBUSY==0)){JqPlayCmd(MusicPlayerDat[0]);MusicTaskStatus[0]=0;}//02:播放，03：暂停，04：停止，05：上一曲，06：下一曲
//	//定时执行//低优先级
//	if((TiBUSY==0)&&(RiBUSY==0)&&(Music_TimingTaskTiming==1)){JqQueryCmd(7);if(Music_TimingTaskTiming<3)Music_TimingTaskTiming++;else Music_TimingTaskTiming=1;}
//	if((TiBUSY==0)&&(RiBUSY==0)&&(Music_TimingTaskTiming==2)){JqQueryCmd(1);if(Music_TimingTaskTiming<3)Music_TimingTaskTiming++;else Music_TimingTaskTiming=1;}
//	if((TiBUSY==0)&&(RiBUSY==0)&&(Music_TimingTaskTiming==3)){JqQueryCmd(0);if(Music_TimingTaskTiming<3)Music_TimingTaskTiming++;else Music_TimingTaskTiming=1;}
//	
//}

void AlarmClock_trigger()
{
	uchar i,j;
	
	for(i=0;i<4;i++)
	{
		
		if(AlarmDat[i][0]==1)
		{
			if(!AlarmDat[i][8])
			{
				if(AlarmDat[i][4]==((ds_dat_time[6]>>4)*10+(ds_dat_time[6]&0x0f)))
					if(AlarmDat[i][5]==((ds_dat_time[4]>>4)*10+(ds_dat_time[4]&0x0f)))
						if(AlarmDat[i][6]==((ds_dat_time[3]>>4)*10+(ds_dat_time[3]&0x0f)))
							if(AlarmDat[i][1]==((ds_dat_time[2]>>4)*10+(ds_dat_time[2]&0x0f)))
								if(AlarmDat[i][2]==((ds_dat_time[1]>>4)*10+(ds_dat_time[1]&0x0f)))
									if(AlarmDat[i][3]==((ds_dat_time[0]>>4)*10+(ds_dat_time[0]&0x0f)))
									{AlarmDat[i][0]=2;clock_time_delay[i]=0;}//JpAlarmMusic(i);
			}
			if(AlarmDat[i][8])	
				for(j=0;j<7;j++)
					if(((AlarmDat[i][7]>>(7-j))&0x01))
						if(AlarmDat[i][1]==((ds_dat_time[2]>>4)*10+(ds_dat_time[2]&0x0f)))
							if(AlarmDat[i][2]==((ds_dat_time[1]>>4)*10+(ds_dat_time[1]&0x0f)))
								if(AlarmDat[i][3]==((ds_dat_time[0]>>4)*10+(ds_dat_time[0]&0x0f)))
								{AlarmDat[i][0]=3;clock_time_delay[i]=0;}//JpAlarmMusic(i);
	
		//if(!(clock_time_delay[0]%120))JpAlarmMusic(0);
		}

	}

	
}

void Calculation()
{
	if((ds_dat_time[3]!=c_day)||(menuRefreshSign!=menuCode)&&menuCode==0)
	{
		Conversion(0,ds_dat_time[6],ds_dat_time[4],ds_dat_time[3]);cale_data_transfer();
		CalculateWeek(20,ds_dat_time[6],ds_dat_time[4],ds_dat_time[3]);
		c_day=ds_dat_time[3];
		
	}
}

void screenOutput()
{
	//uchar i;
	uchar dat[6]={0,0,0,0,0,0};
	switch(menuCode)
	{
		case 0:{
			if(menuRefreshSign!=menuCode){lcd_clr_DDram	();lcd_clr_GD ();menuRefreshSign=menuCode;}
			lcd_wcmd(0x30);
			lcd_wcmd(0x80);
			lcd_wdat('2');lcd_wdat('0');
			lcd_wdat((ds_dat_time[6]>>4)+48);lcd_wdat((ds_dat_time[6]&0x0f)+48);lcd_wdat('-');
			lcd_wdat((ds_dat_time[4]>>4)+48);lcd_wdat((ds_dat_time[4]&0x0f)+48);lcd_wdat('-');
			lcd_wdat((ds_dat_time[3]>>4)+48);lcd_wdat((ds_dat_time[3]&0x0f)+48);
			if(AlarmDat[0][0]||AlarmDat[1][0]||AlarmDat[2][0]||AlarmDat[3][0])lcd_Draw_PM(5,0,2,16,nz);
			else lcd_Draw_PM_dat2(5,0,1,16,0x00,0x00);
			if(sun_week==1)lcd_ch_display(0,6,"周一");
			if(sun_week==2)lcd_ch_display(0,6,"周二");
			if(sun_week==3)lcd_ch_display(0,6,"周\xC8\xFD");//三
			if(sun_week==4)lcd_ch_display(0,6,"周四");
			if(sun_week==5)lcd_ch_display(0,6,"周五");
			if(sun_week==6)lcd_ch_display(0,6,"周六");
			if(sun_week==0||sun_week==7)lcd_ch_display(0,6,"周日");
			if(SwitchFlag[1]==0)
			{
			lcd_Draw_PM(0,16,2,32,num16321[ds_dat_time[2]>>4]);
			lcd_Draw_PM(1,16,2,32,num16321[ds_dat_time[2]&0x0f]);
			lcd_Draw_PM(2,16,2,32,num16321[10]);
			lcd_Draw_PM(3,16,2,32,num16321[ds_dat_time[1]>>4]);
			lcd_Draw_PM(4,16,2,32,num16321[ds_dat_time[1]&0x0f]);
			lcd_Draw_PM(5,16,2,32,num16321[10]);
			lcd_Draw_PM(6,16,2,32,num16321[ds_dat_time[0]>>4]);
			lcd_Draw_PM(7,16,2,32,num16321[ds_dat_time[0]&0x0f]);
			}
			if(SwitchFlag[1]==1)
			{
			lcd_Draw_PM(0,16,2,32,num16322[ds_dat_time[2]>>4]);
			lcd_Draw_PM(1,16,2,32,num16322[ds_dat_time[2]&0x0f]);
			lcd_Draw_PM(2,16,2,32,num16322[10]);
			lcd_Draw_PM(3,16,2,32,num16322[ds_dat_time[1]>>4]);
			lcd_Draw_PM(4,16,2,32,num16322[ds_dat_time[1]&0x0f]);
			lcd_Draw_PM(5,16,2,32,num16322[10]);
			lcd_Draw_PM(6,16,2,32,num16322[ds_dat_time[0]>>4]);
			lcd_Draw_PM(7,16,2,32,num16322[ds_dat_time[0]&0x0f]);
			}
			if(SwitchFlag[1]==2)
			{
			lcd_Draw_PM(0,16,2,32,num16323[ds_dat_time[2]>>4]);
			lcd_Draw_PM(1,16,2,32,num16323[ds_dat_time[2]&0x0f]);
			lcd_Draw_PM(2,16,2,32,num16323[10]);
			lcd_Draw_PM(3,16,2,32,num16323[ds_dat_time[1]>>4]);
			lcd_Draw_PM(4,16,2,32,num16323[ds_dat_time[1]&0x0f]);
			lcd_Draw_PM(5,16,2,32,num16323[10]);
			lcd_Draw_PM(6,16,2,32,num16323[ds_dat_time[0]>>4]);
			lcd_Draw_PM(7,16,2,32,num16323[ds_dat_time[0]&0x0f]);
			}
			if(SwitchFlag[1]==3)
			{
			lcd_Draw_PM(0,16,2,32,num16324[ds_dat_time[2]>>4]);
			lcd_Draw_PM(1,16,2,32,num16324[ds_dat_time[2]&0x0f]);
			lcd_Draw_PM(2,16,2,32,num16324[10]);
			lcd_Draw_PM(3,16,2,32,num16324[ds_dat_time[1]>>4]);
			lcd_Draw_PM(4,16,2,32,num16324[ds_dat_time[1]&0x0f]);
			lcd_Draw_PM(5,16,2,32,num16324[10]);
			lcd_Draw_PM(6,16,2,32,num16324[ds_dat_time[0]>>4]);
			lcd_Draw_PM(7,16,2,32,num16324[ds_dat_time[0]&0x0f]);
			}
			
			if(moon_month==1)lcd_ch_display(3,0,"\xD5\xFD");
			if(moon_month==2)lcd_ch_display(3,0,"二");
			if(moon_month==3)lcd_ch_display(3,0,"\xC8\xFD");
			if(moon_month==4)lcd_ch_display(3,0,"四");
			if(moon_month==5)lcd_ch_display(3,0,"五");
			if(moon_month==6)lcd_ch_display(3,0,"六");
			if(moon_month==7)lcd_ch_display(3,0,"七");
			if(moon_month==8)lcd_ch_display(3,0,"八");
			if(moon_month==9)lcd_ch_display(3,0,"九");
			if(moon_month==0x10)lcd_ch_display(3,0,"十");
			if(moon_month==0x11)lcd_ch_display(3,0,"冬");
			if(moon_month==0x12)lcd_ch_display(3,0,"腊");
			lcd_ch_display(3,1,"月");
			if(moon_day==0x1){lcd_ch_display(3,2,"初一");}
			if(moon_day==0x2){lcd_ch_display(3,2,"初二");}
			if(moon_day==0x3){lcd_ch_display(3,2,"初\xC8\xFD");}
			if(moon_day==0x4){lcd_ch_display(3,2,"初四");}
			if(moon_day==0x5){lcd_ch_display(3,2,"初五");}
			if(moon_day==0x6){lcd_ch_display(3,2,"初六");}
			if(moon_day==0x7){lcd_ch_display(3,2,"初七");}
			if(moon_day==0x8){lcd_ch_display(3,2,"初八");}
			if(moon_day==0x9){lcd_ch_display(3,2,"初九");}
			if(moon_day==0x10){lcd_ch_display(3,2,"初十");}
			if(moon_day==0x11){lcd_ch_display(3,2,"十一");}
			if(moon_day==0x12){lcd_ch_display(3,2,"十二");}
			if(moon_day==0x13){lcd_ch_display(3,2,"十\xC8\xFD");}
			if(moon_day==0x14){lcd_ch_display(3,2,"十四");}
			if(moon_day==0x15){lcd_ch_display(3,2,"十五");}
			if(moon_day==0x16){lcd_ch_display(3,2,"十六");}
			if(moon_day==0x17){lcd_ch_display(3,2,"十七");}
			if(moon_day==0x18){lcd_ch_display(3,2,"十八");}
			if(moon_day==0x19){lcd_ch_display(3,2,"十九");}
			if(moon_day==0x20){lcd_ch_display(3,2,"二十");}
			if(moon_day==0x21){lcd_ch_display(3,2,"廿一");}
			if(moon_day==0x22){lcd_ch_display(3,2,"廿二");}
			if(moon_day==0x23){lcd_ch_display(3,2,"廿\xC8\xFD");}
			if(moon_day==0x24){lcd_ch_display(3,2,"廿四");}
			if(moon_day==0x25){lcd_ch_display(3,2,"廿五");}
			if(moon_day==0x26){lcd_ch_display(3,2,"廿六");}
			if(moon_day==0x27){lcd_ch_display(3,2,"廿七");}
			if(moon_day==0x28){lcd_ch_display(3,2,"廿八");}
			if(moon_day==0x29){lcd_ch_display(3,2,"廿九");}
			if(moon_day==0x30){lcd_ch_display(3,2,"\xC8\xFD十");}
			if((!Sensor_switch[1])&&(!Sensor_switch[0])||SwitchFlag[0]==2)
			{
				if(SwitchFlag[0]!=SwitchFlag_sign[0]){lcd_Draw_PM_dat(4,48,8,16,0x00);lcd_ch_display(3,4,"        ");SwitchFlag_sign[0]=SwitchFlag[0];}			
				
				if((moon_year%10)==0)lcd_ch_display(3,4,"\xB8\xFD");//天干
				if((moon_year%10)==1)lcd_ch_display(3,4,"辛");
				if((moon_year%10)==2)lcd_ch_display(3,4,"壬");
				if((moon_year%10)==3)lcd_ch_display(3,4,"癸");
				if((moon_year%10)==4)lcd_ch_display(3,4,"甲");
				if((moon_year%10)==5)lcd_ch_display(3,4,"乙");
				if((moon_year%10)==6)lcd_ch_display(3,4,"丙");
				if((moon_year%10)==7)lcd_ch_display(3,4,"丁");
				if((moon_year%10)==8)lcd_ch_display(3,4,"戊");
				if((moon_year%10)==9)lcd_ch_display(3,4,"己");
				
				if((moon_year%12)==0)lcd_ch_display(3,5,"辰");//地支
				if((moon_year%12)==1)lcd_ch_display(3,5,"巳");
				if((moon_year%12)==2)lcd_ch_display(3,5,"午");
				if((moon_year%12)==3)lcd_ch_display(3,5,"未");
				if((moon_year%12)==4)lcd_ch_display(3,5,"申");
				if((moon_year%12)==5)lcd_ch_display(3,5,"酉");
				if((moon_year%12)==6)lcd_ch_display(3,5,"戌");
				if((moon_year%12)==7)lcd_ch_display(3,5,"亥");
				if((moon_year%12)==8)lcd_ch_display(3,5,"子");
				if((moon_year%12)==9)lcd_ch_display(3,5,"丑");
				if((moon_year%12)==10)lcd_ch_display(3,5,"寅");
				if((moon_year%12)==11)lcd_ch_display(3,5,"卯");
				
				if((moon_year%12)==0)lcd_ch_display(3,6,"龙");//地支
				if((moon_year%12)==1)lcd_ch_display(3,6,"蛇");
				if((moon_year%12)==2)lcd_ch_display(3,6,"马");
				if((moon_year%12)==3)lcd_ch_display(3,6,"羊");
				if((moon_year%12)==4)lcd_ch_display(3,6,"猴");
				if((moon_year%12)==5)lcd_ch_display(3,6,"鸡");
				if((moon_year%12)==6)lcd_ch_display(3,6,"狗");
				if((moon_year%12)==7)lcd_ch_display(3,6,"猪");
				if((moon_year%12)==8)lcd_ch_display(3,6,"鼠");
				if((moon_year%12)==9)lcd_ch_display(3,6,"牛");
				if((moon_year%12)==10)lcd_ch_display(3,6,"虎");
				if((moon_year%12)==11)lcd_ch_display(3,6,"兔");
				lcd_ch_display(3,7,"年");
			}

			if((SwitchFlag[0]==0||(!Sensor_switch[1])&&SwitchFlag[0]==1)&&Sensor_switch[0])
			{
				if(SwitchFlag[0]!=SwitchFlag_sign[0]){lcd_Draw_PM_dat(4,48,8,16,0x00);lcd_ch_display(3,4,"        ");SwitchFlag_sign[0]=SwitchFlag[0];}
				lcd_Draw_PM(4,48,2,16,wdj);
				lcd_wcmd(0x30);
				lcd_wcmd(0x9d);
				if(temp_data_Integer/100%10)lcd_wdat(temp_data_Integer/100%10+48);
				lcd_wdat(temp_data_Integer/10%10+48);
				lcd_wdat(temp_data_Integer%10+48);
				if(!(temp_data_Integer/100%10)){lcd_wdat('.');lcd_wdat(temp_data_Decimal_r%10+48);}
				lcd_ch_display(3,7,"℃");
				//lcd_Draw_PM(7,48,2,16,wd);
			}
			if((SwitchFlag[0]==1||(!Sensor_switch[0])&&SwitchFlag[0]==0)&&Sensor_switch[1])
			{
				if(SwitchFlag[0]!=SwitchFlag_sign[0]){lcd_Draw_PM_dat(4,48,8,16,0x00);lcd_ch_display(3,4,"        ");SwitchFlag_sign[0]=SwitchFlag[0];}

				lcd_Draw_PM(4,48,2,16,shd);
				if(Dht_RhData[0]!=100)lcd_ch_display(3,5,"\xA1\xEF");//填充空位
				lcd_wcmd(0x30);
				if(Dht_RhData[0]!=100)lcd_wcmd(0x9e);else {lcd_wcmd(0x9d);lcd_wdat(' ');lcd_wdat('1');}//100%
				lcd_wdat(Dht_RhData[0]/10%10+48);
				lcd_wdat(Dht_RhData[0]%10+48);
//				lcd_wdat('.');//小数没有数据
//				lcd_wdat(Dht_RhData[1]+48);
				lcd_Draw_PM(7,48,2,16,bfb);
			}
			break;
		}
		case 1:
		{	
			if(menuRefreshSign!=menuCode){
				lcd_clr_DDram	();lcd_clr_GD ();menuRefreshSign=menuCode;
				DisplayOffset=0;OptionCode_old[0]=0;OptionSite=0;OptionSite_old=0;
			}
			lcd_ch_display(0,0,"功");
			lcd_ch_display(1,0,"能");
			lcd_ch_display(2,0,"菜");
			lcd_ch_display(3,0,"单");
			lcd_wcmd(0x87);lcd_wdat(OptionCode[0]+48);
			lcd_Draw_PM_dat2(0,0,1,64,0xff,0xfe);
			//光标
			if(OptionCode[0]>OptionCode_old[0])
			{
				if(OptionCode[0]<3)OptionSite=OptionCode[0]*16;
				else if(OptionCode[0]<(OptionCode_Max[0]))OptionSite=32;
				else OptionSite=48;
				OptionSite_old_Direction=0;
			}
			if(OptionCode[0]<OptionCode_old[0])
			{
				if(OptionCode[0]>(OptionCode_Max[0]-3))OptionSite=48-(OptionCode_Max[0]-OptionCode[0])*16;
				else if(OptionCode[0]>0) OptionSite=16;
				else OptionSite=0;
				OptionSite_old_Direction=1;
			}
			OptionCode_old[0]=OptionCode[0];
			if(OptionSite!=OptionSite_old)
			{
			lcd_Draw_PM_dat2(1,0,5,64,0x00,0x00);
			OptionSite_old=OptionSite;
			}
			
			switch(OptionCode_old[0])
			{
				case 0:lcd_Draw_PM_dat2(1,OptionSite,4,16,0xff,0xff);lcd_Draw_PM_dat2(5,OptionSite,1,16,0x00,0x00);break;
				case 2:lcd_Draw_PM_dat2(1,OptionSite,4,16,0xff,0xff);lcd_Draw_PM_dat2(5,OptionSite,1,16,0x00,0x00);break;
				case 4:lcd_Draw_PM_dat2(1,OptionSite,4,16,0xff,0xff);lcd_Draw_PM_dat2(5,OptionSite,1,16,0x00,0x00);break;
//				case 5:lcd_Draw_PM_dat2(1,OptionSite,4,16,0xff,0xff);lcd_Draw_PM_dat2(5,OptionSite,1,16,0xff,0x00);break;
//				case 6:lcd_Draw_PM_dat2(1,OptionSite,4,16,0xff,0xff);lcd_Draw_PM_dat2(5,OptionSite,1,16,0xff,0x00);break;
				case 5:lcd_Draw_PM_dat2(1,OptionSite,4,16,0xff,0xff);lcd_Draw_PM_dat2(5,OptionSite,1,16,0x00,0x00);break;
				case 6:lcd_Draw_PM_dat2(1,OptionSite,4,16,0xff,0xff);lcd_Draw_PM_dat2(5,OptionSite,1,16,0x00,0x00);break;
				default:lcd_Draw_PM_dat2(1,OptionSite,5,16,0xff,0xff);break;
			}
//			lcd_Draw_PM_dat2(1,OptionSite,5,16,0xff,0xff);
			
			//进度条
			lcd_scrollBar(7,3,62,7,OptionCode[0]);
			
			//选单
			DisplayOffset=0;
			if((OptionCode_old[0]<2)&&OptionSite_old_Direction									||(OptionCode_old[0]<3)&&(!OptionSite_old_Direction)){lcd_ch_display(DisplayOffset,5,"  ");lcd_ch_display(DisplayOffset++,1,"设置时间  ");}
			if((OptionCode_old[0]<3)&&OptionSite_old_Direction									||(OptionCode_old[0]<4)&&(!OptionSite_old_Direction))lcd_ch_display(DisplayOffset++,1,"传感器设置");
			if((OptionCode_old[0]<4)&&OptionSite_old_Direction									||(OptionCode_old[0]<5)&&(!OptionSite_old_Direction))lcd_ch_display(DisplayOffset++,1,"闹钟设置  ");
			if(																	OptionSite_old_Direction||								(!OptionSite_old_Direction))lcd_ch_display(DisplayOffset++,1,"音频播放器");
			if(									(OptionCode_old[0]>1)&&OptionSite_old_Direction||(OptionCode_old[0]<7)&&(OptionCode_old[0]>2)&&(!OptionSite_old_Direction))lcd_ch_display(DisplayOffset++,1,"声音设置  ");
			if(									(OptionCode_old[0]>2)&&OptionSite_old_Direction||(OptionCode_old[0]<8)&&(OptionCode_old[0]>3)&&(!OptionSite_old_Direction))lcd_ch_display(DisplayOffset++,1,"关于设备  ");
			if(									(OptionCode_old[0]>3)&&OptionSite_old_Direction||(OptionCode_old[0]<9)&&(OptionCode_old[0]>4)&&(!OptionSite_old_Direction))lcd_ch_display(DisplayOffset++,1,"附加功能  ");
//			if(									(OptionCode[0]>4)&&OptionSite_old_Direction||(OptionCode[0]<10)&&(OptionCode[0]>5)&&(!OptionSite_old_Direction))lcd_ch_display(DisplayOffset++,1,"功能选项7");
//			if(									(OptionCode[0]>5)&&OptionSite_old_Direction||(OptionCode[0]<11)&&(OptionCode[0]>6)&&(!OptionSite_old_Direction))lcd_ch_display(DisplayOffset++,1,"功能选项8");
//			if(									(OptionCode[0]>6)&&OptionSite_old_Direction||(OptionCode[0]<12)&&(OptionCode[0]>7)&&(!OptionSite_old_Direction))lcd_ch_display(DisplayOffset++,1,"功能选项9");
			break;
		}
		
		case 10:
		{	if(menuRefreshSign!=menuCode){lcd_clr_DDram	();lcd_clr_GD ();menuRefreshSign=menuCode;Ds1302_time_Copy(0);OptionCode[1]=10;Option_count=0;}
			//lcd_ch_display(0,0,"功能子菜单");lcd_Draw_PM_dat(0,0,10,16,0xff);
			if(OptionCode[1]==10)Ds1302_time_Copy(0);
			lcd_ch_display(0,0,"设");
			lcd_ch_display(1,0,"置");
			lcd_ch_display(2,0,"时");
			lcd_ch_display(3,0,"间");
			
			lcd_wcmd(0x30);lcd_wcmd(0x81);
			lcd_wdat((ds_dat_time_set[2]>>4)+48);lcd_wdat((ds_dat_time_set[2]&0x0f)+48);lcd_wdat(':');
			lcd_wdat((ds_dat_time_set[1]>>4)+48);lcd_wdat((ds_dat_time_set[1]&0x0f)+48);lcd_wdat(':');
			lcd_wdat((ds_dat_time_set[0]>>4)+48);lcd_wdat((ds_dat_time_set[0]&0x0f)+48);
			lcd_Draw_PM_dat2(0,0,1,64,0xff,0xfe);
			lcd_wcmd(0x30);lcd_wcmd(0x91);
			lcd_wdat('2');lcd_wdat('0');
			lcd_wdat((ds_dat_time_set[6]>>4)+48);lcd_wdat((ds_dat_time_set[6]&0x0f)+48);lcd_wdat('-');
			lcd_wdat((ds_dat_time_set[4]>>4)+48);lcd_wdat((ds_dat_time_set[4]&0x0f)+48);lcd_wdat('-');
			lcd_wdat((ds_dat_time_set[3]>>4)+48);lcd_wdat((ds_dat_time_set[3]&0x0f)+48);
			

			
			
			ds_dat_time_set[5]=CalculateWeek(20,ds_dat_time_set[6],ds_dat_time_set[4],ds_dat_time_set[3]);
			if(ds_dat_time_set[5]==1)lcd_ch_display(2,6,"周一");
			if(ds_dat_time_set[5]==2)lcd_ch_display(2,6,"周二");
			if(ds_dat_time_set[5]==3)lcd_ch_display(2,6,"周\xC8\xFD");//三
			if(ds_dat_time_set[5]==4)lcd_ch_display(2,6,"周四");
			if(ds_dat_time_set[5]==5)lcd_ch_display(2,6,"周五");
			if(ds_dat_time_set[5]==6)lcd_ch_display(2,6,"周六");
			if(ds_dat_time_set[5]==0||ds_dat_time_set[5]==7)lcd_ch_display(2,6,"周日");
			
			Conversion(0,ds_dat_time_set[6],ds_dat_time_set[4],ds_dat_time_set[3]);
			//lcd_wcmd(0x30);lcd_wcmd(0x8b);lcd_wdat((cale_moon_year>>4)+48);lcd_wdat((cale_moon_year&0x0f)+48);
			cale_moon_year=(cale_moon_year>>4)*10+(cale_moon_year&0x0f);
			
			if((cale_moon_year%10)==0)lcd_ch_display(3,1,"\xB8\xFD");//天干
			if((cale_moon_year%10)==1)lcd_ch_display(3,1,"辛");
			if((cale_moon_year%10)==2)lcd_ch_display(3,1,"壬");
			if((cale_moon_year%10)==3)lcd_ch_display(3,1,"癸");
			if((cale_moon_year%10)==4)lcd_ch_display(3,1,"甲");
			if((cale_moon_year%10)==5)lcd_ch_display(3,1,"乙");
			if((cale_moon_year%10)==6)lcd_ch_display(3,1,"丙");
			if((cale_moon_year%10)==7)lcd_ch_display(3,1,"丁");
			if((cale_moon_year%10)==8)lcd_ch_display(3,1,"戊");
			if((cale_moon_year%10)==9)lcd_ch_display(3,1,"己");
			
			if((cale_moon_year%12)==0)lcd_ch_display(3,2,"辰");//地支
			if((cale_moon_year%12)==1)lcd_ch_display(3,2,"巳");
			if((cale_moon_year%12)==2)lcd_ch_display(3,2,"午");
			if((cale_moon_year%12)==3)lcd_ch_display(3,2,"未");
			if((cale_moon_year%12)==4)lcd_ch_display(3,2,"申");
			if((cale_moon_year%12)==5)lcd_ch_display(3,2,"酉");
			if((cale_moon_year%12)==6)lcd_ch_display(3,2,"戌");
			if((cale_moon_year%12)==7)lcd_ch_display(3,2,"亥");
			if((cale_moon_year%12)==8)lcd_ch_display(3,2,"子");
			if((cale_moon_year%12)==9)lcd_ch_display(3,2,"丑");
			if((cale_moon_year%12)==10)lcd_ch_display(3,2,"寅");
			if((cale_moon_year%12)==11)lcd_ch_display(3,2,"卯");
			
			if((cale_moon_year%12)==0)lcd_ch_display(2,1,"龙");//地支
			if((cale_moon_year%12)==1)lcd_ch_display(2,1,"蛇");
			if((cale_moon_year%12)==2)lcd_ch_display(2,1,"马");
			if((cale_moon_year%12)==3)lcd_ch_display(2,1,"羊");
			if((cale_moon_year%12)==4)lcd_ch_display(2,1,"猴");
			if((cale_moon_year%12)==5)lcd_ch_display(2,1,"鸡");
			if((cale_moon_year%12)==6)lcd_ch_display(2,1,"狗");
			if((cale_moon_year%12)==7)lcd_ch_display(2,1,"猪");
			if((cale_moon_year%12)==8)lcd_ch_display(2,1,"鼠");
			if((cale_moon_year%12)==9)lcd_ch_display(2,1,"牛");
			if((cale_moon_year%12)==10)lcd_ch_display(2,1,"虎");
			if((cale_moon_year%12)==11)lcd_ch_display(2,1,"兔");
			lcd_ch_display(3,3,"年");
			if(cale_moon_month==1)lcd_ch_display(3,4,"\xD5\xFD");
			if(cale_moon_month==2)lcd_ch_display(3,4,"二");
			if(cale_moon_month==3)lcd_ch_display(3,4,"\xC8\xFD");
			if(cale_moon_month==4)lcd_ch_display(3,4,"四");
			if(cale_moon_month==5)lcd_ch_display(3,4,"五");
			if(cale_moon_month==6)lcd_ch_display(3,4,"六");
			if(cale_moon_month==7)lcd_ch_display(3,4,"七");
			if(cale_moon_month==8)lcd_ch_display(3,4,"八");
			if(cale_moon_month==9)lcd_ch_display(3,4,"九");
			if(cale_moon_month==0x10)lcd_ch_display(3,4,"十");
			if(cale_moon_month==0x11)lcd_ch_display(3,4,"冬");
			if(cale_moon_month==0x12)lcd_ch_display(3,4,"腊");
			lcd_ch_display(3,5,"月");
			if(cale_moon_day==0x1){lcd_ch_display(3,6,"初一");}
			if(cale_moon_day==0x2){lcd_ch_display(3,6,"初二");}
			if(cale_moon_day==0x3){lcd_ch_display(3,6,"初\xC8\xFD");}
			if(cale_moon_day==0x4){lcd_ch_display(3,6,"初四");}
			if(cale_moon_day==0x5){lcd_ch_display(3,6,"初五");}
			if(cale_moon_day==0x6){lcd_ch_display(3,6,"初六");}
			if(cale_moon_day==0x7){lcd_ch_display(3,6,"初七");}
			if(cale_moon_day==0x8){lcd_ch_display(3,6,"初八");}
			if(cale_moon_day==0x9){lcd_ch_display(3,6,"初九");}
			if(cale_moon_day==0x10){lcd_ch_display(3,6,"初十");}
			if(cale_moon_day==0x11){lcd_ch_display(3,6,"十一");}
			if(cale_moon_day==0x12){lcd_ch_display(3,6,"十二");}
			if(cale_moon_day==0x13){lcd_ch_display(3,6,"十\xC8\xFD");}
			if(cale_moon_day==0x14){lcd_ch_display(3,6,"十四");}
			if(cale_moon_day==0x15){lcd_ch_display(3,6,"十五");}
			if(cale_moon_day==0x16){lcd_ch_display(3,6,"十六");}
			if(cale_moon_day==0x17){lcd_ch_display(3,6,"十七");}
			if(cale_moon_day==0x18){lcd_ch_display(3,6,"十八");}
			if(cale_moon_day==0x19){lcd_ch_display(3,6,"十九");}
			if(cale_moon_day==0x20){lcd_ch_display(3,6,"二十");}
			if(cale_moon_day==0x21){lcd_ch_display(3,6,"廿一");}
			if(cale_moon_day==0x22){lcd_ch_display(3,6,"廿二");}
			if(cale_moon_day==0x23){lcd_ch_display(3,6,"廿\xC8\xFD");}
			if(cale_moon_day==0x24){lcd_ch_display(3,6,"廿四");}
			if(cale_moon_day==0x25){lcd_ch_display(3,6,"廿五");}
			if(cale_moon_day==0x26){lcd_ch_display(3,6,"廿六");}
			if(cale_moon_day==0x27){lcd_ch_display(3,6,"廿七");}
			if(cale_moon_day==0x28){lcd_ch_display(3,6,"廿八");}
			if(cale_moon_day==0x29){lcd_ch_display(3,6,"廿九");}
			if(cale_moon_day==0x30){lcd_ch_display(3,6,"\xC8\xFD十");}
			
			if((OptionCode_old[1]!=OptionCode[1])||Option_count==0)
			{
				lcd_Draw_PM_dat2(1,0,5,64,0x00,0x00);
				Option_count=1;
				switch(OptionCode[1])
				{
					case 0:lcd_Draw_PM_dat2(1,0,1,16,0xff,0xff);break;
					case 1:lcd_Draw_PM_dat2(2,0,1,16,0x00,0xff);lcd_Draw_PM_dat2(3,0,1,16,0xff,0x00);break;
					case 2:lcd_Draw_PM_dat2(4,0,1,16,0xff,0xff);break;
					case 3:lcd_Draw_PM_dat2(2,16,1,16,0xff,0xff);break;
					case 4:lcd_Draw_PM_dat2(3,16,1,16,0x00,0xff);lcd_Draw_PM_dat2(4,16,1,16,0xff,0x00);break;
					case 5:lcd_Draw_PM_dat2(5,16,1,16,0xff,0xff);break;
				}
				OptionCode_old[1]=OptionCode[1];
			}
			break;
		}
		case 100:
		{
			if(menuRefreshSign!=menuCode){lcd_clr_DDram	();lcd_clr_GD ();menuRefreshSign=menuCode;}
			lcd_ch_display(1,0,"时间发生变动！！");
			lcd_ch_display(2,0,"是否确认更改？");
			break;
		}
		case 11:
		{if(menuRefreshSign!=menuCode){lcd_clr_DDram	();lcd_clr_GD ();menuRefreshSign=menuCode;
			
			lcd_Draw_PM_dat2(3,0,1,64,0x00,0x01);
			lcd_Draw_PM_dat2(5,0,1,48,0x00,0x01);
			lcd_Draw_PM_dat2(7,0,1,64,0x00,0x01);
			lcd_Draw_PM_dat2(4,48,4,1,0xff,0xff);
		
		}
			//lcd_ch_display(0,0,"功能子菜单");lcd_Draw_PM_dat(0,0,10,16,0xff);
			lcd_ch_display(0,0,"传");
			lcd_ch_display(1,0,"感");
			lcd_ch_display(2,0,"器");
			lcd_Draw_PM_dat2(0,0,1,48,0xff,0xfe);
		
	/*		lcd_Draw_PM_dat2(1,0,5,64,0x00,0x00);
			lcd_Draw_PM_dat2(3,0,1,64,0x00,0x01);
			lcd_Draw_PM_dat2(5,0,1,48,0x00,0x01);
			lcd_Draw_PM_dat2(7,0,1,64,0x00,0x01);
			lcd_Draw_PM_dat2(4,48,4,1,0xff,0xff);*/
			lcd_ch_display(0,4,"温度");lcd_ch_display(0,6,"湿度");
			lcd_ch_display(1,2,"开关");
			lcd_ch_display(2,2,"刷新");
			lcd_ch_display(3,2,"切换");
			if(Sensor_switch[0])lcd_ch_display(1,4,"YES");else lcd_ch_display(1,4,"NO");
			if(Sensor_switch[1])lcd_ch_display(1,6,"YES");else lcd_ch_display(1,6,"NO");
			lcd_wcmd(0x30);lcd_wcmd(0x8c);
			lcd_wdat((uchar)(rot_taskFrequency[0]/1000%10+48));lcd_wdat((uchar)(rot_taskFrequency[0]/100%10+48));lcd_ch_display(2,5,"秒");
			lcd_wdat((uchar)(rot_taskFrequency[1]/1000%10+48));lcd_wdat((uchar)(rot_taskFrequency[1]/100%10+48));lcd_ch_display(2,7,"秒");
			if(Sensor_switch[0])lcd_ch_display(1,4,"YES");else lcd_ch_display(1,4,"NO ");
			if(Sensor_switch[1])lcd_ch_display(1,6,"YES");else lcd_ch_display(1,6,"NO ");
			lcd_wcmd(0x9d);lcd_wdat((uchar)(Sensor_display_switchingSpeed/1000%10+48));lcd_wdat((uchar)(Sensor_display_switchingSpeed/100%10+48));lcd_ch_display(3,6,"秒");
			if((OptionCode_old[2]!=OptionCode[2]))
			{
				lcd_Draw_PM_dat2(1,0,7,64,0x00,0x00);
				lcd_Draw_PM_dat2(3,0,1,64,0x00,0x01);
				lcd_Draw_PM_dat2(5,0,1,48,0x00,0x01);
				lcd_Draw_PM_dat2(7,0,1,64,0x00,0x01);
				lcd_Draw_PM_dat2(4,48,4,1,0xff,0xff);
				//Option_count=1;
				switch(OptionCode[2])
				{
					case 0:lcd_Draw_PM_dat2(4,17,1,14,0xff,0xff);lcd_Draw_PM_dat2(5,17,1,14,0xff,0x01);break;
					case 1:lcd_Draw_PM_dat2(6,17,1,14,0xff,0xff);lcd_Draw_PM_dat2(7,17,1,14,0xff,0x01);break;
					case 2:lcd_Draw_PM_dat2(4,33,1,14,0xff,0xff);break;
					case 3:lcd_Draw_PM_dat2(6,33,1,14,0xff,0xff);break;
					case 4:lcd_Draw_PM_dat2(5,49,1,14,0xff,0xff);break;
				}
				OptionCode_old[2]=OptionCode[2];
			}
		break;
		}
		case 12:
		{
			if(menuRefreshSign!=menuCode){lcd_clr_DDram	();lcd_clr_GD ();menuRefreshSign=menuCode;AlarmClockNum=10;AlarmClockNum_old=AlarmClockNum;}
			lcd_ch_display(0,0,"闹");
			lcd_ch_display(1,0,"钟");
			lcd_ch_display(2,0,"设");
			lcd_ch_display(3,0,"置");
			lcd_Draw_PM_dat2(0,0,1,64,0xff,0xfe);
			
			lcd_ch_display(0,1,"clock: 1 2 3 4");
			if(AlarmClockNum<4)
			{
				//lcd_wcmd(0x30);lcd_wcmd(0x91);lcd_wdat(AlarmClockNum-1);//lcd_wdat(' ');
				if(AlarmDat[AlarmClockNum][0]==0)lcd_ch_display(1,1,"  关");else lcd_ch_display(1,1,"  开");
				if(AlarmDat[AlarmClockNum][8]==0)lcd_ch_display(2,1,"Data:");else lcd_ch_display(2,1,"Week:");
				lcd_wcmd(0x30);lcd_wcmd(0x94);
				lcd_wdat(AlarmDat[AlarmClockNum][1]/10+48);
				lcd_wdat(AlarmDat[AlarmClockNum][1]%10+48);
				lcd_wdat(':');
				lcd_wdat(AlarmDat[AlarmClockNum][2]/10+48);
				lcd_wdat(AlarmDat[AlarmClockNum][2]%10+48);
				lcd_wdat(':');
				lcd_wdat(AlarmDat[AlarmClockNum][3]/10+48);
				lcd_wdat(AlarmDat[AlarmClockNum][3]%10+48);
				if(AlarmDat[AlarmClockNum][8]==0)
				{
				lcd_Draw_PM_dat2(1,63,7,1,0x00,0x00);	
				lcd_wcmd(0x30);lcd_wcmd(0x99);
				lcd_wdat(' ');lcd_wdat(' ');lcd_wdat(' ');lcd_wdat(' ');
				lcd_wdat('2');lcd_wdat('0');
				lcd_wdat(AlarmDat[AlarmClockNum][4]/10+48);
				lcd_wdat(AlarmDat[AlarmClockNum][4]%10+48);
				lcd_wdat('-');
				lcd_wdat(AlarmDat[AlarmClockNum][5]/10+48);
				lcd_wdat(AlarmDat[AlarmClockNum][5]%10+48);
				lcd_wdat('-');
				lcd_wdat(AlarmDat[AlarmClockNum][6]/10+48);
				lcd_wdat(AlarmDat[AlarmClockNum][6]%10+48);
				}
				else 
				{
					lcd_ch_display(3,1,"一二\xC8\xFD\xCB\xC4\xCE\xE5\xC1\xF9\xC8\xD5");
					if((AlarmDat[AlarmClockNum][7]>>7)&0x01)lcd_Draw_PM_dat2(1,63,1,1,0x15,0x50);else lcd_Draw_PM_dat2(1,63,1,1,0x00,0x00);
					if((AlarmDat[AlarmClockNum][7]>>6)&0x01)lcd_Draw_PM_dat2(2,63,1,1,0x15,0x50);else lcd_Draw_PM_dat2(2,63,1,1,0x00,0x00);
					if((AlarmDat[AlarmClockNum][7]>>5)&0x01)lcd_Draw_PM_dat2(3,63,1,1,0x15,0x50);else lcd_Draw_PM_dat2(3,63,1,1,0x00,0x00);
					if((AlarmDat[AlarmClockNum][7]>>4)&0x01)lcd_Draw_PM_dat2(4,63,1,1,0x15,0x50);else lcd_Draw_PM_dat2(4,63,1,1,0x00,0x00);
					if((AlarmDat[AlarmClockNum][7]>>3)&0x01)lcd_Draw_PM_dat2(5,63,1,1,0x15,0x50);else lcd_Draw_PM_dat2(5,63,1,1,0x00,0x00);
					if((AlarmDat[AlarmClockNum][7]>>2)&0x01)lcd_Draw_PM_dat2(6,63,1,1,0x15,0x50);else lcd_Draw_PM_dat2(6,63,1,1,0x00,0x00);
					if((AlarmDat[AlarmClockNum][7]>>1)&0x01)lcd_Draw_PM_dat2(7,63,1,1,0x15,0x50);else lcd_Draw_PM_dat2(7,63,1,1,0x00,0x00);
				}
			}
			
			if(AlarmClockNum!=AlarmClockNum_old)
			{
				AlarmClockNum_old=AlarmClockNum;
				if(AlarmClockNum_old<4)lcd_Draw_PM_dat2(1,0,7,64,0x00,0x00);
				lcd_Draw_PM_dat2(4+AlarmClockNum,0,1,16,0x0f,0xff);
			}
			if(AlarmOptionSite!=AlarmOptionSite_old)
			{
				AlarmOptionSite_old=AlarmOptionSite;
				lcd_Draw_PM_dat2(1,16,7,47,0x00,0x00);
				if(AlarmOptionSite>0&&AlarmClockNum<4)
				{
					switch(AlarmOptionSite)
					{
						case 1:lcd_Draw_PM_dat2(2,16,1,16,0xff,0xfe);break;
						case 2:lcd_Draw_PM_dat2(4,16,1,16,0xff,0xff);break;
						case 3:lcd_Draw_PM_dat2(5,16,1,16,0x00,0xff);lcd_Draw_PM_dat2(6,16,1,16,0xff,0x00);break;
						case 4:lcd_Draw_PM_dat2(7,16,1,16,0xff,0xff);break;
						case 5:lcd_Draw_PM_dat2(1,32,2,16,0xff,0xff);break;
						case 6:if(!AlarmDat[AlarmClockNum][8])lcd_Draw_PM_dat2(4,48,1,15,0xff,0xff);else lcd_Draw_PM_dat2(1,48,1,15,0xff,0xfe);break;
						case 7:if(!AlarmDat[AlarmClockNum][8]){lcd_Draw_PM_dat2(5,48,1,15,0x00,0xff);lcd_Draw_PM_dat2(6,48,1,15,0xff,0x00);}else lcd_Draw_PM_dat2(2,48,1,15,0xff,0xfe);break;
						case 8:if(!AlarmDat[AlarmClockNum][8])lcd_Draw_PM_dat2(7,48,1,15,0xff,0xff);else lcd_Draw_PM_dat2(3,48,1,15,0xff,0xfe);break;
						case 9:lcd_Draw_PM_dat2(4,48,1,15,0xff,0xfe);break;
						case 10:lcd_Draw_PM_dat2(5,48,1,15,0xff,0xfe);break;
						case 11:lcd_Draw_PM_dat2(6,48,1,15,0xff,0xfe);break;
						case 12:lcd_Draw_PM_dat2(7,48,1,15,0xff,0xfe);break;
					}
					
				}
			}
			break;
		}
		case 13:
		{if(menuRefreshSign!=menuCode){lcd_clr_DDram();lcd_clr_GD();menuRefreshSign=menuCode;UartError=0x00;JqQueryCmd(2);Jq_Counter_clock=0;PS=1;}
			lcd_ch_display(0,0,"播");
			lcd_ch_display(1,0,"放");
			lcd_ch_display(2,0,"器");
			lcd_Draw_PM_dat2(0,0,1,49,0xff,0xfe);
//			lcd_ch_display(3,0,"VOL:");
			lcd_ch_display(0,4,"曲目");
		
			lcd_wcmd(0x30);
			lcd_wcmd(0x86);
//uchar RiPlaybackStatus;//播放状态
//uchar RiTotalTrackTlaybackTime[3];//曲目总时间，时分秒
//uchar RiCurrentPlayingTime[3];//曲目已播放时间
//uchar RiTotalTrack[2];//总曲目
//uchar RiCurrentTrack[2];//当前曲目
			lcd_wdat(':');lcd_wdat(RiCurrentTrack/100%10+48);
			lcd_wdat(RiCurrentTrack/10%10+48);lcd_wdat(RiCurrentTrack%10+48);
			lcd_wcmd(0x94);
			lcd_wdat(RiCurrentPlayingTime[0]/10+48);lcd_wdat(RiCurrentPlayingTime[0]%10+48);lcd_wdat(':');
			lcd_wdat(RiCurrentPlayingTime[1]/10+48);lcd_wdat(RiCurrentPlayingTime[1]%10+48);lcd_wdat(':');
			lcd_wdat(Jq_SmartCorrection_Sec/10+48);lcd_wdat(Jq_SmartCorrection_Sec%10+48);
			lcd_wcmd(0x8c);
			lcd_wdat(RiTotalTrackTlaybackTime[0]/10+48);lcd_wdat(RiTotalTrackTlaybackTime[0]%10+48);lcd_wdat(':');
			lcd_wdat(RiTotalTrackTlaybackTime[1]/10+48);lcd_wdat(RiTotalTrackTlaybackTime[1]%10+48);lcd_wdat(':');
			lcd_wdat(RiTotalTrackTlaybackTime[2]/10+48);lcd_wdat(RiTotalTrackTlaybackTime[2]%10+48);
			lcd_wcmd(0x98);
			lcd_wdat((UartError>>7)+48);lcd_wdat(((UartError>>6)&0x01)+48);lcd_wdat(((UartError>>5)&0x01)+48);
			lcd_wdat(((UartError>>4)&0x01)+48);
			lcd_wdat(((UartError>>3)&0x01)+48);lcd_wdat(((UartError>>2)&0x01)+48);lcd_wdat(((UartError>>1)&0x01)+48);
//			//lcd_wdat(24);lcd_wdat(25);
//			lcd_wcmd(0x9a);
//			lcd_wdat(MusicPlayerDat[2]/10+48);
//			lcd_wdat(MusicPlayerDat[2]%10+48);


			if(RiPlaybackStatus==0)lcd_ch_display(3,4,"MODE:OFF");
			if(RiPlaybackStatus==1)lcd_ch_display(3,4,"MODE:ON ");
			if(RiPlaybackStatus==2)lcd_ch_display(3,4,"MODE:PAU");
			
			if(Jq_CheckPermission)
			{
				if(!(Jq_Counter_clock%3))//延迟
				{
					if((TiBUSY==0))
					{
					if(Jq_Counter==0)JqQueryCmd(0);//播放状态
					if(Jq_Counter==1)JqQueryCmd(1);//播放总时间
					if(Jq_Counter==2)JqQueryCmd(7);//当前曲目
					if(Jq_Counter<2)Jq_Counter++;else Jq_Counter=0;
					}
					else Jq_Shunt++;
					
						
				}
				else if((TiBUSY==0)&&(Jq_Shunt>0))
				{
					if(Jq_Counter==0)JqQueryCmd(0);
					if(Jq_Counter==1)JqQueryCmd(1);
					if(Jq_Counter==2)JqQueryCmd(7);
					if(Jq_Counter<2)Jq_Counter++;
					Jq_Shunt--;
				}
			}
//			if(Jq_SecOld!=RiCurrentPlayingTime[0])
//			{
//				Jq_SecOld==RiCurrentPlayingTime[0];
//				Jq_Counter_clock=0;
//			}
			Jq_Counter_clock++;
			break;
		}
		case 14:
		{
if(menuRefreshSign!=menuCode){lcd_clr_DDram();lcd_clr_GD();menuRefreshSign=menuCode;JqSoundOption=10;DsHourOld=ds_dat_time[2];}
			lcd_ch_display(0,0,"声");
			lcd_ch_display(1,0,"音");
			lcd_ch_display(2,0,"设");
			lcd_ch_display(3,0,"置");
			lcd_Draw_PM_dat2(0,0,1,64,0xff,0xfe);
			lcd_ch_display(0,1,"整点报时：");
			if(JqHourlyTimeSwitch)lcd_ch_display(0,6,"开");else lcd_ch_display(0,6,"关");
			lcd_ch_display(1,1,"闹铃曲目：");
			lcd_ch_display(2,1,"铃1:");lcd_ch_display(2,5,"铃2:");
			lcd_ch_display(3,1,"铃3:");lcd_ch_display(3,5,"铃4:");
			lcd_wcmd(0x30);
			lcd_wcmd(0x8b);lcd_wdat(AlarmClockMusicSet[0]/10+48);lcd_wdat(AlarmClockMusicSet[0]%10+48);
			lcd_wcmd(0x8f);lcd_wdat(AlarmClockMusicSet[1]/10+48);lcd_wdat(AlarmClockMusicSet[1]%10+48);
			lcd_wcmd(0x9b);lcd_wdat(AlarmClockMusicSet[2]/10+48);lcd_wdat(AlarmClockMusicSet[2]%10+48);
			lcd_wcmd(0x9f);lcd_wdat(AlarmClockMusicSet[3]/10+48);lcd_wdat(AlarmClockMusicSet[3]%10+48);
			if(JqSoundOptionOld!=JqSoundOption)
			{
				JqSoundOptionOld=JqSoundOption;
				lcd_Draw_PM_dat2(1,0,7,64,0x00,0x00);
			}
			if(JqSoundOptionOld<5)
				switch(JqSoundOptionOld)
				{
					case 0:lcd_Draw_PM_dat2(6,0,1,16,0xff,0xfe);break;
					case 1:lcd_Draw_PM_dat2(3,32,1,16,0xff,0xff);break;
					case 2:lcd_Draw_PM_dat2(7,32,1,16,0xff,0xff);break;
					case 3:lcd_Draw_PM_dat2(3,48,1,16,0xff,0xff);break;
					case 4:lcd_Draw_PM_dat2(7,48,1,16,0xff,0xff);break;
					default:break;
				}
			break;
		}
		case 15:
		{
			Clr_menuScreen
			if(menuRefreshSign!=menuCode){lcd_clr_DDram();lcd_clr_GD();menuRefreshSign=menuCode;}
			//lcd_Draw_PM(0,0,8,64,gImage_sias);
			lcd_Draw_PM_Any(0,0,64,64,gImage_sias);
			lcd_ch_display(0,4,"毕业设计");
			lcd_ch_display(1,4,"  万年历");
			lcd_ch_display(2,4," by:");
			lcd_ch_display(3,4," RorySpt");
			break;
		}
		case 16:
		{
			Clr_menuScreen
			switch(moreMenuCode)
			{
				case 1:
				{
					if(moreMenuRefreshSign!=moreMenuCode){lcd_clr_DDram();lcd_clr_GD();moreMenuRefreshSign=moreMenuCode;}
					//Clr_menuScreen
					lcd_ch_display(0,0,"附");
					lcd_ch_display(1,0,"加");
					lcd_ch_display(2,0,"功");
					lcd_ch_display(3,0,"能");
					lcd_Draw_PM_dat2(0,0,1,64,0xff,0xfe);
					lcd_ch_display(0,1,"贪吃蛇");
					switch(moreFunctionsOption)//光标
					{
						case 0:lcd_Draw_PM_dat2(1,0,3,16,0xff,0xff);break;
						default:break;
					}
					break;
				}
				case 10:
				{
					if(moreMenuRefreshSign!=moreMenuCode){lcd_clr_DDram();lcd_clr_GD();moreMenuRefreshSign=moreMenuCode;}
					lcd_ch_display(0,0,"贪");
					lcd_ch_display(1,0,"吃");
					lcd_ch_display(2,0,"蛇");
					lcd_Draw_PM_dat2(0,0,1,48,0xff,0xfe);
					lcd_ch_display(0,1,"\xD5\xFD常模式");
					lcd_ch_display(1,1,"穿墙模式");
					lcd_ch_display(2,1,"挑战模式");
					lcd_ch_display(3,1,"游戏设置");
					
					if(gameSnakeOption!=gameSnakeOptionOld)
					{
						
						gameSnakeOptionOld=gameSnakeOption;
						lcd_Draw_PM_dat2(1,0,7,64,0x00,0x00);
					}
					switch(gameSnakeOptionOld)
					{
						case 0:lcd_Draw_PM_dat2(1,0,4,16,0xff,0xff);break;
						case 1:lcd_Draw_PM_dat2(1,16,4,16,0xff,0xff);break;
						case 2:lcd_Draw_PM_dat2(1,32,4,16,0xff,0xff);break;
						case 3:lcd_Draw_PM_dat2(1,48,4,16,0xff,0xff);break;
						default:break;
					}

					break;
				}
				case 100://正常模式
				{
					if(moreMenuRefreshSign!=moreMenuCode){lcd_clr_DDram();lcd_clr_GD();moreMenuRefreshSign=moreMenuCode;
						rot_taskFrequency[4]=1;
						for(i=0;i<96;i+=4)lcd_Draw_PM_Any_plus(i,0,4,4,PixelBlock4x4[11]);
						for(i=0;i<96;i+=4)lcd_Draw_PM_Any_plus(i,60,4,4,PixelBlock4x4[11]);
						for(j=4;j<60;j+=4)lcd_Draw_PM_Any_plus(0,j,4,4,PixelBlock4x4[11]);
						for(j=4;j<60;j+=4)lcd_Draw_PM_Any_plus(92,j,4,4,PixelBlock4x4[11]);
						srand(rot_timeBase);
						
						greedySnakeBody[0][0]=7;greedySnakeBody[0][1]=5;
						greedySnakeBody[1][0]=7;greedySnakeBody[1][1]=4;
						greedySnakeHead		[0]=7;greedySnakeHead		[1]=6;
						GreedySnakeMovementDirection=0;
						greedySnakeBodyLength=2;
						//greedySnakeFeedUseNum=10;
						greedySnake_feed_init();
						greedySnake_feed();
						for(i=0;i<greedySnakeFeedUseNum;i++)lcd_Draw_PM_Any_plus((greedySnakeFeedNum[i][1]+1)*4,(greedySnakeFeedNum[i][0]+1)*4,4,4,PixelBlock4x4[6]);
						for(i=0;i<greedySnakeBodyLength;i++){lcd_Draw_PM_Any_plus((greedySnakeBody[i][1]+1)*4,(greedySnakeBody[i][0]+1)*4,4,4,PixelBlock4x4[0]);}
						lcd_Draw_PM_Any_plus((greedySnakeHead[1]+1)*4,(greedySnakeHead[0]+1)*4,4,4,PixelBlock4x4[2+GreedySnakeMovementDirection]);
						
						gamedatcache[0]=greedySnakeBody[greedySnakeBodyLength-1][0];
						gamedatcache[1]=greedySnakeBody[greedySnakeBodyLength-1][1];
					}
					if(gametime>=gameSpeed)
					{
						gametime=0;

						switch(k=greedySnake_move(0))
						{
							case 0:break;
							case 1:/*lcd_Draw_PM_Any_plus((greedySnakeHead[1]+1)*4,(greedySnakeHead[0]+1)*4,4,4,PixelBlock4x4[7+GreedySnakeMovementDirection]);*/
								greedySnake_feed();for(i=0;i<greedySnakeFeedUseNum;i++)lcd_Draw_PM_Any_plus((greedySnakeFeedNum[i][1]+1)*4,(greedySnakeFeedNum[i][0]+1)*4,4,4,PixelBlock4x4[6]);break;
							case 254:
							case 255:moreMenuCode=210;break;
							default : break;
						}

						
						if(!k)lcd_Draw_PM_Any_plus((gamedatcache[1]+1)*4,(gamedatcache[0]+1)*4,4,4,kong);
						gamedatcache[0]=greedySnakeBody[greedySnakeBodyLength-1][0];
						gamedatcache[1]=greedySnakeBody[greedySnakeBodyLength-1][1];
						lcd_Draw_PM_Any_plus((greedySnakeHead[1]+1)*4,(greedySnakeHead[0]+1)*4,4,4,PixelBlock4x4[2+GreedySnakeMovementDirection]);
						lcd_Draw_PM_Any_plus((greedySnakeBody[0][1]+1)*4,(greedySnakeBody[0][0]+1)*4,4,4,PixelBlock4x4[0]);
						//for(i=0;i<greedySnakeBodyLength;i++){lcd_Draw_PM_Any_plus((greedySnakeBody[i][1]+1)*4,(greedySnakeBody[i][0]+1)*4,4,4,PixelBlock4x4[0]);}
						
						
						lcd_wcmd(0x30);
						lcd_wcmd(0x86);
						lcd_wdat((greedySnakeBodyLength-2)/1000%10+48);lcd_wdat((greedySnakeBodyLength-2)/100%10+48);
						lcd_wdat((greedySnakeBodyLength-2)/10%10+48);lcd_wdat((greedySnakeBodyLength-2)%10+48);

					}
					
					gametime++;
					gameFPS_counter++;
					lcd_wcmd(0x30);lcd_wcmd(0x9f);
					lcd_wdat(gameFPS%100/10+48);lcd_wdat(gameFPS%10+48);
					break;
				}
				case 210:
				{
					if(moreMenuRefreshSign!=moreMenuCode){lcd_clr_DDram();lcd_clr_GD();moreMenuRefreshSign=moreMenuCode;}
					lcd_ch_display(0,2,"游戏结束");
					lcd_ch_display(1,2,"重新开始？");
					break;
				}	
				case 101:
				{
					if(moreMenuRefreshSign!=moreMenuCode){lcd_clr_DDram();lcd_clr_GD();moreMenuRefreshSign=moreMenuCode;
						rot_taskFrequency[4]=1;
						for(i=0;i<96;i+=4)lcd_Draw_PM_Any_plus(i,0,4,4,PixelBlock4x4[11]);
						for(i=0;i<96;i+=4)lcd_Draw_PM_Any_plus(i,60,4,4,PixelBlock4x4[11]);
						for(j=4;j<60;j+=4)lcd_Draw_PM_Any_plus(0,j,4,4,PixelBlock4x4[11]);
						for(j=4;j<60;j+=4)lcd_Draw_PM_Any_plus(92,j,4,4,PixelBlock4x4[11]);
						srand(rot_timeBase);
						
						greedySnakeBody[0][0]=7;greedySnakeBody[0][1]=5;
						greedySnakeBody[1][0]=7;greedySnakeBody[1][1]=4;
						greedySnakeHead		[0]=7;greedySnakeHead		[1]=6;
						GreedySnakeMovementDirection=0;
						greedySnakeBodyLength=2;
					//	greedySnakeFeedUseNum=1;
						greedySnake_feed_init();
						greedySnake_feed();
						for(i=0;i<greedySnakeFeedUseNum;i++)lcd_Draw_PM_Any_plus((greedySnakeFeedNum[i][1]+1)*4,(greedySnakeFeedNum[i][0]+1)*4,4,4,PixelBlock4x4[6]);
						for(i=0;i<greedySnakeBodyLength;i++){lcd_Draw_PM_Any_plus((greedySnakeBody[i][1]+1)*4,(greedySnakeBody[i][0]+1)*4,4,4,PixelBlock4x4[0]);}
						lcd_Draw_PM_Any_plus((greedySnakeHead[1]+1)*4,(greedySnakeHead[0]+1)*4,4,4,PixelBlock4x4[2+GreedySnakeMovementDirection]);
						
						gamedatcache[0]=greedySnakeBody[greedySnakeBodyLength-1][0];
						gamedatcache[1]=greedySnakeBody[greedySnakeBodyLength-1][1];
					}
					if(gametime>=gameSpeed)
					{
						gametime=0;

						switch(k=greedySnake_move(1))
						{
							case 0:break;
							case 1:/*lcd_Draw_PM_Any_plus((greedySnakeHead[1]+1)*4,(greedySnakeHead[0]+1)*4,4,4,PixelBlock4x4[7+GreedySnakeMovementDirection]);*/
								greedySnake_feed();for(i=0;i<greedySnakeFeedUseNum;i++)lcd_Draw_PM_Any_plus((greedySnakeFeedNum[i][1]+1)*4,(greedySnakeFeedNum[i][0]+1)*4,4,4,PixelBlock4x4[6]);break;
							case 254:
							case 255:moreMenuCode=211;break;
							default : break;
						}

						
						if(!k)lcd_Draw_PM_Any_plus((gamedatcache[1]+1)*4,(gamedatcache[0]+1)*4,4,4,kong);
						gamedatcache[0]=greedySnakeBody[greedySnakeBodyLength-1][0];
						gamedatcache[1]=greedySnakeBody[greedySnakeBodyLength-1][1];
						lcd_Draw_PM_Any_plus((greedySnakeHead[1]+1)*4,(greedySnakeHead[0]+1)*4,4,4,PixelBlock4x4[2+GreedySnakeMovementDirection]);
						lcd_Draw_PM_Any_plus((greedySnakeBody[0][1]+1)*4,(greedySnakeBody[0][0]+1)*4,4,4,PixelBlock4x4[0]);
						//for(i=0;i<greedySnakeBodyLength;i++){lcd_Draw_PM_Any_plus((greedySnakeBody[i][1]+1)*4,(greedySnakeBody[i][0]+1)*4,4,4,PixelBlock4x4[0]);}
						
						
						lcd_wcmd(0x30);
						lcd_wcmd(0x86);
						lcd_wdat((greedySnakeBodyLength-2)/1000%10+48);lcd_wdat((greedySnakeBodyLength-2)/100%10+48);
						lcd_wdat((greedySnakeBodyLength-2)/10%10+48);lcd_wdat((greedySnakeBodyLength-2)%10+48);

					}
					
					gametime++;
					gameFPS_counter++;
					lcd_wcmd(0x30);lcd_wcmd(0x9f);
					lcd_wdat(gameFPS%100/10+48);lcd_wdat(gameFPS%10+48);
					break;
				}
				case 211:
				{
					if(moreMenuRefreshSign!=moreMenuCode){lcd_clr_DDram();lcd_clr_GD();moreMenuRefreshSign=moreMenuCode;}
					lcd_ch_display(0,2,"游戏结束");
					lcd_ch_display(1,2,"重新开始？");
					break;
				}	
				case 103:
				{
					if(moreMenuRefreshSign!=moreMenuCode){lcd_clr_DDram();lcd_clr_GD();moreMenuRefreshSign=moreMenuCode;
					lcd_ch_display(0,0,"游");
					lcd_ch_display(1,0,"戏");
					lcd_ch_display(2,0,"设");
					lcd_ch_display(3,0,"置");
					lcd_Draw_PM_dat2(0,0,1,64,0xff,0xfe);
					}
					lcd_ch_display(0,1,"食物\xCA\xFD量：");
					lcd_ch_display(1,1,"移动速度：");
					lcd_wcmd(0x30);
					lcd_wcmd(0x86);lcd_wdat(greedySnakeFeedUseNum/10%10+48);lcd_wdat(greedySnakeFeedUseNum%10+48);
					lcd_wcmd(0x96);lcd_wdat((10-gameSpeedSet)/10%10+48);lcd_wdat((10-gameSpeedSet)%10+48);
					if(gameSnakeSetOption!=gameSnakeSetOptionOld)
					{
						gameSnakeSetOptionOld=gameSnakeSetOption;
						lcd_Draw_PM_dat2(1,0,7,64,0x00,0x00);
					}
					switch(gameSnakeSetOption)
					{
						case 0:lcd_Draw_PM_dat2(6,0,1,16,0xff,0xff);break;
						case 1:lcd_Draw_PM_dat2(6,16,1,16,0xff,0xff);break;
						default:break;
					}
				}
			}
			break;
		}
		
//		case 161:	
//		{
//			if(menuRefreshSign!=menuCode){lcd_clr_DDram();lcd_clr_GD();menuRefreshSign=menuCode;
//				rot_taskFrequency[4]=1;
//				for(i=0;i<96;i+=4)lcd_Draw_PM_Any_plus(i,0,4,4,PixelBlock4x4[11]);
//				for(i=0;i<96;i+=4)lcd_Draw_PM_Any_plus(i,60,4,4,PixelBlock4x4[11]);
//				for(j=4;j<60;j+=4)lcd_Draw_PM_Any_plus(0,j,4,4,PixelBlock4x4[11]);
//				for(j=4;j<60;j+=4)lcd_Draw_PM_Any_plus(92,j,4,4,PixelBlock4x4[11]);
//				srand(rot_timeBase);
//				greedySnakeBody[0][0]=7;greedySnakeBody[0][1]=5;
//				greedySnakeBody[1][0]=7;greedySnakeBody[1][1]=4;
//				greedySnakeHead		[0]=7;greedySnakeHead		[1]=6;
//				GreedySnakeMovementDirection=0;
//				greedySnakeBodyLength=2;
//				greedySnake_feed(1);
//				for(i=0;i<greedySnakeBodyLength;i++){lcd_Draw_PM_Any_plus((greedySnakeBody[i][1]+1)*4,(greedySnakeBody[i][0]+1)*4,4,4,PixelBlock4x4[0]);}
//				lcd_Draw_PM_Any_plus((greedySnakeHead[1]+1)*4,(greedySnakeHead[0]+1)*4,4,4,PixelBlock4x4[2+GreedySnakeMovementDirection]);
//				
//				gamedatcache[0]=greedySnakeBody[greedySnakeBodyLength-1][0];
//				gamedatcache[1]=greedySnakeBody[greedySnakeBodyLength-1][1];
//			}
//			if(gametime>=gameSpeed)
//			{
//				gametime=0;
//				//lcd_Draw_PM_dat2(0,0,8,64,0x00,0x00);
////				lcd_Draw_PM_Any_plus(4,4,88,56,kong);
////				lcd_Draw_PM_Any_plus((greedySnakeBody[greedySnakeBodyLength-1][1]+1)*4,(greedySnakeBody[greedySnakeBodyLength-1][0]+1)*4,4,4,PixelBlock4x4[1]);
//				//lcd_Draw_PM_Any_plus((greedySnakeBody[greedySnakeBodyLength-2][1]+1)*4,(greedySnakeBody[greedySnakeBodyLength-2][0]+1)*4,4,4,kong);
////				for(i=0;i<96;i+=4)lcd_Draw_PM_Any_plus(i,0,4,4,PixelBlock4x4[0]);
////				for(i=0;i<96;i+=4)lcd_Draw_PM_Any_plus(i,60,4,4,PixelBlock4x4[0]);
////				for(j=4;j<60;j+=4)lcd_Draw_PM_Any_plus(0,j,4,4,PixelBlock4x4[0]);
////				for(j=4;j<60;j+=4)lcd_Draw_PM_Any_plus(92,j,4,4,PixelBlock4x4[0]);
//				switch(k=greedySnake_move())
//				{
//					case 0:break;
//					case 1:/*lcd_Draw_PM_Any_plus((greedySnakeHead[1]+1)*4,(greedySnakeHead[0]+1)*4,4,4,PixelBlock4x4[7+GreedySnakeMovementDirection]);*/greedySnake_feed(1);break;
//					case 254:
//					case 255:menuCode=169;break;
//					default : break;
//				}
////				lcd_wcmd(0x30);
////				lcd_wcmd(0x96);
////				lcd_wdat(gamedatcache[0]/10+48);lcd_wdat(gamedatcache[0]%10+48);
////				lcd_wdat(gamedatcache[1]/10+48);lcd_wdat(gamedatcache[1]%10+48);
////				lcd_wcmd(0x9e);
////				lcd_wdat(greedySnakeBody[greedySnakeBodyLength-1][0]/10+48);lcd_wdat(greedySnakeBody[greedySnakeBodyLength-1][0]%10+48);
////				lcd_wdat(greedySnakeBody[greedySnakeBodyLength-1][1]/10+48);lcd_wdat(greedySnakeBody[greedySnakeBodyLength-1][1]%10+48);
//				
//				if(!k)lcd_Draw_PM_Any_plus((gamedatcache[1]+1)*4,(gamedatcache[0]+1)*4,4,4,kong);
//				gamedatcache[0]=greedySnakeBody[greedySnakeBodyLength-1][0];
//				gamedatcache[1]=greedySnakeBody[greedySnakeBodyLength-1][1];
//				lcd_Draw_PM_Any_plus((greedySnakeHead[1]+1)*4,(greedySnakeHead[0]+1)*4,4,4,PixelBlock4x4[2+GreedySnakeMovementDirection]);
//				lcd_Draw_PM_Any_plus((greedySnakeBody[0][1]+1)*4,(greedySnakeBody[0][0]+1)*4,4,4,PixelBlock4x4[0]);
//				//for(i=0;i<greedySnakeBodyLength;i++){lcd_Draw_PM_Any_plus((greedySnakeBody[i][1]+1)*4,(greedySnakeBody[i][0]+1)*4,4,4,PixelBlock4x4[0]);}
//				lcd_Draw_PM_Any_plus((greedySnakeFeedNum[0][1]+1)*4,(greedySnakeFeedNum[0][0]+1)*4,4,4,PixelBlock4x4[6]);
//				
//				lcd_wcmd(0x30);
//				lcd_wcmd(0x86);
//				lcd_wdat((greedySnakeBodyLength-2)/1000%10+48);lcd_wdat((greedySnakeBodyLength-2)/100%10+48);
//				lcd_wdat((greedySnakeBodyLength-2)/10%10+48);lcd_wdat((greedySnakeBodyLength-2)%10+48);
////				lcd_wdat(greedySnakeBody[0][0]/10+48);lcd_wdat(greedySnakeBody[0][0]%10+48);
////				lcd_wdat(greedySnakeBody[0][1]/10+48);lcd_wdat(greedySnakeBody[0][1]%10+48);
////				lcd_wcmd(0x96);lcd_wdat(greedySnakeBody[1][0]/10+48);lcd_wdat(greedySnakeBody[1][0]%10+48);
////				lcd_wdat(greedySnakeBody[1][1]/10+48);lcd_wdat(greedySnakeBody[1][1]%10+48);
////				lcd_wcmd(0x8e);lcd_wdat(greedySnakeHead[0]/10+48);lcd_wdat(greedySnakeHead[0]%10+48);
////				lcd_wdat(greedySnakeHead[1]/10+48);lcd_wdat(greedySnakeHead[1]%10+48);
////				lcd_wcmd(0x9e);lcd_wdat(greedySnakeFeedNum[0][0]/10+48);lcd_wdat(greedySnakeFeedNum[0][0]%10+48);
////				lcd_wdat(greedySnakeFeedNum[0][1]/10+48);lcd_wdat(greedySnakeFeedNum[0][1]%10+48);
//			}
//			
//			gametime++;
//			gameFPS_counter++;
//			lcd_wcmd(0x30);lcd_wcmd(0x9f);
//			lcd_wdat(gameFPS%100/10+48);lcd_wdat(gameFPS%10+48);
//			break;
//	  }
	
		case 255:
		{
			if(menuRefreshSign!=menuCode){lcd_clr_DDram();lcd_clr_GD();menuRefreshSign=menuCode;}
			if(BootInterfaceDisplayTime>40)lcd_Draw_PM_Any(32,0,64,64,gImage_sias);
			if(BootInterfaceDisplayTime<40)
			{
				if(BootInterfaceDisplayTime>28)lcd_Draw_PM_Any((BootInterfaceDisplayTime-29)*3,0,64,64,gImage_sias);
				
				if(BootInterfaceDisplayTime<25)
				{
					lcd_ch_display(0,4,"毕业设计");
					lcd_ch_display(1,4,"  万年历");
					lcd_ch_display(2,4," by:");
					lcd_ch_display(3,4," RorySpt");
				}
			}

			BootInterfaceDisplayTime--;
			if(!BootInterfaceDisplayTime)menuCode=0;
			break;
		}
	}		
}

//This is for test!
//{
//	uchar i,j;
//	lcd_init();

//		
//		lcd_wcmd(0x80);
//		lcd_wdat((ds_dat_time[2]>>4)+48);lcd_wdat((ds_dat_time[2]&0x0f)+48);//时
//		lcd_wdat('/');
//		lcd_wdat((ds_dat_time[1]>>4)+48);lcd_wdat((ds_dat_time[1]&0x0f)+48);//分
//		lcd_wdat('/');
//		lcd_wdat(((ds_dat_time[0])>>4)+48);lcd_wdat((ds_dat_time[0]&0x0f)+48);//秒
//	
//		lcd_wcmd(0x84);
//		lcd_wdat((moon_year>>4)+48);lcd_wdat((moon_year&0x0f)+48);//阴历
//		lcd_wdat('/');
//		lcd_wdat((moon_month>>4)+48);lcd_wdat((moon_month&0x0f)+48);
//		lcd_wdat('/');
//		lcd_wdat(((moon_day)>>4)+48);lcd_wdat((moon_day&0x0f)+48);
//		
//		lcd_wcmd(0x94);
//		lcd_wdat((ds_dat_time[6]>>4)+48);lcd_wdat((ds_dat_time[6]&0x0f)+48);//阳历
//		lcd_wdat('/');
//		lcd_wdat((ds_dat_time[4]>>4)+48);lcd_wdat((ds_dat_time[4]&0x0f)+48);
//		lcd_wdat('/');
//		lcd_wdat(((ds_dat_time[3])>>4)+48);lcd_wdat((ds_dat_time[3]&0x0f)+48);
//		
//		//i=CalculateWeek(20,((ds_dat_time[6]>>4)+(ds_dat_time[6]&0x0f)),((ds_dat_time[4]>>4)+(ds_dat_time[4]&0x0f)), ((ds_dat_time[3]>>4)+(ds_dat_time[3]&0x0f)));
//		i=CalculateWeek(century,ds_dat_time[6],ds_dat_time[4],ds_dat_time[3]);
//		lcd_wcmd(0x90);
//		lcd_wdat(i+48);lcd_wdat(' ');
////		lcd_wcmd(0x94);
////		lcd_wdat(Double_Press_counter[0]/10/100+48);
////		lcd_wdat(Double_Press_counter[0]/100%10+48);
////		lcd_wdat(Double_Press_counter[0]/10%10+48);
////		lcd_wdat(Double_Press_counter[0]%10+48);
//	
//	for(i=0;i<4;i++)
//		{
//			lcd_wcmd((0x88+i*4)*(i<2)+(0x98+(i-2)*4)*(i>=2));
//			for(j=0;j<4;j++)
//				lcd_wdat(ver_key[i][j]);
//			lcd_wdat(' ');
//			switch(Instruction_code_key[i])
//			{
//				case 0x80:lcd_wdat('0');break;
//				case 0x81:lcd_wdat('1');break;
//				case 0x82:if(Double_Press_counter[i]>2)lcd_wdat('2');else lcd_wdat('0');break;
//				case 0x83:if(Double_Press_counter[i]>2)lcd_wdat('3');else lcd_wdat('1');break;
//				default  :lcd_wdat('4');break;
//			}
//			lcd_wdat(' ');lcd_wdat(' ');
//		}
//}

uchar rot_task0()		//温度
{
	 if(task0_operating_status==0){EA=0;Ds18b20_ChangTemp();task0_operating_status=1;EA=1;}
	 if(task0_operating_status==2){EA=0;Ds18b20_rTemp();temp_data_collation();task0_operating_status=0;EA=1;}
	//Ds18b20_ChangTemp();Ds18b20_rTemp();temp_data_collation();
	return 0;
}
void 	rot_task1(){if(Sensor_switch[1]){EA=0;dht_rdat();EA=1;}}	//湿度
void 	rot_task2(){Ds1302_Read_time();	}	//时间
void 	rot_task3(){if(Sensor_switch[0])Calculation();}	//数据计算
void	rot_task4(){screenOutput();}	//显示
void	rot_task5(){AlarmClock_trigger();}	//闹钟
//void	rot_task6(){MusicPlayer();}

void rot_taskSchedule()	//任务调度器
{
	if(rot_taskTrigger[0]){rot_taskTrigger[0]=rot_task0();}//rot_taskTrigger[0]=rot_task0();程序可连续执行
	if(rot_taskTrigger[1]){rot_task1();rot_taskTrigger[1]=0;}
	if(rot_taskTrigger[2]){rot_task2();rot_taskTrigger[2]=0;}
	if(rot_taskTrigger[3]){rot_task3();rot_taskTrigger[3]=0;}
	if(rot_taskTrigger[4]){rot_task4();rot_taskTrigger[4]=0;}
	if(rot_taskTrigger[5]){rot_task5();rot_taskTrigger[5]=0;}
	//if(rot_taskTrigger[6]){rot_task6();rot_taskTrigger[6]=0;}
}
void rot_T0_time() interrupt 1
{
	uchar data i;
  //TMOD=0x01;
	TL0 = 0x00;		//设置定时初值
	TH0 = 0xB8;		//设置定时初值
  if((rot_timeBase++)>10000)	rot_timeBase=1;//时基，10ms
	if(!(rot_timeBase%5))		recognition_key();	//每5*10ms触发一次
	for(i=0;i<tasksTotalNum;i++)if(rot_taskTrigger[i]==0)if((rot_taskCounter[i]++)>rot_taskFrequency[i]){rot_taskCounter[i]=0;rot_taskTrigger[i]=1;}//定时任务触发器
	//if((task0_operating_status>0)&&(task0_operating_status<15))task0_operating_status++;else task0_operating_status=0;

	if(task0_operating_status==1){						//任务0分步。任务触发之后100ms再次触发一次。
		if((task0_operating_status_timing++)<10);
		else {task0_operating_status_timing=0;task0_operating_status=2;rot_taskTrigger[0]=1;}
		}
	
	//if(!(rot_timeBase%Sensor_display_switchingSpeed))		if(SwitchFlag[0]<1)SwitchFlag[0]++;else if(Sensor_switch[0])SwitchFlag[0]=0;else SwitchFlag[0]=1;		//定时切换
		if(!Sensor_display_switchingTimer){		if(SwitchFlag[0]<1)SwitchFlag[0]++;else if(Sensor_switch[0])SwitchFlag[0]=0;else SwitchFlag[0]=1;Sensor_display_switchingTimer=Sensor_display_switchingSpeed;	}	//定时切换
		else Sensor_display_switchingTimer--;
	//if(!(rot_timeBase%400))		if(SwitchFlag[1]<1)SwitchFlag[1]++;else SwitchFlag[1]=0;
	if(AlarmDat[0][0]>1){if(!(clock_time_delay[0]%300)){JqPlaySpecifiedTrack(AlarmClockMusicSet[0]);clock_time_delay[0]++;}if(!(rot_timeBase%100))clock_time_delay[0]++;if(clock_time_delay[0]>3600)AlarmDat[0][0]-=2;}	//四个闹钟触发时长控制
	if(AlarmDat[1][0]>1){if(!(clock_time_delay[1]%300)){JqPlaySpecifiedTrack(AlarmClockMusicSet[1]);clock_time_delay[1]++;}if(!(rot_timeBase%100))clock_time_delay[1]++;if(clock_time_delay[1]>3600)AlarmDat[1][0]-=2;}
	if(AlarmDat[2][0]>1){if(!(clock_time_delay[2]%300)){JqPlaySpecifiedTrack(AlarmClockMusicSet[2]);clock_time_delay[2]++;}if(!(rot_timeBase%100))clock_time_delay[2]++;if(clock_time_delay[2]>3600)AlarmDat[2][0]-=2;}
	if(AlarmDat[3][0]>1){if(!(clock_time_delay[3]%300)){JqPlaySpecifiedTrack(AlarmClockMusicSet[3]);clock_time_delay[3]++;}if(!(rot_timeBase%100))clock_time_delay[3]++;if(clock_time_delay[3]>3600)AlarmDat[3][0]-=2;}
	
	if(menuCode==13)//防止接收时序重叠
	{
		if(Jq_SecOld!=RiCurrentPlayingTime[2])
		{
			Jq_SecOld=RiCurrentPlayingTime[2];
			Jq_Counter_clock2=0;
			Jq_IntelligentErrorCorrectionCount=0;
		}
		
//************智能检错纠错**********************************************************
//作用：对秒的自动预测，以弥补uart通讯丢帧造成的播放时间跳动
//使用变量：Jq_SmartCorrection_Sec,Jq_IntelligentErrorCorrectionCount;
//引入可能造成的不良后果：如果播放器长时间没响应，秒会自动一直走到59并卡住，当播放器
//反应过来后又会跳回正确的时间。
//**********************************************************************************
		else if((Jq_SmartCorrection_Sec>0)&&(Jq_SmartCorrection_Sec!=RiTotalTrackTlaybackTime[2])&&(RiPlaybackStatus==1))
		{
			Jq_IntelligentErrorCorrectionCount++;
			if(Jq_IntelligentErrorCorrectionCount>100){if(Jq_SmartCorrection_Sec<59)Jq_SmartCorrection_Sec++;Jq_IntelligentErrorCorrectionCount=0;}
		}
		if(!(KEY0&KEY1&KEY2&KEY3))Jq_IntelligentErrorCorrectionCount=0;
//**********************************************************************************
		
		Jq_Counter_clock2++;
		if((Jq_Counter_clock2<105)&&(Jq_Counter_clock2>85))Jq_CheckPermission=0;
		else Jq_CheckPermission=1;

	}
//	//音乐
//	if(menuCode!=13){rot_taskTrigger[6]=0;}
	
//整点报时
	if(JqHourlyTimeSwitch)
	{
		if(DsHourOld!=ds_dat_time[2]){DsHourOld=ds_dat_time[2];Tell_theTime();}
	}
	if(menuCode==16)if(rot_timeBase%100==0)
	{
		gameFPS=gameFPS_counter;
		gameFPS_counter=0;
	}
}

//void rot_os()
//{
//	
//}