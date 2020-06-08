#include "test.h"
#include "habit.h"
#include "lcd.h"
#include "temps.h"
#include "bmp.h"
#include "ds1302.h"
#include "button.h"
#include "dht11.h"
/****temp******/
uchar c[]={"℃"};

/****DS1302****/
uchar init_ds_dat_time[7]={0x00,0x00,0x00,0x23,0x01,0x04,0x20};
uchar code test_ds1302_ver_0[]={"lcd.test:version 1.0"};
uchar time_clock=0;

/****key*******/
//uchar code test_key_ver_0[]={"key0~3.test:version 0"};
uchar code test_key_ver_0[]={"key.test:  ver 0data:   2020/2/7"};
uchar ver_key[4][8]={"key0","key1","key2","key3"};

/****BZ********/
sbit BZ=P1^5;
uchar BZ_Forbiddenbit[4]=0;

void test_BZ()
{
	uint n=1;
	lcd_init();
	while(1)
	{
		recognition_key();lcd_wcmd(0x80);lcd_wdat((n/1000)+48);lcd_wdat((n/100)%10+48);lcd_wdat((n/10)%10+48);lcd_wdat(n%10+48);
		if(Instruction_code_key[0]==0x80&&BZ_Forbiddenbit[0]==0){if(n<1000)n++;else n=0;BZ_Forbiddenbit[0]=1;}
		else {BZ_Forbiddenbit[0]=0;if(Instruction_code_key[0]==0x82&&Double_Press_counter[0]>10){if(n<1000)n++;else n=0;Double_Press_counter[0]=0;}}
		if(Instruction_code_key[1]==0x80&&BZ_Forbiddenbit[1]==0){if(n>0)n--;else n=1000;BZ_Forbiddenbit[1]=1;}
		else {BZ_Forbiddenbit[1]=0;if(Instruction_code_key[1]==0x82&&Double_Press_counter[1]>10){if(n>0)n--;else n=1000;Double_Press_counter[1]=0;}}
		BZ=1;d_n_ms(n);BZ=0;
	}
}
void test_buttons()
{
	uint i,j;
	lcd_init();
	//while(1)test_display_temps();
	lcd_wcmd(0x80);
	for(i=0;i<16;i++)
	{
		lcd_wdat(test_key_ver_0[i]);
	}
	lcd_wcmd(0x90);
	for(;i<32;i++)
	{
		lcd_wdat(test_key_ver_0[i]);
	}
	
	while(1)
	{
		
		//DS_IO=0;
		//test_display_temps();
		recognition_key();
		for(i=0;i<4;i++)
		{
			lcd_wcmd((0x88+i*4)*(i<2)+(0x98+(i-2)*4)*(i>=2));
			for(j=0;j<4;j++)
				lcd_wdat(ver_key[i][j]);
			lcd_wdat(' ');
			switch(Instruction_code_key[i])
			{
				case 0x80:lcd_wdat('0');break;
				case 0x81:lcd_wdat('1');break;
				case 0x82:if(Double_Press_counter[i]>10)lcd_wdat('2');else lcd_wdat('0');break;
				case 0x83:if(Double_Press_counter[i]>10)lcd_wdat('3');else lcd_wdat('1');break;
				default  :lcd_wdat('4');break;
			}
			lcd_wdat(' ');lcd_wdat(' ');
		}
		d_100ms();
	} 
}

void test_display_temps()
{
	uint i=0;
	lcd_init();
	while(1)
	{
		Ds18b20_ChangTemp();
		d_100ms();
		Ds18b20_rTemp();
		
		temp_data_collation();
		//LCD_cmd_functionSet_Basic;//使用基本指令集
		lcd_wcmd(0x80);
		/*	if(temp_data_sign)lcd_wdat('-');
		lcd_wdat((uchar)(temp_data_Integer/100%10+48));
		lcd_wdat(temp_data_Integer/10%10+48);
		lcd_wdat(temp_data_Integer%10+48);
		lcd_wdat('.');
		lcd_wdat((temp_data_Decimal_r/100)+48);
		lcd_wdat((temp_data_Decimal_r/10%10)+48);
		lcd_wdat(c[0]);lcd_wdat(c[1]);
		lcd_wdat(' ');
		if(temp_data_sign)lcd_wdat('-');
		if(temp_data_Integer/100%10)lcd_wdat((uchar)(temp_data_Integer/100%10+48));*/
		lcd_wdat(temp_data_Integer/10%10+48);
		lcd_wdat(temp_data_Integer%10+48);
		lcd_wdat('.');
		lcd_wdat((temp_data_Decimal_r/10%10)+48);
		lcd_wdat((temp_data_Decimal_r%10)+48);
		if(!temp_data_sign)lcd_wdat(' ');
		if(!(temp_data_Integer/100%10))lcd_wdat(' ');	
		
		lcd_wcmd(0x88);
		lcd_wdat(i/10000+48);
		lcd_wdat(i/1000%10+48);	
		lcd_wdat(i/100%10+48);	
		lcd_wdat(i/10%10+48);	
		lcd_wdat(i%10+48);	
		i++;
		lcd_wcmd(0x8f);
		lcd_wdat(error+48);
		//lcd_wcmd(0x98);
		/*for(j=0;j<8;j++)
		{
			lcd_wdat((uchar)(bit)((temp_High<<j)&0x80)+48);
		}
		for(j=0;j<8;j++)
		{
			lcd_wdat((uchar)(bit)((temp_Low<<j)&0x80)+48);
		}*/
}
	
}
void test_lcd12864_displayPM1()
{
		uint i,j;
	lcd_wcmd(0x30);
for(i=0;i<1;i++)
		{
			lcd_wcmd(0x80);
			for(j=0;j<64;j++)
			{
				switch(j/16)
				{
					case 0:lcd_wdat(i+j);break;	
					case 1:lcd_wdat(i+j+16);break;
					case 2:lcd_wdat(i+j-16);break;
					case 3:lcd_wdat(i+j);break;
				}
			}
		}
}
void test_lcd12864_displayPM0()
{

	lcd_init();

	while(1)
	{
		
		test_lcd12864_displayPM1();
		d_1s();
		lcd_Draw_PM(0,0,16,63,zhu);
		lcd_wcmd(0x36);
		d_1s();
		lcd_clr_DDram();lcd_wcmd(0x36);
		d_1s();
		lcd_clr_GD();
		d_1s();
	}
}

void test_ds1302()
{
	uchar i;
	lcd_init();
	
	lcd_wcmd(0x88);
	for(i=0;i<9;i++)lcd_wdat(test_ds1302_ver_0[i]);
	lcd_wcmd(0x98+2);
	for(;i<21;i++)lcd_wdat(test_ds1302_ver_0[i]);
	
	for(i=0;i<7;i++)ds_dat_time[i]=init_ds_dat_time[i];
	//Ds1302_Set_time();

	

	while(1)
	{
		Ds1302_Read_time();
		
		lcd_wcmd(0x80);
		lcd_wdat((ds_dat_time[2]>>4)+48);lcd_wdat((ds_dat_time[2]&0x0f)+48);//时
		lcd_wdat('/');
		lcd_wdat((ds_dat_time[1]>>4)+48);lcd_wdat((ds_dat_time[1]&0x0f)+48);//分
		lcd_wdat('/');
		lcd_wdat(((ds_dat_time[0])>>4)+48);lcd_wdat((ds_dat_time[0]&0x0f)+48);//秒
		lcd_wcmd(0x90);
		lcd_wdat((ds_dat_time[6]>>4)+48);lcd_wdat((ds_dat_time[6]&0x0f)+48);//年
		lcd_wdat('/');
		lcd_wdat((ds_dat_time[4]>>4)+48);lcd_wdat((ds_dat_time[4]&0x0f)+48);//月
		lcd_wdat('/');
		lcd_wdat((ds_dat_time[3]>>4)+48);lcd_wdat((ds_dat_time[3]&0x0f)+48);//日
		lcd_wcmd(0x96);lcd_wdat(0xD6);lcd_wdat(0xDC);
		lcd_wdat((ds_dat_time[5]>>16)+48);lcd_wdat((ds_dat_time[5]&0x0f)+48);//周
		//d_100ms();

	}
/**/
}

void test_dht()//
{
	uchar k=0;
	uchar i;
	lcd_init();
	lcd_clr_DDram	();
	lcd_clr_GD ();
	Ds18b20_Init();
	for(;;)
	{
		Ds18b20_ChangTemp();
		d_1s();
		Ds18b20_rTemp();
		
		temp_data_collation();
		d_n_ms(10);
		lcd_wcmd(0x30);
		lcd_wcmd(0x94);
		if(temp_data_sign)lcd_wdat('-');
		lcd_wdat((uchar)(temp_data_Integer/100%10+48));
		lcd_wdat(temp_data_Integer/10%10+48);
		lcd_wdat(temp_data_Integer%10+48);
		lcd_wdat('.');
		//lcd_wdat((temp_data_Decimal_r/10)+48);
		lcd_wdat((temp_data_Decimal_r%10)+48);
		
		dht_rdat();
		d_n_ms(10);
		lcd_wcmd(0x80);
		lcd_wdat(Dht_RhData[0]/100+48);
		lcd_wdat(Dht_RhData[0]/10%10+48);
		lcd_wdat(Dht_RhData[0]%10+48);
		lcd_wdat('.');
		if(Dht_RhData[1]/100>0)lcd_wdat(Dht_RhData[1]/100+48);
		lcd_wdat(Dht_RhData[1]/10%10+48);
		lcd_wdat(Dht_RhData[1]%10+48);
		d_n_ms(10);
		lcd_wcmd(0x84);
		lcd_wdat(Dht_RhData[2]/100+48);
		lcd_wdat(Dht_RhData[2]/10%10+48);
		lcd_wdat(Dht_RhData[2]%10+48);
		lcd_wdat('.');
		if(Dht_RhData[3]/100>0)lcd_wdat(Dht_RhData[1]/100+48);
		lcd_wdat(Dht_RhData[3]/10%10+48);
		lcd_wdat(Dht_RhData[3]%10+48);
		/*m=(uint)Dht_RhData[1];
		m=m*39;
		lcd_wdat(m/10000+48);
		lcd_wdat(m/1000%10+48);*/
		d_n_ms(10);
		lcd_wcmd(0x90);
		lcd_wdat(Dht_error/100+48);
		lcd_wdat(Dht_error/10%10+48);
		lcd_wdat(Dht_error%10+48);
		d_n_ms(10);
		lcd_wcmd(0x88);
		for(i=0;i<7;i++)lcd_wdat(((Dht_RhData[4]>>(7-i))&0x01)+48);
		d_n_ms(10);
		lcd_wcmd(0x98);
		if((Dht_RhData[3]+Dht_RhData[2]+Dht_RhData[1]+Dht_RhData[0])==Dht_RhData[4])lcd_wdat('0');
		else lcd_wdat('1');
		d_1s();//d_1s();
	}
}
//rot_screenOutput()

//void screenOutput()
//{
//uchar i,j;
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
//		lcd_wdat(' ');
//		if(temp_data_sign)lcd_wdat('-');
//		if(temp_data_Integer/100%10)lcd_wdat(temp_data_Integer/100%10+48);
//		lcd_wdat(temp_data_Integer/10%10+48);
//		lcd_wdat(temp_data_Integer%10+48);
//		lcd_wdat('.');
//		lcd_wdat((temp_data_Decimal_r/10)+48);
//		lcd_wdat((temp_data_Decimal_r%10)+48);
//		if(!temp_data_sign)lcd_wdat(' ');
//		if(!(temp_data_Integer/100%10))lcd_wdat(' ');	
//		
//		dht_rdat();
//		lcd_wcmd(0x90);
//		if(Dht_RhData[0]/100)lcd_wdat(Dht_RhData[0]/100+48);
//		lcd_wdat(Dht_RhData[0]/10%10+48);
//		lcd_wdat(Dht_RhData[0]%10+48);
//		if(!(Dht_RhData[0]/100))lcd_wdat(' ');	
//		/*lcd_wdat('.');
//		if(Dht_RhData[1]/100)lcd_wdat(Dht_RhData[1]/100+48);
//		lcd_wdat(Dht_RhData[1]/10%10+48);
//		lcd_wdat(Dht_RhData[1]%10+48);*/
//		lcd_wcmd(0x92);
//		if(Dht_RhData[2]/100)lcd_wdat(Dht_RhData[2]/100+48);
//		lcd_wdat(Dht_RhData[2]/10%10+48);
//		lcd_wdat(Dht_RhData[2]%10+48);
//		if(!(Dht_RhData[2]/100))lcd_wdat(' ');
//		/*lcd_wdat('.');
//		if(Dht_RhData[3]/100>0)lcd_wdat(Dht_RhData[1]/100+48);
//		lcd_wdat(Dht_RhData[3]/10%10+48);
//		lcd_wdat(Dht_RhData[3]%10+48);	*/
//		
//		lcd_wcmd(0x94);
//		lcd_wdat(Double_Press_counter[0]/10/100+48);
//		lcd_wdat(Double_Press_counter[0]/100%10+48);
//		lcd_wdat(Double_Press_counter[0]/10%10+48);
//		lcd_wdat(Double_Press_counter[0]%10+48);
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