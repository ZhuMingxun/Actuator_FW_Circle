#include "mlx90316.h"

#define __SPIF  ((u8)(1<<7))
#define __WCOL  ((u8)(1<<6))
#define __SS    P07
#define __SS_L  {__SS = 0;}
#define __SS_H  {__SS = 1;}

void MLX90316_SPI_Config()
{
	SPI_InitTypeDef		SPI_InitStructure;
	SPI_InitStructure.SPI_Module    = ENABLE;              //SPI启动    ENABLE, DISABLE
	SPI_InitStructure.SPI_SSIG      = DISABLE;			  //片选位     ENABLE, DISABLE
	SPI_InitStructure.SPI_FirstBit  = SPI_MSB;			  //移位方向   SPI_MSB, SPI_LSB
	SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;	  //主从选择   SPI_Mode_Master, SPI_Mode_Slave
	SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low;      //时钟相位   SPI_CPOL_High,   SPI_CPOL_Low
	SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge;	  //数据边沿   SPI_CPHA_1Edge,  SPI_CPHA_2Edge
	SPI_InitStructure.SPI_Interrupt = DISABLE;			  //中断允许   ENABLE,DISABLE
	SPI_InitStructure.SPI_Speed     = SPI_Speed_64;		  //SPI速度    SPI_Speed_4, SPI_Speed_16, SPI_Speed_64, SPI_Speed_128
	SPI_InitStructure.SPI_IoUse     = SPI_P12_P13_P14_P15; //IO口切换   SPI_P12_P13_P14_P15, SPI_P24_P23_P22_P21, SPI_P54_P40_P41_P43
	SPI_Init(&SPI_InitStructure);
     
}

u8 SPI_WtiteReadByte(u8 dat)
{ 
    SPDAT = dat;
    while(!(SPSTAT&__SPIF));
    SPSTAT = __SPIF|__WCOL;//清除SPSTAT标志
    
    return SPDAT;     
}


u16  data angle_adc = 0;
float data angle = 0.0;
#define __DELAY_BYTE    {delay_us(40);}
u16 MLX90316_ReadData()
{
    u8 startwordl;
    u8 startwordh;
    u16 startword;
    u8 framel;
    u8 frameh;
    u16 frame;
    u8 inverted_framel;
    u8 inverted_frameh;
    u16 inverted_frame;
    
    __SS_H
    delay_ms(3);
    __SS_L
    delay_us(10);
    
    startwordh = SPI_WtiteReadByte(0x55);__DELAY_BYTE
    startwordl = SPI_WtiteReadByte(0x00);__DELAY_BYTE
    
    frameh = SPI_WtiteReadByte(0x00);__DELAY_BYTE
    framel = SPI_WtiteReadByte(0x00);__DELAY_BYTE
    inverted_frameh = SPI_WtiteReadByte(0x00);__DELAY_BYTE
    inverted_framel = SPI_WtiteReadByte(0x00);__DELAY_BYTE
    __SS_H
    
    startword = ((u16)startwordh<<8 & 0xFF00) | ((u16)startwordl & 0x00FF);//获取起始字节
    frame = ((u16)frameh<<8 & 0xFF00) | ((u16)framel & 0x00FF);//获取数据帧
    inverted_frame = ((u16)inverted_frameh<<8 & 0xFF00) | ((u16)inverted_framel & 0x00FF);//数据帧反码

    if( (startword==0xAAFF) && ((frame^inverted_frame)==0xFFFF) && ((frame&0x0003)==0x0001) )
    {
        angle_adc = frame>>2;
        angle = (float)angle_adc/((u16)1<<14) *359.9999;
        //printf("%f\r\n",angle);
    }
    else
    {
        angle_adc = 0xC000;
        //printf("\r\n Data ERROR \r\n");
    }
    return angle_adc;

    
     
}

//void UART1_Config(void)
//{
//	COMx_InitDefine		COMx_InitStructure;					//结构定义
//	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
//	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//使用波特率,   BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
//	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
//	COMx_InitStructure.UART_RxEnable  = DISABLE;				//接收允许,   ENABLE或DISABLE
//	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
//	COMx_InitStructure.UART_Interrupt = DISABLE;				//中断允许,   ENABLE或DISABLE
//	COMx_InitStructure.UART_Polity    = PolityHigh;			//中断优先级, PolityLow,PolityHigh
//	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(必须使用内部时钟)
//	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//内部短路RXD与TXD, 做中继, ENABLE,DISABLE
//	USART_Configuration(USART1, &COMx_InitStructure);		//初始化串口1 USART1,USART2
//	EA = 1;
//}

//char putchar (char c)   
//{        
//    ES=0;        
//    SBUF = c;        
//    while(TI==0);        
//    TI=0;        
//    ES=1;        
//    return 0;
//}





