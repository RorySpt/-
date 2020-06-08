#include "temps.h"

uchar temp_High;
uchar temp_Low;
uchar temp_data_sign;//1����0��
uchar temp_data_Integer;
//uchar temp_data_Decimal;//����λ�����λ��С��λ�ں���λ��
uchar temp_data_Decimal_r;
uchar error;

uchar Ds18b20_Init()
{
	uchar i;
	TEMP_IO = 0;			 //����������480us~960us
	i = 90;	
	while(i--);//��ʱ642us
	TEMP_IO = 1;			//Ȼ���������ߣ����DS18B20������Ӧ�Ὣ��15us~60us����������
	i = 0;
	while(TEMP_IO)	//�ȴ�DS18B20��������
	{
		d_n_ms(1);
		i++;
		if(i>5)//�ȴ�>5MS
		{
			return 0;//��ʼ��ʧ��
		}
	
	}
	error=1;
	return 1;//��ʼ���ɹ�
}


void Ds18b20_wByte(uchar dat)
{
	uint i, j;
	for(j=0; j<8; j++)
	{
		TEMP_IO = 0;	     	  //ÿд��һλ����֮ǰ�Ȱ���������1us
		i++;
		TEMP_IO = dat & 0x01;  //Ȼ��д��һ�����ݣ������λ��ʼ
		i=6;
		while(i--); //��ʱ68us������ʱ������60us
		TEMP_IO = 1;	//Ȼ���ͷ����ߣ�����1us�����߻ָ�ʱ����ܽ���д��ڶ�����ֵ
		dat >>= 1;
	}
}

uchar Ds18b20_rByte()
{
	
	uchar byte, bi;
	uint i, j;	
	for(j=8; j>0; j--)
	{
		TEMP_IO = 0;//�Ƚ���������1us
		i++;
		TEMP_IO = 1;//Ȼ���ͷ�����
		i++;
		i++;//��ʱ6us�ȴ������ȶ�
		bi = TEMP_IO;	 //��ȡ���ݣ������λ��ʼ��ȡ
		/*��byte����һλ��Ȼ����������7λ���bi��ע���ƶ�֮���Ƶ���λ��0��*/
		byte = (byte >> 1) | (bi << 7);						  
		i = 4;		//��ȡ��֮��ȴ�48us�ٽ��Ŷ�ȡ��һ����
		while(i--);
	}				
	return byte;
}
/*******************************************************************************
* �� �� ��         : Ds18b20ChangTemp
* ��������		   : ��18b20��ʼת���¶�
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/

void  Ds18b20_ChangTemp()
{
	Ds18b20_Init();
	{d_n_ms(1);
	Ds18b20_wByte(0xcc);		//����ROM��������		 
	Ds18b20_wByte(0x44);	    //�¶�ת������
	//d_n_ms(100);	//�ȴ�ת���ɹ������������һֱˢ�ŵĻ����Ͳ��������ʱ��
	}
}

void  Ds18b20_rTempcom()
{	

	Ds18b20_Init();
	d_n_ms(1);
	Ds18b20_wByte(0xcc);	 //����ROM��������
	Ds18b20_wByte(0xbe);	 //���Ͷ�ȡ�¶�����
}
/*******************************************************************************
* �� �� ��         : Ds18b20ReadTemp
* ��������		   : ��ȡ�¶�
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/

void Ds18b20_rTemp()
{

	//Ds18b20_ChangTemp();			 	//��д��ת������
	Ds18b20_rTempcom();			//Ȼ��ȴ�ת������Ͷ�ȡ�¶�����
	temp_Low = Ds18b20_rByte();		//��ȡ�¶�ֵ��16λ���ȶ����ֽ�
	temp_High = Ds18b20_rByte();		//�ٶ����ֽ�
}

void temp_data_collation()
{
		if(temp_High&0xf0){
			temp_data_Integer=~((temp_Low>>4)+(temp_High<<4))+1;//��������ȡ����һ�þ���ֵ��ԭ��
			temp_data_Decimal_r=6*(0x10-(temp_Low&0x0f))+(0x10-(temp_Low&0x0f))/4;
			temp_data_sign=1;
		}
		else
		{
			temp_data_Integer=(temp_Low>>4)|(temp_High<<4);
			temp_data_Decimal_r=6*(temp_Low&0x0f)+(temp_Low&0x0f)/4;//С����λ��ʵ����ֵ�÷�����ʾӦ����dec/16(2^4=16)��1/16=0.0625,��nλʮ����������ʾ����(dec/16)*10^n,�ü�������㲽����dec*0.0625*10^n,����nȡ�������Ҳ������16����������ʾ����λʮ����С��
			temp_data_sign=0;
		}
		if(temp_data_Decimal_r%10>4)temp_data_Decimal_r=temp_data_Decimal_r/10+1;//�������룬����һλС��
		else temp_data_Decimal_r/=10;
}

