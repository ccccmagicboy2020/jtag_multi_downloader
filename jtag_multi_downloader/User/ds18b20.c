#include "ds18b20.h"
//#include "delay.h"	

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK miniSTM32开发板
//DS18B20驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
  
const unsigned char b20rom[4][8]={
{0x28,0xff,0xdb,0x95,0x54,0x14,0x1,0x6a},	
{0x28,0xff,0x7c,0x92,0x54,0x14,0x1,0x8e},
{0x28,0xff,0xdb,0x95,0x54,0x14,0x1,0x6a},
{0x28,0xff,0x7c,0x92,0x54,0x14,0x1,0x8e},

};	 

//复位DS18B20
void DS18B20_Rst(void)	   
{                 
	//DS18B20_IO_OUT(); //SET PA0 OUTPUT
	GPIOB->CRL&=0XFFFFFF0F;
	GPIOB->CRL|=0X30;
	DS18B20_DQ_OUT=1;
    DS18B20_DQ_OUT=0; //拉低DQ
    delay_us(700);    //拉低750us
    DS18B20_DQ_OUT=1; //DQ=1 
	delay_us(300);     //300US
}
//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
uint8_t DS18B20_Check(void) 	   
{   
	uint8_t retry=0;
	DS18B20_IO_IN();//SET PA0 INPUT	 
    while (DS18B20_DQ_IN&&retry<200)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)
		return 1;	    
	return 0;
}
//从DS18B20读取一个位
//返回值：1/0
uint8_t DS18B20_Read_Bit(void) 			 // read one bit
{
    uint8_t data;
	DS18B20_IO_OUT();//SET PA0 OUTPUT
    DS18B20_DQ_OUT=0; 
	delay_us(2);
    DS18B20_DQ_OUT=1; 
	DS18B20_IO_IN();//SET PA0 INPUT
	delay_us(12);
	if(DS18B20_DQ_IN)data=1;
    else data=0;	 
    delay_us(50);           
    return data;
}
//从DS18B20读取一个字节
//返回值：读到的数据
uint8_t DS18B20_Read_Byte(void)    // read one byte
{        
    uint8_t i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}
//写一个字节到DS18B20
//dat：要写入的字节
void DS18B20_Write_Byte(uint8_t dat)     
 {             
    uint8_t j;
    uint8_t testb;
	DS18B20_IO_OUT();//设置 PA0 为输出模式;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_DQ_OUT=0;// Write 1
            delay_us(2);                            
            DS18B20_DQ_OUT=1;
            delay_us(60);             
        }
        else 
        {
            DS18B20_DQ_OUT=0;// Write 0
            delay_us(60);             
            DS18B20_DQ_OUT=1;
            delay_us(2);                          
        }
    }
}
//开始温度转换
void DS18B20_Start(void)// ds1820 start convert
{   						               
    DS18B20_Rst();	//复位ds18b20   
		DS18B20_Check();//检测ds18b20是否存在	 
    DS18B20_Write_Byte(0xcc);// skip rom  写入字节
    DS18B20_Write_Byte(0x44);// convert   
} 
//初始化DS18B20的IO口 DQ 同时检测DS18B20的存在
//返回1:不存在
//返回0:存在    	 
uint8_t DS18B20_Init(void)
{
	/*
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PORTA口时钟 
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				//PORTA0 推挽输出
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOA,GPIO_Pin_0);    //输出1
*/
	DS18B20_Rst();

	return DS18B20_Check();
}  
//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
short DS18B20_Get_Temp(void)
{
    uint8_t temp;//,k;
    uint8_t TL,TH;
	short tem;
    DS18B20_Start ();                    // ds1820 start convert
 //   DS18B20_Rst();
 //   DS18B20_Check();	 
 
	/*
			DS18B20_Write_Byte(0x55);
			for(k=0;k<8;k++)
			{
				DS18B20_Write_Byte(b20rom[0][k]);
			}
		*/	
		delay_ms(500);
		DS18B20_Rst();
	
		DS18B20_Write_Byte(0xCC);
    DS18B20_Write_Byte(0xbe);// convert	    
    TL=DS18B20_Read_Byte(); // LSB   
    TH=DS18B20_Read_Byte(); // MSB  
	    	  
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//温度为负  
    }else temp=1;//温度为正	  	  
    tem=TH; //获得高八位
    tem<<=8;    
    tem+=TL;//获得底八位
    tem=(float)tem*0.625;//转换     
	if(temp)return tem; //返回温度值
	else return -tem;    
} 
 
