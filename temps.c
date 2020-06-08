#include "temps.h"

uchar temp_High;
uchar temp_Low;
uchar temp_data_sign;//1负，0正
uchar temp_data_Integer;
//uchar temp_data_Decimal;//符号位在最高位，小数位在后四位。
uchar temp_data_Decimal_r;
uchar error;

uchar Ds18b20_Init()
{
	uchar i;
	TEMP_IO = 0;			 //将总线拉低480us~960us
	i = 90;	
	while(i--);//延时642us
	TEMP_IO = 1;			//然后拉高总线，如果DS18B20做出反应会将在15us~60us后总线拉低
	i = 0;
	while(TEMP_IO)	//等待DS18B20拉低总线
	{
		d_n_ms(1);
		i++;
		if(i>5)//等待>5MS
		{
			return 0;//初始化失败
		}
	
	}
	error=1;
	return 1;//初始化成功
}


void Ds18b20_wByte(uchar dat)
{
	uint i, j;
	for(j=0; j<8; j++)
	{
		TEMP_IO = 0;	     	  //每写入一位数据之前先把总线拉低1us
		i++;
		TEMP_IO = dat & 0x01;  //然后写入一个数据，从最低位开始
		i=6;
		while(i--); //延时68us，持续时间最少60us
		TEMP_IO = 1;	//然后释放总线，至少1us给总线恢复时间才能接着写入第二个数值
		dat >>= 1;
	}
}

uchar Ds18b20_rByte()
{
	
	uchar byte, bi;
	uint i, j;	
	for(j=8; j>0; j--)
	{
		TEMP_IO = 0;//先将总线拉低1us
		i++;
		TEMP_IO = 1;//然后释放总线
		i++;
		i++;//延时6us等待数据稳定
		bi = TEMP_IO;	 //读取数据，从最低位开始读取
		/*将byte左移一位，然后与上右移7位后的bi，注意移动之后移掉那位补0。*/
		byte = (byte >> 1) | (bi << 7);						  
		i = 4;		//读取完之后等待48us再接着读取下一个数
		while(i--);
	}				
	return byte;
}
/*******************************************************************************
* 函 数 名         : Ds18b20ChangTemp
* 函数功能		   : 让18b20开始转换温度
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void  Ds18b20_ChangTemp()
{
	Ds18b20_Init();
	{d_n_ms(1);
	Ds18b20_wByte(0xcc);		//跳过ROM操作命令		 
	Ds18b20_wByte(0x44);	    //温度转换命令
	//d_n_ms(100);	//等待转换成功，而如果你是一直刷着的话，就不用这个延时了
	}
}

void  Ds18b20_rTempcom()
{	

	Ds18b20_Init();
	d_n_ms(1);
	Ds18b20_wByte(0xcc);	 //跳过ROM操作命令
	Ds18b20_wByte(0xbe);	 //发送读取温度命令
}
/*******************************************************************************
* 函 数 名         : Ds18b20ReadTemp
* 函数功能		   : 读取温度
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void Ds18b20_rTemp()
{

	//Ds18b20_ChangTemp();			 	//先写入转换命令
	Ds18b20_rTempcom();			//然后等待转换完后发送读取温度命令
	temp_Low = Ds18b20_rByte();		//读取温度值共16位，先读低字节
	temp_High = Ds18b20_rByte();		//再读高字节
}

void temp_data_collation()
{
		if(temp_High&0xf0){
			temp_data_Integer=~((temp_Low>>4)+(temp_High<<4))+1;//负数补码取反加一得绝对值的原码
			temp_data_Decimal_r=6*(0x10-(temp_Low&0x0f))+(0x10-(temp_Low&0x0f))/4;
			temp_data_sign=1;
		}
		else
		{
			temp_data_Integer=(temp_Low>>4)|(temp_High<<4);
			temp_data_Decimal_r=6*(temp_Low&0x0f)+(temp_Low&0x0f)/4;//小数四位，实际数值用分数表示应该是dec/16(2^4=16)，1/16=0.0625,用n位十进制整数表示就是(dec/16)*10^n,用计算机来算步骤是dec*0.0625*10^n,这里n取二，结果也就是用16进制整数表示的两位十进制小数
			temp_data_sign=0;
		}
		if(temp_data_Decimal_r%10>4)temp_data_Decimal_r=temp_data_Decimal_r/10+1;//四舍五入，保留一位小数
		else temp_data_Decimal_r/=10;
}

