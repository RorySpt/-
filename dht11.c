#include "dht11.h"
#include "delay.h"


uchar Dht_RhData[5];//0~4�ֱ��ʪ�ȡ�ʪ��С�����¶ȡ��¶�С����У���

u8 data Dht_FLAG;
u8 Dht_error=0;
u8 data Dht_count;




void dht_byte(uchar n)
{
	//uchar error;
	uchar data i;
	for(i=0;i<8;i++)
		{
			Dht_FLAG=2;while((!DHT_IO)&&Dht_FLAG++);//�ȴ�����,�͵ȴ�
			if(Dht_FLAG==1){Dht_error=4+n;return;}//����������������
			
			Dht_FLAG=2;while(DHT_IO&&Dht_FLAG++);//Dht_count++;������ʱ����м�ʱ
			
			if(Dht_FLAG>9)Dht_RhData[n]=(Dht_RhData[n]<<1)|0x01;
			else Dht_RhData[n]<<=1;
			if(Dht_FLAG==1){Dht_error=(5+n)+10*i;return;}//����������������
		}
		}
void dht_rdat()
{
	uchar data i;
	DHT_IO=0;
	d_n_ms(25);
	DHT_IO=1;
	
	Dht_FLAG=2;while(DHT_IO&&Dht_FLAG++);//�ȴ�DHT����,��ʱ����
	if(Dht_FLAG==1){Dht_error=1;return;}//����������������
	
	Dht_FLAG=2;while((!DHT_IO)&&Dht_FLAG++);//�ȴ�DHT����,��ʱ����
	if(Dht_FLAG==1){Dht_error=2;return;}//����������������
	
	Dht_FLAG=2;while(DHT_IO&&Dht_FLAG++);//�ȴ�DHT����,��ʱ����
	if(Dht_FLAG==1){Dht_error=3;return;}//����������������
	for(i=0;i<5;i++)
	{
	
		dht_byte(i);
		if(Dht_FLAG==1){Dht_error+=10;return;}//����������������	
	}
	Dht_error=0;

}

