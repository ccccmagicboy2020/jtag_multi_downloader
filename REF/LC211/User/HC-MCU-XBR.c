#define ALLOCATE_EXTERN
#include "HC89S003F4.h"

////累计采样9600次后进行判断，耗时250ms
////串口波特率57600

#define  VERSION  0X21

#define  TH_LOW		30000
#define  TH_HIGH	3000000

#define  TH_DEF		60000
	
//允许噪声值偏差范围
#define  MAX_DELTA0  20000
#define  MAX_DELTA1  80000

#define  MAX_DELAY	1800
//最大延时秒数

//感光门限-30对应8LUX左右的AD值,设置为255表示不检测感光
#define  LIGHT_TH0 255
//30

//一个检测周期为0.25秒
//#define DELAY_NUM 20

#define LIGHT_ON P0_6=1
#define LIGHT_OFF P0_6=0

//void Delay_2us(u16 Cnt);		//延时函数

// bit Timer1_FLAG;
// bit Time_10mS_FLAG;
// bit Time_100mS_FLAG;
// bit Time_200mS_FLAG;
// bit Time_500mS_FLAG;
// bit Time_1S_FLAG;
// bit Time_10S_FLAG;
// bit Time_1Min_FLAG;
// bit Time_1H_FLAG;

ulong Timer_Counter=0;

 //u8 Uart1_RX_Buff[10] = {0};					 //用于存放UART1接收数据
 //u8 Uart1_RX_Cnt = 0;								 //UART1接收计数
 //u8 Uart1_TX_Buff[20] = {0};					 //用于存放UART1发送数据
 //u8 Uart1_TX_Cnt = 0;								 //UART1发送计数
 
// u16 AN1_Data = 0;
// u16 AN7_Data = 0;
// u8 Uart_Cnt = 0;


	u8 SUM1_counter=0,SUM1_num=64,SUM0_num=12;
	ulong SUM1=0,SUM0=0,SUM=0,ALL_SUM1=0,SUM10=0,SUM16=0,SUM01,SUM2;
	u16 k,times=0,start_times=1;
	ulong TH,MAX_DELTA;
	u8 alarm_times=0,stop_times=0;

	uint average,t,LIGHT=0;
	u8 light_ad,light_ad0,check_light_times=8,calc_average_times=0,check_sum,send_byte;
	u8 LIGHT_TH;
	u16 DELAY_NUM;
	
	u8 RXnum=0,TXdata[15],RXdata[7];
	
 
void Flash_EraseBlock(unsigned int fui_Address);//扇区擦除
void FLASH_WriteData(unsigned char fui_Address, unsigned int fuc_SaveData);//写入一个数据
//void Flash_WriteArr(unsigned int fui_Address,unsigned char fuc_Length,unsigned char *fucp_SaveArr);//写入任意长度数据
void Flash_ReadArr(unsigned int fui_Address,unsigned char fuc_Length,unsigned char *fucp_SaveArr);//读取任意长度数据

//unsigned char guc_Write_a[5] = {0};	//写入数据
unsigned char guc_Read_a[5] = {0x00};			//用于存放读取的数据
// unsigned char guc_Uartflag = 0;					  //发送标志位
// unsigned char guc_Uartcnt = 0;					  //发送计数
// unsigned char guc_Uartbuf_a[2] = {0x00};	//缓存数组

void Flash_ReadArr(unsigned int fui_Address,unsigned char fuc_Length,unsigned char *fucp_SaveArr)
{
	while(fuc_Length--)
	*(fucp_SaveArr++)=*((unsigned char code *)(fui_Address++));//读取数据
}

void Delay_us(uint q)
{
	uint j;
	for(j=0;j<q;j++)
	{
			;
	}
}

void Delay_ms(uint t)
{
	Timer_Counter=0;
	while(Timer_Counter<t)
	{
		WDTC |= 0x10;		//清看门狗
	}
	
}

void Delay_us_1(uint q1)
{
	uint j;
	for(j=0;j<q1;j++)
	{
			;
	}
}
// void Delay_ms(uint t)
// {
// 	for(;t>0;t--)
// 	{
// 		Delay_us_1(1000);
// 		WDTC |= 0x10;		//清看门狗
// 	}
// }

/***************************************************************************************
  * @说明  	系统初始化函数
  *	@参数	  无
  * @返回值 无
  * @注		  无
***************************************************************************************/
void InitSYS()
{
	/********************************系统频率初始化***************************************/
	
	CLKSWR = 0X50;//32MHZ  //0x51;						         //选择内部高频RC为系统时钟，内部高频RC 2分频，Fosc=16MHz
	CLKDIV = 0X02;//2分频  //0x01;						         //Fosc 1分频得到Fcpu，Fcpu=16MHz 
	FREQ_CLK = 0x10;									 //IAP频率
	
	/**********************************低压复位初始化**************************************/
	
//	BORC = 0xC0;											 //使能低压复位1.8V，带消抖使能
//	BORDBC = 0x01;										 //消抖时间BORDBC*8TCPU+2TCPU
	
	/***********************************看门口初始化***************************************/
	WDTC = 0x5F;											 //允许WDT复位，空闲模式下禁止WDT，选择1024分频（内部低频时钟44K）
	WDTCCR = 0X20;	//0X20/44	=0.73秒						//0xFF;	 //溢出时间约6秒
	//溢出计算时间=（WDT分频系数*（WDTCCR+1））/内部低频RC频率
}

/***************************************************************************************
  * @说明  	定时器初始化函数
  *	@参数	  无
  * @返回值 无
  * @注		  无
***************************************************************************************/
void Timer_Init()
{
	/**********************************TIM1配置初始化**************************************/
	TCON1 = 0x00;						  //T1定时器时钟为Fosc
	TMOD = 0x01;						  //T1-16位重装载定时器/计数器,T0-16位定时器

	//Tim1计算时间 	= (65536 - 0xFACB) * (1 / (Fosc /Timer分频系数))
	//				= 1333 / (16000000 / 12)
	//				= 1 ms

	//定时1ms
	//反推初值 	= 65536 - ((1/1000) / (1/(Fosc / Timer分频系数)))
	//		   	= 65536 - ((1/1000) / (1/(16000000 / 12)))
	//			= 65536 - 1333
	//			= 0xFACB
	
	TH1 = 0XF5;//0xFA;
	TL1 = 0X95;//0xCB;
	IE |= 0x08;							  //打开T1中断
	TCON |= 0x40;						  //使能T1
    
	
		
	TH0 = 0X97;//0xCB;
	TL0 = 0XD6;//0xEB;							  //T0定时时间10ms
	
	TCON |= 0x10;						  //使能T0
}

/***************************************************************************************
  * @说明  	UART1初始化函数
  *	@参数	  无
  * @返回值 无
  * @注		  无
***************************************************************************************/
void UART1_Init()
{
	/**********************************UART配置初始化**************************************/
	P2M0 = P2M0&0xF0|0x08;				      //P20设置为推挽输出
	P0M2 = P0M2&0xF0|0x02;				      //P04设置为上拉输入
	P0_4 = 1;
	TXD_MAP = 0x20;						          //TXD映射P20
	RXD_MAP = 0x04;						          //RXD映射P04		
	T4CON = 0x06;						            //T4工作模式：UART1波特率发生器
	
	//波特率计算
	//波特率 = 1/16 * (T4时钟源频率 / 定时器4预分频比) / (65536 - 0xFF98)
	//       = 1/16 * ((16000000 / 1) / 104)
	//		 = 9615.38(误差0.16%)

	//波特率9600
	//反推初值 = (65536 - ((T4时钟源频率 / 定时器4预分频比) * (1 / 16)) / 波特率)
	//		   = (65536 - (16000000 * (1 / 16) / 9600))
	//		   = (65536 - 104.167)
	//         = FF98
	//0xFF98->9600
	//0xFFCC->19200
	//0xFFEF->57600
	
	
  TH4 = 0xFF;
	TL4 = 0XDD;//57600@32MHZ   //0x98;	//波特率9600		//0xEE;				//波特率56000
	SCON2 = 0x02;						             //8位UART，波特率可变
	SCON = 0x10;					               //允许串行接收
	IE |= 0X10;							             //使能串口中断
	//EA = 1;							              	 //使能总中断
}

/***************************************************************************************
  * @说明  	ADC初始化函数
  *	@参数	  无
  * @返回值 无
  * @注		  无
***************************************************************************************/
void ADC_Init()
{
	
	ADCC0 |= 0x03;						//参考源为内部2V
	ADCC0 |= 0x80;						  //打开ADC转换电源
	Delay_us(20);					 	//延时20us，确保ADC系统稳定
	ADCC1 = 0x01;						  //选择外部通道1
	ADCC2 = 0x4F;						  //转换结果12位数据，数据右对齐，ADC时钟16分频-1MHZ//0X4B-8分频//0X41-4分频
}

/***************************************************************************************
  * @说明  	IO口初始化函数
  *	@参数	  无
  * @返回值 无
  * @注		  无
***************************************************************************************/
void GPIO_Init()
{
	//P0M0分高4位与低4位，低4位控制P00输入输出，高4位控制P01输入输出，其他以此类推
	//P0M1高4控制P03，低4控制P02
	//P1M2高4控制P15，低4控制P14
	P0M0 = P0M0&0xF0|0x08;		      //P00设置为推挽输出
	
	P0M0 = P0M0&0x0F|0x30;				  //P01设置为模拟输入
	P0M3 = P0M3&0x0F|0x30;				  //P07设置为模拟输入
//	P0M0 = P0M0&0x0F|0x80;		      //P01设置为推挽输出
	
	P0M3 = P0M3&0xF0|0x08;		      //P06设置为推挽输出
}

// u16 Read_ADC(u8 Channel)
// {
// 	u16 ADC_Temp;
// 	ADCC1 = Channel;						  //选择外部通道1
//   ADCC0 |= 0x40;					//启动ADC转换
// 	while(!(ADCC0&0x20));		//等待ADC转换结束
// 	ADCC0 &=~ 0x20;					//清除标志位
// 	ADC_Temp = ADCR;				//获取ADC的值
// 	return ADC_Temp;
// }

/***************************************************************************************
  * @说明  	系统时钟计数函数
  *	@参数	  无
  * @返回值 无
  * @注		  无
***************************************************************************************/
/*
void System_Time_Cnt(void)
{
    static u32 Timer1Count1 = 0;
    static u8 Timer1Count2 = 0;
    u32 Tampe = 1;
    if(Timer1_FLAG)              //1ms
    {
        Timer1_FLAG = 0;
        Timer1Count1 ++;
        Tampe = Timer1Count1 % 10;
        if(Tampe == 0)
            Time_10mS_FLAG = 1;
        Tampe = Timer1Count1 % 100;
        if(Tampe == 0)
            Time_100mS_FLAG = 1;
        Tampe = Timer1Count1 % 200;
        if(Tampe == 0)
            Time_200mS_FLAG = 1;
        Tampe = Timer1Count1 % 500;
        if(Tampe == 0)
            Time_500mS_FLAG = 1;
        Tampe = Timer1Count1 % 1000;
        if(Tampe == 0)
            Time_1S_FLAG = 1;
        Tampe = Timer1Count1 % 10000;
        if(Tampe == 0)
        {
            Time_10S_FLAG = 1;
            Timer1Count1 = 0;
            Timer1Count2 ++;
            Tampe = Timer1Count2 % 6;
            if(Tampe == 0)
                Time_1Min_FLAG = 1;
            Tampe = Timer1Count2 % 60;
            if(Tampe == 0)
            {
                Time_1H_FLAG = 1;
                Timer1Count2 = 0;
            }
        }
    }
}
*/

/***************************************************************************************
  * @说明  	系统任务处理函数
  *	@参数	  无
  * @返回值 无
  * @注		  无
***************************************************************************************/

/*
void System_Task(void)
{
    u8 temp;
		if(Time_10mS_FLAG)
    {
        Time_10mS_FLAG = 0;
        //User TASK
        
        
        //User TASK
        if(Time_100mS_FLAG)
        {
            Time_100mS_FLAG = 0;
            //User TASK

        
        
            //User TASK
        }
        if(Time_200mS_FLAG)
        {
            Time_200mS_FLAG = 0;
            //User TASK

        
        
            //User TASK
        }
        if(Time_500mS_FLAG)
        {
            Time_500mS_FLAG = 0;
            //User TASK
						AN1_Data = Read_ADC(0x01);
					
            //User TASK
        }
         if(Time_1S_FLAG)
        {
            Time_1S_FLAG = 0;
            //User TASK

						temp = 0xFF & (AN1_Data >> 8);
						SBUF = temp;
						while(!(SCON & 0x02));
						SCON &=~ 0x02;			           //清除发送中断标志位
						temp = 0xFF & AN1_Data;
						SBUF = temp;
						while(!(SCON & 0x02));
						SCON &=~ 0x02;			           //清除发送中断标志位
						P0_0 =~ P0_0;	
						//User TASK
            
        }
        if(Time_10S_FLAG)
        {
            Time_10S_FLAG = 0;
            //User TASK
            

            //User TASK
        }
        if(Time_1Min_FLAG)
        {
            Time_1Min_FLAG = 0;
            //User TASK
        
        
        
            //User TASK
        }
        if(Time_1H_FLAG)
        {
            Time_1H_FLAG = 0;
            //User TASK
        
        
        
            //User TASK
        }
    }
}
*/

void send_data(u8 d)
{
		SBUF = d;
		while(!(SCON & 0x02));
		SCON &=~ 0x02;
}

uchar read_ad(uchar ch)
{
	u8 i;
	uint  ad_sum;
	
//	switch (ch)
//	{
//		case 3:
//			ADC_P16_AN3;
//			break;
//		case 4:
//			ADC_P15_AN4;
//			break;
//		case 5:
//			ADC_P14_AN5;
//			break;
//	}
	
	
// 	if(ch==3)
// 	{
// 		ADC_P16_AN3;
// 	}
// 	else if(ch==4)
// 	{
// 		ADC_P15_AN4;
// 	}
	
	ADCC1 = ch;						  //选择外部通道
  ADCC0 |= 0x40;					//启动ADC转换
	while(!(ADCC0&0x20));		//等待ADC转换结束
	ADCC0 &=~ 0x20;					//清除标志位
	
	//ADC_Temp = ADCR;				//获取ADC的值	
	
// 	delay_ms(1);
// 	
// 	ADC_TG; 
	Delay_us(100);
	
	ad_sum=0;
	//ADC_INT_IF_CLR; //清中断标志位
	
	
	for(i=0;i<16;i++)
	{
// 		ADC_TG; 
// 		while(ADC_IF==0){};
// 		//adc_data = ADC_DATA_RD();
// 			
// 		k=ADC_DH;
// 		k =k <<8;
// 		k+=ADC_DL;
// 			
// 		ADC_INT_IF_CLR; //清中断标志位
// 		ad_sum+=k;
			

		ADCC0 |= 0x40;					//启动ADC转换
		while(!(ADCC0&0x20));		//等待ADC转换结束
		ADCC0 &=~ 0x20;					//清除标志位
		ad_sum += ADCR;				//获取ADC的值
		
		Delay_us(20);
	}
	
	//ADC_P14_AN5;
	ADCC1 =1;
	i=ad_sum>>8;
	
	Delay_us(100);
	return(i);
	
}

void set_var(void)
{
	
	Flash_ReadArr(0x2F00,5,guc_Read_a);	//读取地址0x2F00所在扇区
	
	TH=guc_Read_a[0];
	TH<<=8;
	TH+=guc_Read_a[1];
	TH*=1000;
	if(TH<TH_LOW||TH>TH_HIGH)TH=TH_DEF;
	
		
	LIGHT_TH=guc_Read_a[2];

  if(LIGHT_TH==0)LIGHT_TH=LIGHT_TH0;
	else 
		if(LIGHT_TH==0XFE)LIGHT_TH=255;

	DELAY_NUM=guc_Read_a[3];
	DELAY_NUM<<=8;
	DELAY_NUM+=guc_Read_a[4];
	if(DELAY_NUM==0||DELAY_NUM>MAX_DELAY)DELAY_NUM=5;
	
	DELAY_NUM<<=2;
	
}

/***************************************************************************************
  * @说明  	主函数
  *	@参数	  无
  * @返回值 无
  * @注		  无
***************************************************************************************/
void main()
{
	u8 i,j;
	InitSYS();
	GPIO_Init();
	LIGHT_ON;
	
	Timer_Init();
	UART1_Init();
	ADC_Init();

	LVDC = 0xAA;						  //LVD设置2.4V,禁止中断
	//	消抖时间 = 	(0xFF + 2) * 1/Fcpu
	//			 =	(0xFF + 2) / 16000000	（当前的CPU时钟）
	//			 =	16.0625us
	LVDDBC = 0xFF;					  //设置消抖时间
	LVDC &=~ 0x08;			      //清除LVD中断标志位 	
// 	
	EA=1;
	
	Delay_ms(200);
	
	//LIGHT_ON;
	
	light_ad=read_ad(7);
	light_ad0=light_ad;
	
	EA=0;
	set_var();
	
	//for(i=0;i<5;i++)send_data(guc_Read_a[i]);
	
// 	TXdata[0]=VERSION;
// 	t=TH/1000;
// 	TXdata[1]=t>>8;
// 	TXdata[2]=t;
// 	TXdata[3]=LIGHT_TH;
// 	t=DELAY_NUM/4;
// 	TXdata[4]=t>>8;
// 	TXdata[5]=t;

// 	TXdata[6]=light_ad;

// 	for(i=0;i<7;i++)send_data(TXdata[i]);	
	
	EA=1;
	
//等待直流电压稳定
	j=0;
	while(1)
	{
	  SUM=0;
		
		
// 	  for(i=0;i<4;i++)	//0.52s
// 	  {
// 	  	for(t=0;t<8192;t++)	//0.13s
// 	  	{

// 				ADCC0 |= 0x40;					//启动ADC转换
// 				while(!(ADCC0&0x20));		//等待ADC转换结束
// 				ADCC0 &=~ 0x20;					//清除标志位
// 				k = ADCR;				//获取ADC的值
// 				
// 				SUM+=k;
// 	
// 	  	}
// 			
// 			WDTC |= 0x10;		//清看门狗
			

	  	for(i=0;i<128;i++)	//
	  	{

				ADCC0 |= 0x40;					//启动ADC转换
				while(!(ADCC0&0x20));		//等待ADC转换结束
				ADCC0 &=~ 0x20;					//清除标志位
				//k = ADCR;				//获取ADC的值
				
				SUM+=ADCR;
				
	  	}

//发送感光AD值
// 				send_byte=0xFA;
// 				check_sum=send_byte;
// 				send_data(send_byte);

// 				//send_byte=light_ad;
// 				check_sum+=light_ad;
// 				send_data(light_ad);

// 				check_sum+=light_ad;
// 				send_data(light_ad);
// 				
// 				check_sum+=1;
// 				send_data(check_sum);



	//}
		
		Delay_ms(400);
			

		
		//WDTC |= 0x10;		//清看门狗

		i=SUM>>11;
		if((i>12)&&(i<141)&&(j>20))break;

		j++;

		if(j>80)break;	//??35????????????1.1V???????

	}


	
	//Delay_ms(200);

	LIGHT_OFF;
	
	Delay_ms(300);
	
	SUM=0;


	  for(i=0;i<8;i++)
	  {
	  	for(t=0;t<8192;t++)	
	  	{
// 				ADC_TG; 
// 				while(ADC_IF==0){};
// 				//adc_data = ADC_DATA_RD();
// 					
// 				k = ADC_DH<<8;
// 				//adc_data =adc_data <<8;
// 				k+= ADC_DL;
// 					
// 				ADC_INT_IF_CLR; //清中断标志位
		
				ADCC0 |= 0x40;					//启动ADC转换
				while(!(ADCC0&0x20));		//等待ADC转换结束
				ADCC0 &=~ 0x20;					//清除标志位
				k = ADCR;				//获取ADC的值
				
				SUM+=k;
	
	  	}
			WDTC |= 0x10;		//清看门狗

	  }

	  average=SUM>>16;


// 	light_ad=read_ad(7);
// 	light_ad0=light_ad;

// 	Delay_ms(4);	//4ms

	SUM=0;
	while(1)
	{
	//	j=0;
		
		while(1)
		{
// 				ADC_TG; 
// 				while(ADC_IF==0){};
// 				//adc_data = ADC_DATA_RD();
// 					
// 				k = ADC_DH<<8;
// 				//adc_data =adc_data <<8;
// 				k+= ADC_DL;
// 					
// 				ADC_INT_IF_CLR; //清中断标志位

			ADCC0 |= 0x40;					//启动ADC转换
			while(!(ADCC0&0x20));		//等待ADC转换结束
			ADCC0 &=~ 0x20;					//清除标志位
			k = ADCR;				//获取ADC的值
			
			times++;
			
			SUM+=k;

			//计算信号值与直流电压偏差值
			if(k>average){k-=average;}
			else {k=average-k;}
			SUM1+=k;


			if(times>=9600)	//250ms		//12800)	//330ms
			{

				WDTC |= 0x10;		//清看门狗
				
				times=0;

				calc_average_times++;
			
				SUM16+=SUM;
				
				if(calc_average_times>=10)	//每2.5S重新计算一次直流电压值
				{
					calc_average_times=0;
					
					//SUM16>>=10;
					SUM16/=96000;//102400;
					average+=SUM16;
					average/=2;
					SUM16=0;
				}
				
				if(check_light_times<8)	//2s	读取一次感光AD值
				{
					check_light_times++;
				}
				else
				{
					if(LIGHT==0)	
					{
						//light_ad=READ_LIGHT();
						light_ad=read_ad(7);

						if((light_ad<=(light_ad0+2))&&(light_ad0<=(light_ad+2)))
							light_ad=light_ad0;

						light_ad0=light_ad;

						check_light_times=0;
					}
				}

				if(SUM0==0)
				{
					SUM0=SUM1+5000;	
					if(start_times==0&&SUM0>1000000)SUM0=1000000;
				}

				if(SUM1_counter==0)
				{
					SUM10=SUM1;
					MAX_DELTA=1;//SUM10>>3;		
					//if(MAX_DELTA<MAX_DELTA0)MAX_DELTA=MAX_DELTA0;
				}
				
					if((SUM10<(SUM1+MAX_DELTA))&&(SUM1<(SUM10+MAX_DELTA)))//???????????
					{
						SUM1_counter++;
						ALL_SUM1+=SUM1;
						SUM10=ALL_SUM1/SUM1_counter;
						MAX_DELTA=SUM10>>3;
						if(MAX_DELTA<MAX_DELTA0)MAX_DELTA=MAX_DELTA0;
						if(MAX_DELTA>MAX_DELTA1)MAX_DELTA=MAX_DELTA1;
						
						if(SUM0>SUM10)
						{
							SUM=SUM0-SUM10;
							if(SUM>80000)SUM0_num=6;
							else if(SUM>40000)SUM0_num=9;
							else SUM0_num=12;
						}
						else
						{
							SUM0_num=12;
						}
						
						if((SUM1_counter>=SUM0_num)&&(SUM10<SUM0))
						{
							if(SUM1_num>16)	//???????????????
							{
								if(SUM0_num<=9)SUM0=SUM10;
								else if(SUM0>(SUM10+4000))
								{
									SUM0+=SUM10;
									SUM0/=2;
								}
								SUM1_counter=0;
								ALL_SUM1=0;
							}
						}

						else if(SUM1_counter>=SUM1_num)
						{
							
// 							if(SUM0>SUM10)
// 							{
// 								if(SUM1_num>16)SUM0=SUM10;	//???????????????
// 							}
// 							else 
								
							if(SUM10>(SUM0+4000))
							{
								SUM=SUM10-SUM0;

								if((SUM10<9000000)&&(SUM<1000000))
		//????????????,???????????????100000?,???????
								{
									if(SUM1_num>16)	//????????????
									{
										SUM0+=SUM10;
										SUM0/=2;
									}
									else
									{
										if(SUM>600000)SUM1_num=16;
										else if(SUM>300000)SUM1_num=10;
										else SUM1_num=6;
										if(SUM1_counter>=SUM1_num)
										{
											SUM0+=SUM10;
											SUM0/=2;
										}
									}
								}
							}
// 							else if((LIGHT>0)&&(TH==TH_LOW))
// 							{
// 								SUM=SUM10-SUM0;	
// 								TH+=SUM;		//?????????????
// 							}

							if(SUM1_counter>=SUM1_num)
							{
								SUM1_counter=0;
								ALL_SUM1=0;
							}
						}
					}
					else
					{
						SUM1_counter=0;
						ALL_SUM1=0;
					}


// 				//send_byte=0x35;
// 				check_sum=0xFa;
 				//send_data(0xFa);

// 				send_byte=TH>>15;
// 				check_sum+=send_byte;
// 				send_data(send_byte);

// 				//k=TH;
// 				send_byte=TH>>10;
// 				check_sum+=send_byte;
// 				send_data(send_byte);
// 					
// 				send_byte=LIGHT_TH;
// 				check_sum+=send_byte;
// 				send_data(send_byte);					

// 				//send_byte=light_ad;
// 				check_sum+=light_ad;
// 				send_data(light_ad);

// 				send_byte=average>>4;
// 				check_sum+=send_byte;
// 				send_data(send_byte);

// 				send_byte=SUM0>>16;
// 				check_sum+=send_byte;
// 				send_data(send_byte);

// 				send_byte=SUM0>>8;
// 				check_sum+=send_byte;
// 				send_data(send_byte);

// 				send_byte=SUM1>>16;
// 				check_sum+=send_byte;
// 				send_data(send_byte);

// 				send_byte=SUM1>>8;
// 				check_sum+=send_byte;
// 				send_data(send_byte);

// 				check_sum+=1;
// 				send_data(check_sum);

				if(stop_times>0)	//
				{
					stop_times--;	
					if((SUM0>(SUM01+6000))&&(SUM1<(SUM01+15000)))SUM0=SUM01+6000;
				}
				else
				{
				
					if(start_times>0)
					{
						start_times--;
						
						if(start_times>0)	//???????????,???????250*88ms=22S?????????					
						{
							//start_times++;
/*
							if(SUM0>600000)
							{
								TH=10000;
							}
							else */
							if(SUM0>9000000)
							{
								TH=800000;
							}
							else
							{
								SUM=SUM0+TH;
								if(SUM>10000000)
								{
									TH=10000000-SUM0;
								}
								//if(TH<30000)TH=30000;
							}

						}
						else
						{
							//TH=TH_LOW;
							//start_times=0;
							EA=0;
							set_var();
							EA=1;
						}

					}

					if(SUM1>(SUM0+TH))
					{
						//SUM=SUM1-SUM0;

					//	if(SUM>TH)
					//	{
							if((light_ad<=LIGHT_TH)||(start_times>0))
							{
//								send_data(0xaa);

								
								//if(alarm_times<2)
								//{
								//	alarm_times++;
							//	  }
								//if(alarm_times>=2)	//??????????????
								{
									if(LIGHT==0)SUM01=SUM0;
									LIGHT=1;
							
									//PC3=0;
									LIGHT_ON;

									SUM1_num=6;

	//								send_data(0xaa);

								}
							}

/*
							else
							{
								send_data(0xbb);
								send_data(0xbb);

							}
	*/						
						}
					//	else
					//	{
					//		alarm_times=0;
					//	}
				}

				SUM2=SUM1;
				
				SUM=0;
				SUM1=0;
				
				if(LIGHT>0)	//????
				{
					LIGHT++;
					if(LIGHT>DELAY_NUM)
					{
						LIGHT=0;
						break;
					}
				}				
				
			}


		}
	
		//PC3=1;
		LIGHT_OFF;

		Delay_ms(250);


		
// 		SUM=0;
// 		SUM1=0;
// 		times=0;
		
		SUM16=0;
		calc_average_times=0;
		SUM1_num=64;
		
		stop_times=2;
		//if(start_times==0)TH=TH_LOW;
		check_light_times=6;
		
		SUM1_counter=0;
		ALL_SUM1=0;

//		send_data(0xdd);
//		send_data(0xdd);


	}	
	
// 	while(1)
// 	{
// 		System_Time_Cnt();
// 		System_Task();
// 		WDTC |= 0x10;              //清狗
// 	}

	
	
}

/***************************************************************************************
  * @说明  	T1中断服务函数
  *	@参数	  无
  * @返回值 无
  * @注		  无
***************************************************************************************/
void TIMER1_Rpt(void) interrupt TIMER1_VECTOR
{
	//Timer1_FLAG = 1;		//1mS
	Timer_Counter++;
}

/***************************************************************************************
  * @说明  	UART1中断服务函数
  *	@参数	  无
  * @返回值 无
  * @注		  无
***************************************************************************************/

void UART1_Rpt(void) interrupt UART1_VECTOR
{
	u8 i;
	u16 t;
	
	if(SCON & 0x01)						            //判断接收中断标志位
	{
			//RXdata[0] = SBUF;//转存8位串口接收数据
		  //SCON &=~ 0x01;					          //清除接收中断标志位
		//EA=0;
		if(RXnum>0)	//????
		{
			
			if(TF0==1)
			{
			//	send_data(RXnum);
				RXnum=0;
				RXdata[0]=0;
				
			}
		}
		TH0 = 0X97;//0XCB;  
		TL0 = 0XD6;//0XEB; 	//0X10000-0XCBEB=13333=10MS
		TF0=0;     
		
			RXdata[RXnum]=SBUF;
			RXnum++;
			
		
			if(RXdata[0]==0xfa)
			{
				if(RXnum==3)
				{
				
					check_sum=1;
					check_sum+=RXdata[0];
					check_sum+=RXdata[1];
					Delay_us_1(200);

					if(RXdata[2]==check_sum)
					{
						if(RXdata[1]==0x02)	//0X01
						{
							TXdata[0]=VERSION;
							t=TH/1000;
							TXdata[1]=t>>8;
							TXdata[2]=t;
							TXdata[3]=LIGHT_TH;
							t=DELAY_NUM/4;
							TXdata[4]=t>>8;
							TXdata[5]=t;
						
							TXdata[6]=light_ad;
							TXdata[7]=average>>4;
							TXdata[8]=SUM0>>16;
							TXdata[9]=SUM0>>8;
							TXdata[10]=SUM2>>16;
							TXdata[11]=SUM2>>8;
							TXdata[12]=LIGHT>>8;
							TXdata[13]=LIGHT;
							TXdata[14]=1;
							for(i=0;i<14;i++)TXdata[14]+=TXdata[i];						
							
							//DelayUs(200);
							for(i=0;i<15;i++)send_data(TXdata[i]);
							RXnum=0;

						}
/*
						else if(RXdata[1]==0x12)
						{
							send_data(light_ad);
						}
*/
						else if(RXdata[1]==0x42)
						{
							//send_data(LIGHT_TH);
							if(start_times==1)
							{
								start_times=90;
								TH=TH_HIGH;
									
							//	DelayUs(200);
								send_data(0X42);

								//send_back();
/*
								TXdata[0]=0x42;
								TXdata[1]=0X42;
								TXdata[2]=0X85;

								//DelayUs(200);
								for(i=0;i<3;i++)send_data(TXdata[i]);
*/

							}
							RXnum=0;
						}
						else RXnum=0;

					}
					else RXnum=0;
				}
			}
			else if(RXdata[0]==0xfd)
			{
				if(RXnum==7)
				{
					check_sum=1;
				
					for(i=0;i<6;i++)check_sum+=RXdata[i];

					if(RXdata[6]==check_sum)
					{

						Delay_us_1(200);
						send_data(0XFD);
						RXnum=0;


						//send_back();
/*
					
						TXdata[0]=0xFD;
						TXdata[1]=5;
						TXdata[2]=3;

						DelayUs(200);
						for(i=0;i<3;i++)send_data(TXdata[i]);
						RXnum=0;
						
*/
						Flash_EraseBlock(0X2F00);
						Delay_us_1(10000);
						
						for(i=0;i<5;i++)//EEPROMwrite(i,RXdata[i]);	
						{
							FLASH_WriteData(RXdata[i+1],0X2F00+i);
							Delay_us_1(100);
							
						}
/*
						EEPROMwrite(0x01,RXdata[1]);	//TH>>16
						EEPROMwrite(0x02,RXdata[2]);	//TH>>8
						EEPROMwrite(0x03,RXdata[3]);	//LIGHT_TH,0XFE->0XFF
						EEPROMwrite(0x04,RXdata[4]);	//DELAY_NUM/3
*/
						
						set_var();

					}
					else RXnum=0;
				}

			}
			else 
			{
				//send_data(RXdata[0]);
				RXnum=0;
			}
			SCON &=~ 0x01;					          //清除接收中断标志位
			EA=1;
	}
}

void UART2_Rpt(void) interrupt UART2_VECTOR
{
}

// void Delay_2us(u16 Cnt)
// {
// 	while(Cnt--);	
// }

void Flash_EraseBlock(unsigned int fui_Address)
{
	while(1)
	{
		LVDC &=~ 0x08;			  //清除LVD中断标志位
		P0_0=0;		
		if((LVDC&0x08)==0)		break;
	}
	P0_0=1;		
	EA=0;
	IAP_CMD = 0xF00F;				//Flash解锁
	IAP_ADDR = fui_Address;	//写入擦除地址
	IAP_CMD = 0xD22D;				//选择操作方式， 扇区擦除
	IAP_CMD = 0xE11E; 			//触发后IAP_ADDRL&IAP_ADDRH指向0xFF，同时自动锁定
	//EA=1;
}

/**
  * @说明  	写入一个字节数据到Flash里面
  *         该函数需绝对地址编译，详情请查阅IAP操作应用手册
  * @参数  	fui_Address ：FLASH地址
  *	@参数	  fucp_SaveData：写入的数据
  * @返回值 无
  * @注		  写之前必须先对操作的扇区进行擦除
  */
void FLASH_WriteData(unsigned char fuc_SaveData, unsigned int fui_Address)
{
	while(1)
	{
		LVDC &=~ 0x08;			   //清除LVD中断标志位
		P0_0=0;		
		if((LVDC&0x08)==0)		break;
	}
	P0_0=1;	
	EA=0;
	IAP_DATA = fuc_SaveData;
	IAP_CMD=0xF00F;				   //Flash解锁
	IAP_ADDR = fui_Address;
	IAP_CMD=0xB44B;				   //字节编程
	IAP_CMD=0xE11E;				   //触发一次操作
	//EA=1;
}

/**
  * @说明  	写入任意长度的数据到FLASH里面
  *         该函数需绝对地址编译，详情请查阅IAP操作应用手册
  * @参数  	fui_Address ：FLASH起始地址
  *	@参数	  fuc_Length ： 写入数据长度
  *			    取值范围：0x00-0xFF
  *	@参数  *fucp_SaveArr：写入的数据存放区域的首地址
  * @返回值 无
  * @注		  写之前必须先对操作的扇区进行擦除
  */

// void Flash_WriteArr(unsigned int fui_Address,unsigned char fuc_Length,unsigned char *fucp_SaveArr)
// {
// 	unsigned char fui_i = 0;
// 	EA=0;
// 	for(fui_i=0;fui_i<fuc_Length;fui_i++)
// 	{
// 		FLASH_WriteData(*(fucp_SaveArr++), fui_Address++); 
// 	}
// 	EA=1;
// }

/**
  * @说明  	从FLASH里面读取任意长度的数据
  * @参数  	fui_Address ：FLASH起始地址
  *	@参数	  fuc_Length ：读取数据长度
  *			    取值范围：0x00-0xFF
  *	@参数	 *fucp_SaveArr：读取数据存放的区域首地址
  * @返回值 无
  * @注		  无
  */
