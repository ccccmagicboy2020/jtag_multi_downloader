/**
*   ************************************************************************************
*								上海芯圣电子股份有限公司
*								    www.holychip.cn
*	************************************************************************************
*	@Examle Version		V1.0.5.0
*	@Demo 	Version		V1.0.4.0
*	@Date				2019.12.27
*	************************************************************************************
*									 模块性能介绍
*	1、MCU集成了3个12位PWM模块PWM0、PWM1、PWM2，每个模块各有一个计数器，三个PWM模块的功
*    能以及操作完全一样
*	2、提供每个PWM周期溢出中断，但中断共用同一向量入口
*	3、提供出错帧测功能可紧急关闭PWM输出
*	************************************************************************************
*									 应用注意事项
*	1、互补输出模式及独立输出模式都可受故障检测脚控制
*	2、修改PWMx工作模式时建议先关闭PWMx模块（x=0、1、2）。
*	3、关闭时，PWMx计数停止，输出立即关闭。打开时，PWMx计数器都重新从1开始计数，输出受
*	   PWMx_OEN和PWMx1_OEN控制（x=0、1、2）。
*	4、PWMx允许输出，必须在PWMx_EN置1下才有效，否则PWMx输出关闭状态（输出时对应端口必须
*	   设为输出模式）；即使都禁止输出，只要PWMx_EN位被使能，PWMx都可以溢出中断，即此时
*	   PWMx可以作为定时器使用，此控制位修改立即生效（x=0、1、2）。
*	5、对于独立模式，输出模式选择位同样有效，但与互补模式不同的是有效期间为占空比期间；
*	   而互补模式中对于PWMx的有效期间为占空比期间，PWMx1的有效期间为占空比的互补期间
*   （x=0、1、2）。
*	************************************************************************************
*  								       客户服务
*	感谢您使用我们的单片机，若发现错误或对函数的使用存在疑问。请添加上海芯圣电子官方QQ群
*	****************************技术支持群：201030494***********************************
*   ************************************************************************************
**/

#define ALLOCATE_EXTERN
#include "HC89S003F4.h"

/***************************************************************************************
  * @实现效果	将PWM0设置为定时器模式，每过32.76Ms进入一次中断，P00电平翻转
***************************************************************************************/
void main()
{
/************************************系统初始化****************************************/
	WDTCCR = 0x00;						//关闭看门狗
		                        //本例程为方便测试关闭看门狗，实际使用中，建议客户打开看门狗，详见WDT复位例程
	CLKSWR = 0x51;						//选择内部高频RC为系统时钟，内部高频RC 2分频，Fosc=16MHz
	CLKDIV = 0x01;						//Fosc 1分频得到Fcpu，Fcpu=16MHz 
/**********************************相关配置初始化**************************************/
	P0M0 = P0M0&0xF0|0x08;		//P00设置为推挽输出
/*************************************PWM初始化****************************************/
	PWM0C = 0x83;						  //PWM时钟128分频，打开中断
    
	//周期计数完成后，会产生PWM中断，通过设置周期寄存器实现精确定时功能
	//周期计算 	= 0x0FFF / (Fosc / PWM分频系数)		（Fosc见系统时钟配置的部分）
	//			= 0x0FFF /(16000000 / 128)			
	// 			= 4095 	/125000
	//			= 32.76Ms	 (即定时的时间)
    
	PWM0PH = 0x0F;						//设置周期寄存器高4位为0x0F
	PWM0PL = 0xFF;						//设置周期寄存器低8位为0xFF
	PWM0EN = 0x0F;						//使能模块

	EA = 1;								    //打开总中断
	while(1);
}

/***************************************************************************************
  * @说明  	PWM中断服务函数
***************************************************************************************/
void PWM_Rpt(void) interrupt PWM_VECTOR
{
	P0_0 =~ P0_0;						  //P00翻转
	PWM0C &=~ 0x40;						//清除中断标志位
}