#include <stdio.h>
#include "config.h"
#include "USART.h"
#include "uart_debug.h"
#include "data_input.h"
#include "motor.h"
#include <math.h>
#include "control_new.h"
//#define DEBUG

/*************  串口1初始化函数 *****************/
void	UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			//使用波特率,   BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 19200ul;			//波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
	COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	COMx_InitStructure.UART_Polity    = PolityLow;			//中断优先级, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(必须使用内部时钟)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//内部短路RXD与TXD, 做中继, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//初始化串口1 USART1,USART2

}

void DebugPrintf()
{ 
    #ifdef DEBUG

    //u16 adc1 = pData_ADC->adcvalue_input;
    //float adcf1 = pData_Voltage->voltage_input;
    //float current = pData_Acquire->current_input;
    //u16 curin16 = cur_in*1000;
    //float vol = pData_Voltage->voltage_input;
    
    //printf("InputRes: %f,OutputRes: %f \r\n",pSystemParam->res_input,pSystemParam->res_output);
    //printf("use encoder : %d\r\n",use_encoder_flag);
    #endif   
    
    
}

char putchar (char c)   
{        
    ES=0;        
    SBUF = c;        
    while(TI==0);        
    TI=0;        
    ES=1;        
    return 0;
}





