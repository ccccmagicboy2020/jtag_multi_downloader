/** $Id$ $DateTime$ 
*  @file UART.c 
*  @brief This file implements all IPCs in whole system. 
*  @version 0.0.1 
*  @since 0.0.1 
*  @author tianzhizhong<tzz@bstar.com.cn> 
*  @date 2007-05-18    Created it 
*  @author tianzhizhong<tzz@bstar.com.cn> 
*  @date 2007-05-16    do some modifications 
*  @author luoyuanhao<luoyh@bstar.com.cn> 
*  @date 2009-05-16    modify for dvx 
*/
/******************************************************************************
*@note    Copyright 2007, BeiJing Bluestar Corporation, Limited
                     ALL RIGHTS RESERVED 
Permission is hereby granted to licensees of BeiJing Bluestar, Inc. products 
to use or abstract this computer program for the sole purpose of implementing 
a product based on BeiJing Bluestar, Inc. products. No other rights to 
reproduce, use, or disseminate this computer program,whether in part or  in 
whole, are granted. BeiJing Bluestar, Inc. makes no representation or 
warranties with respect to the performance of this computer program, and 
specifically disclaims any responsibility for any damages, special or consequential, 
connected with the use of this program. 
For details, see http://www.bstar.com.cn/
******************************************************************************/
#include "main.h"

#define UART_HEAD1 0x42         /* 串口报文头字节1----‘B’ */
#define UART_HEAD2 0x53         /* 串口报文头字节2----‘S’ */
#define UART_TAIL  0xff

#define UART_length         UART_recv_buf[0]    /* 串口接收数据长度寄存器 */
#define UART_protocol       UART_recv_buf[1]    /* 串口接收协议类型寄存器 */
#define UART_mode           UART_recv_buf[2]    /* 串口接收负载类型寄存器 */
#define UART_source         UART_recv_buf[3]    /* 串口接收发送设备寄存器 */
#define UART_via            UART_recv_buf[4]    /* 串口接收经由端口寄存器 */
#define UART_target         UART_recv_buf[5]    /* 串口接收目标设备寄存器 */
#define UART_sequence_H     UART_recv_buf[6]    /* 串口接收序列字节寄存器 */
#define UART_sequence_L     UART_recv_buf[7]    /* 串口接收序列字节寄存器 */
#define UART_data1          UART_recv_buf[8]    /* 串口接收数据1寄存器 */
#define UART_data2          UART_recv_buf[9]    /* 串口接收数据2寄存器 */
#define UART_data3          UART_recv_buf[10]   /* 串口接收数据3寄存器 */
#define UART_data4          UART_recv_buf[11]   /* 串口接收数据4寄存器 */
#define UART_data5          UART_recv_buf[12]   /* 串口接收数据5寄存器 */
#define UART_data6          UART_recv_buf[13]   /* 串口接收数据6寄存器 */
#define UART_data7          UART_recv_buf[14]   /* 串口接收数据7寄存器 */
#define UART_data8          UART_recv_buf[15]   /* 串口接收数据7寄存器 */
#define UART_data9          UART_recv_buf[16]   /* 串口接收数据7寄存器 */
#define UART_data10         UART_recv_buf[17]   /* 串口接收数据7寄存器 */
#define UART_data11         UART_recv_buf[18]   /* 串口接收数据7寄存器 */
#define UART_data12         UART_recv_buf[19]   /* 串口接收数据7寄存器 */
#define UART_data13         UART_recv_buf[20]   /* 串口接收数据7寄存器 */
#define UART_data14         UART_recv_buf[21]   /* 串口接收数据7寄存器 */
#define UART_data15         UART_recv_buf[22]   /* 串口接收数据7寄存器 */
#define UART_data16         UART_recv_buf[23]   /* 串口接收数据7寄存器 */
#define UART_data17         UART_recv_buf[24]   /* 串口接收数据7寄存器 */
#define UART_checksum       UART_recv_buf[25]   /* 串口接收校验和寄存 */

/**
*  串口报文目标 
*/
#define FP_MAIN             0x10        /* 目标：前板主控 */
#define FP_KB                0x11        /* 目标：前板按键 */
#define FP_BUZZ            0x12        /* 目标：前板蜂鸣器 */
#define FP_LED              0x13        /* 目标：前板LED */
#define FP_VFD              0x14        /* 目标：前板VFD */
#define FP_WD               0x17        /* 目标：前板看门狗 */
#define FP_RC                0x18        /* 目标：前板遥控 */
#define FP_UPDATE        0x1D 

/**
* LED 
*/
#define LED_STATE           0x14        /* LED显示：状态灯 */
#define LED_BF              0x06        /* LED显示：布防/撤防指示灯 */
#define LED_ALARM           0x07        /* LED显示：报警录像指示灯 */
#define LED_YT              0x04        /* LED显示：云台控制指示灯 */
#define LED_DJ              0x09        /* LED显示：对讲指示灯 */
#define LED_NET1             0x02        /* LED显示：网络连接指示灯 */
#define LED_NET2             0x15        /* LED显示：网络连接指示灯 */
#define LED_HD              0x03        /* LED显示：磁盘工作指示灯 */
#define LED_UART            0x05        /* LED显示：串口工作指示灯 */
#define LED_RC              0x08        /* LED显示：遥控工作指示灯 */
#define LED_CHANNELS        0x1c        /* LED显示：通道状态指示灯 */
#define LED_ALL             0xFF        /* LED显示：全部工作指示灯 */
#define LED_RECORD     0x1c
#define LED_RECORDEXT     0x2c
uint8_t UART_p_data1 = 0;                 /* 串口处理数据1寄存器 */
uint8_t UART_p_data2 = 0;                 /* 串口处理数据2寄存器 */
uint8_t UART_p_data3 = 0;                 /* 串口处理数据3寄存器 */

/**
* 定义串口发送缓冲区 
*/
uint16_t UART_send_buf[UART_send_buf_length] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
/**
* 定义串口接收缓冲区  
*/
uint16_t UART_recv_buf[UART_recv_buf_length] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 

uint16_t R_sequence = 0;          /* 接收串口信息序列号 */
uint16_t T_sequence = 0;          /* 发送串口信息序列号 */

uint8_t UART_state = 0;           /* 串口接收状态寄存器 */
uint8_t UART_data_end = 0;        /* 串口接收接收数据结束寄存器 */
uint8_t i_delay = 0;              /* 关机和复位延时计时器 */
uint8_t TI_end = 0;                 /* 串口发送完毕标志位 */
uint8_t heartbeat_time = 240;     /* 主板心跳超时值 */	

extern uint8_t  AVB_timeout_counter; /* 主板超时重启计时寄存器 */
extern uint8_t  AVB_timeout_value;   /* 主板超时重启时间寄存器 */
extern uint8_t bit_beep;                    /* 蜂鸣器响2声标志位 */
extern uint8_t bit_sound;                   /* 蜂鸣器报警标志位 */

extern uint8_t AVB_Run;                     /* 主板运行状态标志位 */
extern uint8_t power_state;                 /* 开关机状态标志位 */
extern uint16_t close_power_time;         /* 开关机按键延时值寄存器 */
extern uint8_t System_Model;              /* 前板跳线组合码寄存器 */

uint8_t FP_MAIN_state = 0;                /* 前板主控状态 */
uint8_t POWER_OFF_EN = 0;                   /* 立即关机标志位 */
uint16_t i_SBUF = 0;                       /* 串口接收寄存器 */
uint8_t UART_checksum_calc = 0;           /* 串口接收校验和寄存器 */

extern u8 LedReflashBuf;
extern u8 LedReflashBuf_Old;
extern u8 Led_Status_Blink_Flag;
extern u8 Led_Record_Blink_Flag;
extern u8 Led_Hd_Blink_Flag;
extern u8 Led_Bf_Blink_Flag;
extern u8 Led_Alarm_Blink_Flag;
extern u8 Led_Rc_Blink_Flag;
extern u8 Led_Net1_Blink_Flag;
extern u8 Led_Net2_Blink_Flag;

extern uint8_t FP_Update_Flag;

/** 
* @fn UART_recv_buf_CLR 
* @brief  UART_recv_buf_CLR 
* @param[in] none. 
* @param[out] none. 
* @return none. 
*/
void UART_recv_buf_CLR(void)
{
    UART_state = 0;
    UART_data_end = 0;
    UART_length = 0;
    UART_checksum = 0;
    UART_checksum_calc = 0;
    UART_data1 = 0;
    UART_data2 = 0;
    UART_data3 = 0;
    UART_data4 = 0;
    UART_data5 = 0;
    UART_data6 = 0;
    UART_data7 = 0;
    UART_data8 = 0;
    UART_data9 = 0;
    UART_data10= 0;
    UART_data11= 0;
    UART_data12= 0;
    UART_data13= 0;
    UART_data14= 0;
    UART_data15= 0;
    UART_data16= 0;
    UART_data17=0;
}

/** 
* @fn UART2_Init 
* @brief  UART2_Init 
* @param[in] none. 
* @param[out] none. 
* @return none. 
*/
void USART2_Init(void)
{

   USART_InitTypeDef USART_InitStructure;
   NVIC_InitTypeDef NVIC_InitStructure; 
   GPIO_InitTypeDef GPIO_InitStructure;
   
   //清零串口缓存
   UART_recv_buf_CLR();
   
   //使能串口的RCC时钟
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

   //串口使用的GPIO口配置
   /* Configure USART1 Rx (PA.03) as input floating */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   /* Configure USART1 Tx (PA.02) as alternate function push-pull */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   //配置串口
   USART_InitStructure.USART_BaudRate = 19200;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

   /* Configure USART2*/
   USART_Init(USART2, &USART_InitStructure);//配置串口2

   /* Enable USART2 Receive interrupts 使能串口接收中断*/
   USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
   //串口发送中断在发送数据时开启
   //USART_ITConfig(USART2, USART_IT_TC, ENABLE);

   /* Enable the USART2 */
   USART_Cmd(USART2, ENABLE);//使能串口1

   //串口中断配置
   /* Configure the NVIC Preemption Priority Bits */  
   // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

   /* Enable the USART2 Interrupt */
   NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);

   //while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==0);
    //USART_ClearFlag(USART2, USART_FLAG_TC);
}
/** 
* @fn Send_char 
* @brief  Send_char 
* @param[in] unsigned char. 
* @param[out] none. 
* @return none. 
*/
void Usart2_send_char(uint16_t Data)
{
    USART_SendData(USART2,Data);	
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==0);
    USART_ClearFlag(USART2, USART_FLAG_TC);
}


/** 
* @fn MakeCks 
* @brief  MakeCks 
* @param[in] uint8,uint8. 
* @param[out] none. 
* @return uint8. 
*/

uint16_t MakeCks(uint16_t * buf, uint8_t len)
{
    uint8_t i = 0;
    uint16_t cks = 0;
    for(i = 0; i < len; i++)
    {
        cks = cks + buf[i];
    }
    return cks;
}

/** 
* @fn UART_send 
* @brief  UART_send 
* @param[in] uint8. 
* @param[out] none. 
* @return none. 
*/

void UART2_send(uint8_t send_length)
{
    uint8_t i = 0;
    for(i = 0; i < send_length; i++)
    {
        Usart2_send_char(UART_send_buf[i]);
    }
}

/** 
* @fn UART_Send_Reset 
* @brief  UART_Send_Reset 
* @param[in] uint8,uint8. 
* @param[out] none. 
* @return none. 
*/
void UART2_Send_Reset(uint16_t state1, uint16_t state2)
{
    uint8_t i = 3;
    uint16_t cks = 0;
    T_sequence++;
    UART_send_buf[0] = 0x42;    /* 'B' */
    UART_send_buf[1] = 0x53;    /* 'S' */
    UART_send_buf[2] = 0x0a;    /* length:11 */
    UART_send_buf[3] = 0x00;    /* protocol:0,明文传输 */
    UART_send_buf[4] = 0x03;    /* mode:3,事件模式 */
    UART_send_buf[5] = 0x10;    /* source:前板主控 */
    UART_send_buf[6] = 0x00;    /* via:0 */
    UART_send_buf[7] = 0x00;    /* target:0x00,主板 */
    UART_send_buf[8] = (0x00FF&(T_sequence >> 8)); /* 序列号高字节 */
    UART_send_buf[9] = (0x00FF&T_sequence);        /* 序列号低字节 */
    UART_send_buf[10] = state1; /* 前板状态1 */
    UART_send_buf[11] = state2; /* 前板状态2 */
    for(i = 3; i <= 11; i++)
    {
        cks = cks + UART_send_buf[i];
    }
    UART_send_buf[12] = cks;  /* 校验和 */
    UART_send_buf[13] = 0xff; /* 结束符 */
    UART2_send(14);
}

/** 
* @fn UART_Send_FB 
* @brief  UART_Send_FB 
* @param[in] uint8,uint8,uint8. 
* @param[out] none. 
* @return none. 
*/
void UART2_Send_FB(uint16_t state1, uint16_t state2, uint16_t state3)
{
    uint8_t i = 3;
    uint8_t cks = 0;
    T_sequence++;
    UART_send_buf[0] = 0x42;    /* 'B' */
    UART_send_buf[1] = 0x53;    /* 'S' */
    UART_send_buf[2] = 0x0b;    /* length:11 */
    UART_send_buf[3] = 0x00;    /* protocol:0,明文传输 */
    UART_send_buf[4] = 0x03;    /* mode:3,事件模式 */
    UART_send_buf[5] = 0x10;    /* source:前板主控 */
    UART_send_buf[6] = 0x00;    /* via:0 */
    UART_send_buf[7] = 0x00;    /* target:0x00,主板 */
    UART_send_buf[8] = (0x00FF&(T_sequence >> 8)); /* 序列号高字节 */
    UART_send_buf[9] = (0x00FF&T_sequence);        /* 序列号低字节 */
    UART_send_buf[10] = state1; /* 前板状态1 */
    UART_send_buf[11] = state2; /* 前板状态2 */
    UART_send_buf[12] = state3; /* 前板状态3 */
    for(i = 3; i <= 12; i++)
    {
        cks = cks + UART_send_buf[i];
    }
    UART_send_buf[13] = cks;    /* 校验和 */
    UART_send_buf[14] = 0xff;   /* 结束符 */
    UART2_send(15);
}

/** 
* @fn UART_Send_Alarm 
* @brief  UART_Send_Alarm 
* @param[in] uint8,uint8. 
* @param[out] none. 
* @return none. 
*/
void UART2_Send_Alarm(uint16_t cmd_Fun, uint16_t cmd_State)
{
    uint8_t i = 3;
    uint8_t cks = 0;
    T_sequence++;
    UART_send_buf[0] = 0x42;    /* 'B' */
    UART_send_buf[1] = 0x53;    /* 'S' */
    UART_send_buf[2] = 0x0a;    /* length:10 */
    UART_send_buf[3] = 0x00;    /* protocol:0,明文传输 */
    UART_send_buf[4] = 0x03;    /* mode:3,事件模式 */
    UART_send_buf[5] = 0x1a;    /* source */
    UART_send_buf[6] = 0x00;    /* via:0 */
    UART_send_buf[7] = 0x00;    /* target:0x00,主板 */
    UART_send_buf[8] = (0x00FF&(T_sequence >> 8)); /* 序列号高字节 */
    UART_send_buf[9] = (0x00FF&T_sequence);        /* 序列号低字节 */
    UART_send_buf[10] = cmd_Fun;        /* 报警功能编号 */
    UART_send_buf[11] = cmd_State;      /* 报警状态 */
    for(i = 3; i <= 11; i++)
    {
        cks = cks + UART_send_buf[i];
    }
    UART_send_buf[12] = cks;    /* 校验和 */
    UART_send_buf[13] = 0xff;   /* 结束符 */
    UART2_send(14);
}

/** 
* @fn UART_Send_Key1 
* @brief  UART_Send_Key1 
* @param[in] uint8,uint8,uint8. 
* @param[out] none. 
* @return none. 
*/
void UART2_Send_Key1(uint16_t cmd_is_KB_RC, uint16_t cmd_Key_State, uint16_t cmd_Key1)
{
    uint8_t i = 3;
    uint8_t cks = 0;
    T_sequence++;
    UART_send_buf[0] = 0x42;    /* 'B' */
    UART_send_buf[1] = 0x53;    /* 'S' */
    UART_send_buf[2] = 0x0a;    /* length:10 */
    UART_send_buf[3] = 0x00;    /* protocol:0,明文传输 */
    UART_send_buf[4] = 0x03;    /* mode:3,事件模式 */
    UART_send_buf[5] = cmd_is_KB_RC;    /* source */
    UART_send_buf[6] = 0x00;    /* via:0 */
    UART_send_buf[7] = 0x00;    /* target:0x00,主板 */
    UART_send_buf[8] = (0x00FF&(T_sequence >> 8)); /* 序列号高字节 */
    UART_send_buf[9] = (0x00FF&T_sequence);        /* 序列号低字节 */
    UART_send_buf[10] = cmd_Key_State; /* 按键状态 */
    UART_send_buf[11] = cmd_Key1;      /* 按键1 */
    for(i = 3; i <= 11; i++)
    {
        cks = cks + UART_send_buf[i];
    }
    UART_send_buf[12] = cks;           /* 校验和 */
    UART_send_buf[13] = 0xff;          /* 结束符 */
    UART2_send(14);
}

/** 
* @fn UART_Send_Key2 
* @brief  UART_Send_Key2 
* @param[in] uint8,uint8,uint8. 
* @param[out] none. 
* @return none. 
*/
void UART2_Send_Key2(uint16_t cmd_is_KB_RC, uint16_t cmd_Key_State, uint16_t cmd_Key1,
   uint16_t cmd_Key2)
{
    uint8_t i = 3;
    uint8_t cks = 0;
    T_sequence++;
    UART_send_buf[0] = 0x42;    /* 'B' */
    UART_send_buf[1] = 0x53;    /* 'S' */
    UART_send_buf[2] = 0x0b;    /* length:11 */
    UART_send_buf[3] = 0x00;    /* protocol:0,明文传输 */
    UART_send_buf[4] = 0x03;    /* mode:3,事件模式 */
    UART_send_buf[5] = cmd_is_KB_RC;    /* source */
    UART_send_buf[6] = 0x00;    /* via:0 */
    UART_send_buf[7] = 0x00;    /* target:0x00,主板 */
    UART_send_buf[8] = (0x00FF&(T_sequence >> 8)); /* 序列号高字节 */
    UART_send_buf[9] = (0x00FF&T_sequence);        /* 序列号低字节 */
    UART_send_buf[10] = cmd_Key_State;  /* 按键状态 */
    UART_send_buf[11] = cmd_Key1;       /* 按键1 */
    UART_send_buf[12] = cmd_Key2;       /* 按键2 */
    for(i = 3; i <= 12; i++)
    {
        cks = cks + UART_send_buf[i];
    }
    UART_send_buf[13] = cks;            /* 校验和 */
    UART_send_buf[14] = 0xff;           /* 结束符 */
    UART2_send(15);
}


/** 
* @fn Uart_int 
* @brief  Uart_int 
* @param[in] none. 
* @param[out] none. 
* @return none. 
*/
void USART2_IRQHandler(void)
{
    WDI ();     
   	
    if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE)==1)
    {
       USART_ClearFlag(USART2, USART_FLAG_RXNE);//调试是否用写，由于读操作会清该标志
       i_SBUF = USART_ReceiveData(USART2);

		
        switch (UART_state)
        {
            case 0:
                if(UART_HEAD1 == i_SBUF)
                {
                    UART_state = 1;
                }
                else
                {
                    UART_recv_buf_CLR();
                }
                break;
            case 1:
                if(UART_HEAD2 == i_SBUF)
                {
                    UART_state = 2;
                }
                else
                {
                    UART_recv_buf_CLR();
                }
                break;
            case 2:
                if(i_SBUF < 10)
                {
                    UART_recv_buf_CLR();
                }
                else
                {             //如果串口报文数据长度大于10个字节 
                    UART_length = i_SBUF;
                    UART_length--;
                    UART_state = 3;
                }
                break;
            case 3:
                if(i_SBUF != 0)
                {
                    UART_recv_buf_CLR();
                }
                else
                {             //如果串口报文协议类型为明文协议 
                    UART_protocol = i_SBUF;
                    UART_state = 4;
                    UART_length--;
                }
                break;
            case 4:
                  if((i_SBUF != 3)&&(i_SBUF != 2)&&(i_SBUF != 1))
                {
                    UART_recv_buf_CLR();
                }
                else
                {             //如果串口报文模式为事件上传模式 
                    UART_mode = i_SBUF;
                    UART_state = 5;
                    UART_length--;
                }
                break;
            case 5:
                if(i_SBUF != 0)
                {
                    UART_recv_buf_CLR();
                }
                else
                {             //如果串口报文来源于主板 
                    UART_source = i_SBUF;
                    UART_state = 6;
                    UART_length--;
                }
                break;
            case 6:
                if(i_SBUF != 0)
                {
                    UART_recv_buf_CLR();
                }
                else
                {             //如果串口报文端口为0 
                    UART_via = i_SBUF;
                    UART_state = 7;
                    UART_length--;
                }
                break;
            case 7:
                if((i_SBUF < 0x10) || (i_SBUF > 0x20))
                {
                    UART_recv_buf_CLR();
                }
                else
                { //如果串口报文目标为0x10 0x11 0x12 0x13 0x14 0x17 0x18 
                    UART_target = i_SBUF;
                    UART_state = 8;
                    UART_length--;
                }
                break;
            case 8:
                UART_sequence_H = i_SBUF;
                UART_state = 9;
                UART_length--;
                break;
            case 9:
                UART_sequence_L = i_SBUF;
                UART_state = 10;
                UART_length--;
                break;
            case 10:
                UART_data1 = i_SBUF;
                UART_state = 11;
                UART_length--;
                if(UART_length == 0)
                    UART_data_end = 1;
                break;
            case 11:
                if(UART_data_end)
                {
                    UART_checksum = i_SBUF;
                    UART_checksum_calc = MakeCks(&(UART_recv_buf[1]), 24);
                    UART_state = 28;
                }
                else
                {
                    UART_data2 = i_SBUF;
                    UART_state = 12;
                    UART_length--;
                    if(UART_length == 0)
                        UART_data_end = 1;
                }
                break;
            case 12:
                if(UART_data_end)
                {
                    UART_checksum = i_SBUF;
                    UART_checksum_calc = MakeCks(&(UART_recv_buf[1]), 24);
                    UART_state = 28;
                }
                else
                {
                    UART_data3 = i_SBUF;
                    UART_state = 13;
                    UART_length--;
                    if(UART_length == 0)
                        UART_data_end = 1;
                }
                break;
            case 13:
                if(UART_data_end)
                {
                    UART_checksum = i_SBUF;
                    UART_checksum_calc = MakeCks(&(UART_recv_buf[1]), 24);
                    UART_state = 28;
                }
                else
                {
                    UART_data4 = i_SBUF;
                    UART_state = 14;
                    UART_length--;
                    if(UART_length == 0)
                        UART_data_end = 1;
                }
                break;
            case 14:
                if(UART_data_end)
                {
                    UART_checksum = i_SBUF;
                    UART_checksum_calc = MakeCks(&(UART_recv_buf[1]), 24);
                    UART_state = 28;
                }
                else
                {
                    UART_data5 = i_SBUF;
                    UART_state = 15;
                    UART_length--;
                    if(UART_length == 0)
                        UART_data_end = 1;
                }
                break;
            case 15:
                if(UART_data_end)
                {
                    UART_checksum = i_SBUF;
                    UART_checksum_calc = MakeCks(&(UART_recv_buf[1]), 24);
                    UART_state = 28;
                }
                else
                {
                    UART_data6 = i_SBUF;
                    UART_state = 16;
                    UART_length--;
                    if(UART_length == 0)
                        UART_data_end = 1;
                }
                break;
            case 16:
                if(UART_data_end)
                {
                    UART_checksum = i_SBUF;
                    UART_checksum_calc = MakeCks(&(UART_recv_buf[1]), 24);
                    UART_state = 28;
                }
                else
                {
                    UART_data7 = i_SBUF;
                    UART_state = 17;
                    UART_length--;
                    if(UART_length == 0)
                        UART_data_end = 1;
                }
                break;
            case 17:
                if(UART_data_end)
                {
                    UART_checksum = i_SBUF;
                    UART_checksum_calc = MakeCks(&(UART_recv_buf[1]), 24);
                    UART_state = 28;
                }
                else
                {
                     UART_data8= i_SBUF;
                    UART_state = 18;
                    UART_length--;
                    if(UART_length == 0)
                        UART_data_end = 1;
                }
                break;
           case 18:
              if(UART_data_end )
              {
                     UART_checksum=i_SBUF;
                     UART_checksum_calc=MakeCks(&(UART_recv_buf[1]), 24);
                     UART_state=28;
              }
             else
	            {
		                UART_data9=i_SBUF;
                    UART_state=19;
                    UART_length--;
                    if(UART_length == 0)
                        UART_data_end = 1;
		           }
	         break;
	      case 19:
              if(UART_data_end )
              {
                     UART_checksum = i_SBUF;
                     UART_checksum_calc = MakeCks(&(UART_recv_buf[1]), 24);
                     UART_state=28;
              }
              else
              {
                      UART_data10=i_SBUF;
                      UART_state=20;
                      UART_length--;
                     if(UART_length==0) 
                          UART_data_end=1;
		 }
		 break;
           case 20:
		 if(UART_data_end)
              {
                     UART_checksum = i_SBUF;
                     UART_checksum_calc = MakeCks(&(UART_recv_buf[1]), 24);
                     UART_state=28;
              }
		 else
		 {
                     UART_data11=i_SBUF;
                     UART_state=21;
                     UART_length--;
                     if(UART_length==0) 
                          UART_data_end=1;
		 }
		 break;
	    case 21:
		 if(UART_data_end)
              {
                     UART_checksum = i_SBUF;
                     UART_checksum_calc = MakeCks(&(UART_recv_buf[1]), 24);
                     UART_state=28;
              }
		 else
		 {
                     UART_data12=i_SBUF;
                     UART_state=22;
                     UART_length--;
                     if(UART_length==0) 
                          UART_data_end=1;
		 }
		 break;
	    case 22:
		 if(UART_data_end)
              {
                     UART_checksum = i_SBUF;
                     UART_checksum_calc = MakeCks(&(UART_recv_buf[1]), 24);
                     UART_state=28;
              }
		 else
		 {
                     UART_data13=i_SBUF;
                     UART_state=23;
                     UART_length--;
                     if(UART_length==0) 
                          UART_data_end=1;
		 }
		 break;
           case 23:
		 if(UART_data_end)
              {
                     UART_checksum = i_SBUF;
                     UART_checksum_calc = MakeCks(&(UART_recv_buf[1]), 24);
                     UART_state=28;
              }
		 else
		 {
                     UART_data14=i_SBUF;
                     UART_state=24;
                     UART_length--;
                     if(UART_length==0) 
                          UART_data_end=1;
		 }
		 break;
           case 24:
		 if(UART_data_end)
              {
                     UART_checksum = i_SBUF;
                     UART_checksum_calc = MakeCks(&(UART_recv_buf[1]), 24);
                     UART_state=28;
              }
		 else
		 {
                     UART_data15=i_SBUF;
                     UART_state=25;
                     UART_length--;
                     if(UART_length==0) 
                          UART_data_end=1;
		 }
		 break;
           case 25:
		 if(UART_data_end)
              {
                     UART_checksum = i_SBUF;
                     UART_checksum_calc = MakeCks(&(UART_recv_buf[1]), 24);
                     UART_state=28;
              }
		 else
		 {
                     UART_data16=i_SBUF;
                     UART_state=26;
                     UART_length--;
                     if(UART_length==0) 
                          UART_data_end=1;
		 }
		 break;
           case 26:
		 if(UART_data_end)
              {
                     UART_checksum = i_SBUF;
                     UART_checksum_calc = MakeCks(&(UART_recv_buf[1]), 24);
                     UART_state=28;
              }
		 else
		 {
                     UART_data17=i_SBUF;
                     UART_state=27;
                     UART_length--;
                     if(UART_length==0) 
                          UART_data_end=1;
		 }
		 break;
           case 27:
              if(UART_data_end )
              {
                     UART_checksum=i_SBUF;
                     UART_checksum_calc=MakeCks(&(UART_recv_buf[1]), 24);
                     UART_state=28;
               }
              else
              {
                     UART_recv_buf_CLR();
               }
		 break;	
            case 28:
                if(UART_TAIL == i_SBUF)
                {
                
                    if(UART_checksum ==UART_checksum_calc)
                    {
                        switch (UART_target)
                        {
                            case FP_LED:               /* 主板LED数据包处理 */
                                switch (UART_data1)
                                {
                                    case LED_STATE:    /* LED显示：状态灯 */
                                        switch (UART_data2)
                                        {
                                            case 0x00:
							    LedReflashBuf=LedReflashBuf&(~Bit_LED_STATUS);
                                                   Led_Status_Blink_Flag=0;
                                                break;
                                            case 0x01:
							     LedReflashBuf=LedReflashBuf|Bit_LED_STATUS;
                                                    Led_Status_Blink_Flag=0;
                                                break;
                                            case 0x02:
							     LedReflashBuf=LedReflashBuf&(~Bit_LED_STATUS);
                                                    Led_Status_Blink_Flag=1;
                                                break;
                                            default:
                                                break;
                                        }
                                        break;
                                    case LED_RECORD: /* LED显示录像*/
                                        if(UART_data3|UART_data4)
                                        {
							LedReflashBuf=LedReflashBuf|Bit_LED_RECORD;
                                        }
	                                  else
	                                  {
                                               LedReflashBuf=LedReflashBuf&(~Bit_LED_RECORD);
                                         }
                                        break;
					   case LED_RECORDEXT: /* LED显示录像*/
                                        if(UART_data3|UART_data4|UART_data5|UART_data6)
                                        {
							LedReflashBuf=LedReflashBuf|Bit_LED_RECORD;
                                        }
	                                  else
	                                  {
                                               LedReflashBuf=LedReflashBuf&(~Bit_LED_RECORD);
                                         }
                                        break;
                                    case LED_HD: /* LED显示：报警录像指示灯 */
                                        switch (UART_data2)
                                        {
                                            case 0x00:
	                                             LedReflashBuf=LedReflashBuf&(~Bit_LED_HD);
                                                    Led_Hd_Blink_Flag=0;
                                                break;
                                            case 0x01:
                                                    Led_Hd_Blink_Flag=0;
							     LedReflashBuf=LedReflashBuf|Bit_LED_HD;
                                                break;
                                            case 0x02:
							     LedReflashBuf=LedReflashBuf&(~Bit_LED_HD);
                                                    Led_Hd_Blink_Flag=1;
                                                break;
                                            default:
                                                break;
                                        }
                                        break; 
                                    case LED_BF: /* LED显示：布撤防 */
                                        switch (UART_data2)
                                        {
                                            case 0x00:
							     LedReflashBuf=LedReflashBuf&(~Bit_LED_BF);
                                                    Led_Bf_Blink_Flag=0;
                                                break;
                                            case 0x01:
                                                    Led_Bf_Blink_Flag=0;
							     LedReflashBuf=LedReflashBuf|Bit_LED_BF;
                                                break;
                                            case 0x02:
							    LedReflashBuf=LedReflashBuf&(~Bit_LED_BF);
							     Led_Bf_Blink_Flag=1;				
                                                break;
                                            default:
                                                break;
                                        }
                                        break;
                                    case LED_ALARM: /* LED显示：报警*/
                                        switch (UART_data2)
                                        {
                                            case 0x00:
							     LedReflashBuf=LedReflashBuf&(~Bit_LED_ALARM);
                                                   Led_Alarm_Blink_Flag=0;
                                                break;
                                            case 0x01:
                                                   Led_Alarm_Blink_Flag=0;
					                  LedReflashBuf=LedReflashBuf|Bit_LED_ALARM;
                                                break;
                                            case 0x02:
	                                            LedReflashBuf=LedReflashBuf&(~Bit_LED_ALARM);
							    Led_Alarm_Blink_Flag=1;				
                                                break;
                                            default:
                                                break;
                                        }
                                        break;
                                    case LED_RC:    /* LED显示：遥控 */
                                        switch (UART_data2)
                                        {
                                            case 0x00:
							    LedReflashBuf=LedReflashBuf&(~Bit_LED_RC);					
                                                   Led_Rc_Blink_Flag=0;
                                                break;
                                            case 0x01: 
                                                   Led_Rc_Blink_Flag=0;
			                               LedReflashBuf=LedReflashBuf|Bit_LED_RC;
                                                break;      
						   case 0x02:
						           LedReflashBuf=LedReflashBuf&(~Bit_LED_RC);	
                                                   Led_Rc_Blink_Flag=1;
                                                break;
                                            default:
                                                break;
                                        }
                                        break;
		                        case LED_NET1:    /* LED显示：遥控 */
                                        switch (UART_data2)
                                        {
                                            case 0x00:
				                         LedReflashBuf=LedReflashBuf&(~Bit_LED_NET1);
                                                    Led_Net1_Blink_Flag=0;
                                                break;
                                            case 0x01: 
                                                    Led_Net1_Blink_Flag=0;
		                                       LedReflashBuf=LedReflashBuf|Bit_LED_NET1;
                                                break;      
						   case 0x02:
	                                              LedReflashBuf=LedReflashBuf&(~Bit_LED_NET1);
                                                    Led_Net1_Blink_Flag=1;
                                                break;
                                            default:
                                                break;
                                        }
                                   case LED_YT:
						switch(UART_data2)
						{
                                           case 0x00:
							VfdYT = 0;
			                           break;
						   case 0x01:
						       VfdYT = 1;
						       break;
						   default:
						   	break;
						}
                                        break;
					 case LED_NET2:    /* LED显示：遥控 */
                                        switch (UART_data2)
                                        {
                                             case 0x00:
                                                    LedReflashBuf=LedReflashBuf&(~Bit_LED_NET2);
                                                    Led_Net2_Blink_Flag=0;
                                                break;
                                            case 0x01: 
                                                    Led_Net2_Blink_Flag=0;
	                                             LedReflashBuf=LedReflashBuf|Bit_LED_NET2;
                                                break;      
					                              	   case 0x02:
			                                              LedReflashBuf=LedReflashBuf&(~Bit_LED_NET2);
                                                    Led_Net2_Blink_Flag=1;
                                                break;
                                            default:
                                                break;
                                        }
                                        break;
                                    default:
                                        break;
                                }
                                break;
                            case FP_WD: /* 主板心跳包处理 */
                                if(0x04 == UART_data1)  /* 如果超时值为4秒 */
                                {
                                    heartbeat_time = 4;
                                    AVB_timeout_value = 120;
                                }
                                else if(0x08 == UART_data1)/* 如果超时值为8秒 */
                                {
                                    heartbeat_time = 8;
                                    AVB_timeout_value = 240;
                                }
                                else
                                {
                                    heartbeat_time = 1;
                                    AVB_timeout_value = 30;
                                }
                                AVB_timeout_counter = 0;
                                AVB_Run = 1;
                                break;
                            case FP_BUZZ: /* 主板蜂鸣器数据包处理 */
                                if(0x01 == UART_data1)
                                {
                                    switch (UART_data2)
                                    {
                                        case 0x00:
                                            bit_sound = 0;
                                            break;
                                        case 0x01:
                                        case 0x02:
                                            bit_sound = 1;
                                            break;
                                        default:
                                            break;
                                    }
                                }
                                break;
                            case FP_MAIN: /* 主板状态数据包处理 */
                                switch (UART_data1)
                                {
                                    case 0x01:
                                        FP_MAIN_state = 1;
                                        UART_p_data1 = UART_data1;
                                        UART_p_data2 = UART_data2;
                                        UART_p_data3 = UART_data3;
                                        break;
                                    case 0x02:
                                        FP_MAIN_state = 2;
                                        UART_p_data1 = UART_data1;
                                        UART_p_data2 = UART_data2;
                                        UART_p_data3 = UART_data3;
                                        break;
                                    case 0x03:
                                        FP_MAIN_state = 3;
                                        UART_p_data1 = UART_data1;
                                        UART_p_data2 = UART_data2;
                                        UART_p_data3 = UART_data3;
                                        break;
                                    default:
                                        break;
                                }
                                break;
                            case FP_VFD:        /* 主板VFD数据包处理  */
                                break;
				 case FP_UPDATE:
				     FP_Update_Flag=1;
				    break;
                            default:
                                break;
                        }
                        UART_recv_buf_CLR();    /* 信息处理完毕 */
                    }
                    else
                    {
                        UART_recv_buf_CLR();  //校验码错误 
                    }
                }
                else
                {
                    UART_recv_buf_CLR();      //最后一个字节不等于0xff 
                }
                break;
            default:
                UART_recv_buf_CLR();          //收到非法字节 
                break;
        }
				
    }
		
}



/** 
* @fn UART_proc 
* @brief  UART_proc 
* @param[in] none. 
* @param[out] none. 
* @return none. 
*/

void UART_proc(void)
{
    WDI();
    switch (FP_MAIN_state)
    {
        case 1:                //主板请求前板状态 
            UART2_Send_FB(FPVersion, (0x30 | System_Model), 0xc7);
           
            // Bit7   Bit6   Bit5   Bit4   Bit3   Bit2   Bit1   Bit0
            //-------------------------PCID------------------------
            //
            //Bit7   Bit6   Bit5   Bit4   Bit3   Bit2   Bit1   Bit0 
            //SEL4   SEL3   SEL2   SEL1   0      0      0      1
            //
            //Bit7   Bit6   Bit5   Bit4   Bit3   Bit2   Bit1   Bit0 
            //BUZZ   KB     1      1      0      0      1      0     
            
						
            FP_MAIN_state = 0;
            break;
        case 2:                //主板请求前板复位 
            UART2_Send_Reset(UART_p_data1, UART_p_data2);
            for(i_delay = 0; i_delay < UART_p_data2; i_delay++)
            {
                delay1s();
            }
            BEEP=1;
	     LedAllOff();
            AVB_cool_reset();
            FP_MAIN_state = 0;
            break;
        case 3:                //主板请求前板关机 
            UART2_Send_Reset(UART_p_data1, UART_p_data2);
            for(i_delay = 0; i_delay < UART_p_data2; i_delay++)
            {
                delay1s();
            }
            POWER_OFF_EN = 1;
             __disable_irq();  
            LedAllOff();
            HC595_OE=1;
	     BEEP=1;
            AVB_Run = 0;
            bit_beep = 0;
            bit_sound = 0;
            power_state = 0;
            close_power_time = 0;
            AVB_timeout_counter = 0;
            POWER_STB = POWER_OFF;
            Single_Write(0xa0,ADDR_POWER_STATE,power_state);
            delay100ms();
            Wait_power_on();   //进入等待开机死循环 
            FP_MAIN_state = 0;
            break;
        default:
            break;
    }    
}

