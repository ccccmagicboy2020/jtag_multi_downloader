#include<main.h>

/*
extern uint8_t  Time2_500ms_OK;
extern uint8_t   Leds_Light_TimeCnt;
extern uint8_t   PWM_Switch_State;
extern uint8_t  Key_UpD_Flg;
extern u8 PWM_val,PWM_UP;
extern u8 S6_recnum,Send_data;
//extern u8 USB_Send[30];
extern u8 USART1_recdata[20];
extern u8 beep_st;


extern u8 Pulse_num,Pulse_mode;
*/


/**************************************************************/
void Timer6_Configuration(void)
{
  TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
  NVIC_InitTypeDef          NVIC_InitStructure;
  //------------------------------------------------------------
  //打开定时器的时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  //------------------------------------------------------------
  //TIMx寄存器重设为缺省值
  TIM_DeInit(TIM6);
  TIM_TimeBaseStructure.TIM_Period=499;      //自动重装载寄存器周期的值
  TIM_TimeBaseStructure.TIM_Prescaler=23999;  //TIMx 时钟频率除数的预分频值1mms
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM6, DISABLE);//使能ARR预装载缓冲器
  TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE); 
  TIM_Cmd(TIM6, ENABLE);	//开启时钟 
  //------------------------------------------------------------
  //配置TIM6中断
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel =TIM6_DAC_IRQn;
  //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/***************************************************************/
void Timer7_Configuration(void)
{
  TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
  NVIC_InitTypeDef          NVIC_InitStructure;
  //------------------------------------------------------------
  //打开定时器的时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
  //------------------------------------------------------------
  TIM_DeInit(TIM7);
  TIM_TimeBaseStructure.TIM_Period=100;		 //自动重装载寄存器周期的值
  TIM_TimeBaseStructure.TIM_Prescaler=23999;  //TIMx 时钟频率除数的预分频值1mms
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM7, DISABLE);//使能ARR预装载缓冲器
  TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE); 
  TIM_Cmd(TIM7, ENABLE);	//开启时钟 
  //------------------------------------------------------------
  //配置TIM7中断
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel =TIM7_IRQn;
  //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/*************************************************************************/
void Timer2_Configuration(void)
{
	/*
  TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
  NVIC_InitTypeDef          NVIC_InitStructure;
  //------------------------------------------------------------
  //打开定时器的时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  //------------------------------------------------------------
  TIM_DeInit(TIM2);
  TIM_TimeBaseStructure.TIM_Period=500;		 //自动重装载寄存器周期的值
  TIM_TimeBaseStructure.TIM_Prescaler=23999;  //TIMx 时钟频率除数的预分频值1mms
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM2, ENABLE);//使能ARR预装载缓冲器
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); 
  TIM_Cmd(TIM2, ENABLE);	//开启时钟 
  //------------------------------------------------------------
  //配置TIM2中断
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel =TIM2_IRQn;
  //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	*/
	
	    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
			NVIC_InitTypeDef          NVIC_InitStructure;

			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
			TIM_DeInit(TIM2);
	
     TIM_TimeBaseStructure.TIM_Period =19;  //  10us*20->5kHz
     TIM_TimeBaseStructure.TIM_Prescaler = 239; //24MHZ/(239+1) =100Khz      
     TIM_TimeBaseStructure.TIM_ClockDivision = 0;   //24MHZ 
     TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //Mode;
     TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
     TIM_Cmd(TIM2, ENABLE);

		 
     NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;  
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);		 
		 
     TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);
     TIM_ClearITPendingBit(TIM2, TIM_IT_CC1 );			 
}

/*************************************************************************/
void Timer3_Configuration(void)
{

  TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
  NVIC_InitTypeDef          NVIC_InitStructure;
  //------------------------------------------------------------
  //打开定时器的时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  //------------------------------------------------------------
  TIM_DeInit(TIM3);
  TIM_TimeBaseStructure.TIM_Period=200;		 //自动重装载寄存器周期的值
  TIM_TimeBaseStructure.TIM_Prescaler=23999;        //TIMx 时钟频率除数的预分频值1mms
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM3, DISABLE);//使能ARR预装载缓冲器
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); 
  TIM_Cmd(TIM3, DISABLE);	//开启时钟 
  //------------------------------------------------------------
  //配置TIM3中断
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel =TIM3_IRQn;
 // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =6;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void Timer4_Configuration(void)
{
  TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
  NVIC_InitTypeDef          NVIC_InitStructure;
  //------------------------------------------------------------
  //打开定时器的时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  //------------------------------------------------------------
  //TIMx寄存器重设为缺省值
  TIM_DeInit(TIM4);
  TIM_TimeBaseStructure.TIM_Period=65;      //自动重装载寄存器周期的值
  TIM_TimeBaseStructure.TIM_Prescaler=7999;  //TIMx 时钟频率除数的预分频值1mms
 // TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
//  TIM_ARRPreloadConfig(TIM6, ENABLE);//使能ARR预装载缓冲器
//  TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE); 
//  TIM_Cmd(TIM6, ENABLE);	//开启时钟 
	
	   TIM_TimeBaseStructure.TIM_ClockDivision = 0;   //48MHZ 
     TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //Mode;
     TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
     TIM_Cmd(TIM4, ENABLE);
    // TIM_ITConfig(TIM4,TIM_IT_CC1,ENABLE);
     TIM_ClearITPendingBit(TIM4, TIM_IT_CC1 );	
	
  //------------------------------------------------------------
  //配置TIM6中断
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel =TIM4_IRQn;//TIM6_DAC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =4;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


/*************************************************************************/
void TIM6_DAC_IRQHandler(void)
{
 
   if(TIM_GetITStatus(TIM6,TIM_IT_Update)!=RESET)
   {
      WDI();
      //清除中断标志
      TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
  //    SEC_CYCLE_MARK =1;
  //    AVB_timeout_counter++;
		  
   }
}
/************************************************************************/

void TIM7_IRQHandler(void)
{
 
   if(TIM_GetITStatus(TIM7,TIM_IT_Update)!=RESET)
   {
      WDI();
      //清除中断标志
      TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
//      AVB_States_WatchDog();
   }
}
/*************************************************************************/
void TIM2_IRQHandler(void)	//控制无源蜂鸣器	//2.5kHz
{
 

     if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
     {
        // for LED test
			 /*
        if (GPIOA->ODR & 0x04 ) 
	    GPIOA->ODR &= (~0x04);
        else
	    GPIOA->ODR |= 0x04;
				*/
       // if (TimeCount<0xff) TimeCount++;
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC1 );	
				
				if(beep_st==0)
				{
					BEEP_OFF;
				}
				else
				{
					if((beep_st%2)==1)
					{
						BEEP_ON;
					}
					else
					{
						BEEP_OFF;
					}
					beep_st++;
					if(beep_st>100)beep_st=1;
				}
			 
     }  
}

/*************************************************************************/

/*
void TIM3_IRQHandler(void)
{
 
   if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)
   {
      WDI();
      //清除中断标志
      TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
   //   LedBlinkControl();
  }
}


void TIM4_IRQHandler(void)
{
	if(PWM_UP==1)
	{
		TIM_SetCompare2(TIM3,PWM_val);
		PWM_val++;
		if(PWM_val>20)PWM_val++;
		if(PWM_val>60)PWM_val++;
		if(PWM_val>=120)
			PWM_UP=0;
	}
	else
	{
		TIM_SetCompare2(TIM3,PWM_val);	
		PWM_val--;
		if(PWM_val>20)PWM_val--;
		if(PWM_val>60)PWM_val--;
		
		if(PWM_val<2)
			PWM_UP=1;		
	}
	TIM_ClearITPendingBit(TIM4, TIM_IT_CC1 );	
}
*/

//

//          |  PA.06: (TIM3_CH1)  |
//          |  PA.07: (TIM3_CH2)  |
//    	|  PB.00: (TIM3_CH3)  | 
//  		|  PB.01: (TIM3_CH4)  |

/**********************************************************************************/


/*
 * 函数名：TIM3_Mode_Config
 * 描述  ：配置TIM3输出的PWM信号的模式，如周期、极性、占空比
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */



 void TIM3_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	 uint16_t CCR2_Val = 0;
 //------------------------------------------------------------
  //打开定时器的时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* PWM信号电平跳变值 */
	//u16 CCR1_Val = 500;        

	//u16 CCR3_Val = 250;
	//u16 CCR4_Val = 125;

/* -----------------------------------------------------------------------
    TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles:
    TIM3CLK = 72 MHz, Prescaler = 0x0, TIM3 counter clock = 72 MHz
    TIM3 ARR Register = 999 => TIM3 Frequency = TIM3 counter clock/(ARR + 1)
    TIM3 Frequency = 72 KHz.
    TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR)* 100 = 50%
    TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR)* 100 = 37.5%
    TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR)* 100 = 25%
    TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR)* 100 = 12.5%
  ----------------------------------------------------------------------- */

  /* Time base configuration */		 
  TIM_TimeBaseStructure.TIM_Period = 199;       //当定时器从0计数到999，即为1000次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Prescaler = 1;	    //设置预分频：不预分频，即为72MHz 10就是20KHz  于2016年2月19日定为4,频率为48KHz，电感声音微弱
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//设置时钟分频系数：不分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;	   //设置跳变值，当计数器计数到这个值时，电平发生跳变
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR2_Val时为高电平

  TIM_OC2Init(TIM3, &TIM_OCInitStructure);	 //使能通道2

  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);


  TIM_ARRPreloadConfig(TIM3, ENABLE);			 // 使能TIM3重载寄存器ARR

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);                   //使能定时器3	
}


//PWM
/*
void PWM_Out_Off(void)
{
  
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_Cmd(TIM3, DISABLE);                   //使能定时器3	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);				 //设置PA7为复用输出�
  PA7OUT=0x00;
  MOS_B_CTRL=0x00;
}
*/

/*
void PWM_Out_On(void)
{
  
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);				 //设置PA7为复用输出�
  TIM_Cmd(TIM3, ENABLE);                   //使能定时器3	
  MOS_B_CTRL=0x01;
}
*/

/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/

