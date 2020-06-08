#include "dht11.h"
#include "delay.h"


uchar Dht_RhData[5];//0~4分别分湿度、湿度小数、温度、温度小数、校验和

u8 data Dht_FLAG;
u8 Dht_error=0;
u8 data Dht_count;




void dht_byte(uchar n)
{
	//uchar error;
	uchar data i;
	for(i=0;i<8;i++)
		{
			Dht_FLAG=2;while((!DHT_IO)&&Dht_FLAG++);//等待拉高,低等待
			if(Dht_FLAG==1){Dht_error=4+n;return;}//错误跳出，并报错
			
			Dht_FLAG=2;while(DHT_IO&&Dht_FLAG++);//Dht_count++;对拉高时间进行计时
			
			if(Dht_FLAG>9)Dht_RhData[n]=(Dht_RhData[n]<<1)|0x01;
			else Dht_RhData[n]<<=1;
			if(Dht_FLAG==1){Dht_error=(5+n)+10*i;return;}//错误跳出，并报错
		}
		}
void dht_rdat()
{
	uchar data i;
	DHT_IO=0;
	d_n_ms(25);
	DHT_IO=1;
	
	Dht_FLAG=2;while(DHT_IO&&Dht_FLAG++);//等待DHT拉低,超时跳出
	if(Dht_FLAG==1){Dht_error=1;return;}//错误跳出，并报错
	
	Dht_FLAG=2;while((!DHT_IO)&&Dht_FLAG++);//等待DHT拉高,超时跳出
	if(Dht_FLAG==1){Dht_error=2;return;}//错误跳出，并报错
	
	Dht_FLAG=2;while(DHT_IO&&Dht_FLAG++);//等待DHT拉低,超时跳出
	if(Dht_FLAG==1){Dht_error=3;return;}//错误跳出，并报错
	for(i=0;i<5;i++)
	{
	
		dht_byte(i);
		if(Dht_FLAG==1){Dht_error+=10;return;}//错误跳出，并报错	
	}
	Dht_error=0;

}

