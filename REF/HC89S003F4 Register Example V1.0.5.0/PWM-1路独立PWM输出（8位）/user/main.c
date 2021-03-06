/**
*   ************************************************************************************
*								上海芯圣电子股份有限公司
*								    www.holychip.cn
*	************************************************************************************
*	@Examle Version		V1.0.5.0
*	@Demo 	Version		V1.0.4.0
*	@Date				2019.12.27
*	************************************************************************************
*									模块性能介绍
*	1、MCU拥有一个单路8位PWM3模块，PWM3提供周期溢出中断
*	2、PWM3周期：[PWM3P]*PWM3工作时钟源周期
*	3、PWM3占空比：[PWM3D]*PWM3工作时钟源周期
*	************************************************************************************
*									 应用注意事项
*	1、关闭PWM3模块时，PWM计数停止，输出立即关闭。再打开PWM3模块时，PWM计数器都重新从1开
*	   始计数，输出受PWM3OEN控制
*	2、PWM3允许输出，必须在PWM3EN置1下才有效，否则为PWM3输出关闭状态（输出时对应端口必须
*	   设为输出模式）；即使都禁止输出，只要相关位被使能，PWM3都可以溢出中断，即此时PWM3
*	   可以作为定时器/计数器使用，此控制位修改立即生效
*	************************************************************************************
*  								       客户服务
*	感谢您使用我们的单片机，若发现错误或对函数的使用存在疑问。请添加上海芯圣电子官方QQ群
*	****************************技术支持群：201030494***********************************
* ************************************************************************************
**/

#define ALLOCATE_EXTERN
#include "HC89S003F4.h"

/***************************************************************************************
  * @实现效果	P22口输出频率为15.69KHz，占空比为34%的方波
***************************************************************************************/
void main()
{
/************************************系统初始化****************************************/
	WDTCCR = 0x00;						//关闭看门狗
		                        //本例程为方便测试关闭看门狗，实际使用中，建议客户打开看门狗，详见WDT复位例程
	CLKSWR = 0x51;						//选择内部高频RC为系统时钟，内部高频RC 2分频，Fosc=16MHz
	CLKDIV = 0x01;						//Fosc 1分频得到Fcpu，Fcpu=16MHz 
/************************************PWM3初始化****************************************/
	P2M1 = P2M1&0xF0|0x08;		//P22设置为推挽输出
	PWM3_MAP = 0x22;					//PWM3映射P22口
	//周期计算 	= 0xFF / (Fosc / PWM分频系数)		（Fosc见系统时钟配置的部分）
	//			= 0xFF /(16000000 / 4)			
	// 			= 255 /4000000
	//			= 63.75us		即15.69KHZ		

	PWM3P = 0xFF;						  //PWM周期为0xFF
	//有效电平时间计算（即占空比） 	
	//			= 0x55 / (Fosc / PWM分频系数)		（Fosc见系统时钟配置的部分）
	//			= 0x55 /(16000000 / 4)			
	// 			= 85 /4000000
	//			= 21.25us		占空比为 21.25 / 63.75 = 34%

	PWM3D = 0x55;						  //PWM占空比设置
	PWM3C = 0x92; 						//使能PWM3，关闭中断，允许输出，时钟4分频
	
  while(1);
}