/**************************************************************************/
/* This I/O Redefine header file is suitable for the PCB of Version2.0  
/* date:2015.2.4
/* author:ZhuMX

/**************************************************************************/
#ifndef _IORedefine_H_
#define _IORedefine_H_

/*****LCD板****/
#define     HT1621B_CS          P00   //HT1621B片选端
#define     HT1621B_WR          P01   //HT1621B写数据端
#define     HT1621B_DATA        P02 //HT1621B数据端
#define     LCD_RED             P03  //LCD红灯
#define     LCD_GREEN           P04 //LCD绿灯
#define     LCD_YELLOW          P34 //LCD黄灯
#define     IR_RCV              P25 //红外接收 CCP0_3

/*主控板按键*/
#define     LED_LSG             P07  //Lose signal LED#define     DEF_BOTTOM          P54 //0%标定
#define     DEF_TOP             P40 //100%标定
#define     SET_SEN             P55 // 设置/灵敏度

/*拨码器*/
#define     LOSE_OPEN           P45 // 丢信开
#define     LOSE_CLOSE          P46 // 丢信关
#define     POS_NEG             P23 //正，反作用
#define     ALARM               P24 //电机10s保护

/*现场信号*/
#define     LOCAL               P20 // 现场信号
#define     LOCAL_STOP          P21 // 现场停止
#define     LOCAL_CLOSE         P37 // 现场关信号
#define     LOCAL_OPEN          P36 // 现场开信号
/*远方控制信号*/
#define     DISTANT_KEEP        P22   //远方保持
#define     DISTANT_OPEN        P26  //CCP1_3 远方开
#define     DISTANT_CLOSE       P27  //CCP2_3 远方关

/*行程力矩限位信号*/                               
#define     UP_MOM              P44 // 力矩限位       
#define     UP_LIM              P43 // 行程限位 
#define     DOWN_LIM            P42 // 行程限位 
#define     DOWN_MOM            P41 // 力矩限位 


/*相序检测捕获信号*/
#define     U_CAP               P32 // INT0
#define     V_CAP               P33 // INT1

/*电机正反转控制*/
#define     M_OPEN              P05  // 电机正转
#define     M_CLOSE             P06  // 电机反转

/*报错输出*/
#define     ERR                 P47

                               //P10,Input Current  Vi   ADC0    
                               //P11,Actual Opening Va   ADC1
                               //P12,Vref_2.5V      Vref ADC2
                               //P30，P31为TXD,RXD
                               //P35 为PWM

#endif

//#ifndef _IORedefine_H_
//#define _IORedefine_H_


//#define     LCD_GREEN      P00 //LCD绿灯
//#define     LCD_RED        P01 //LCD红灯
//#define     HT1621B_DATA   P02 //HT1621B数据端
//#define     HT1621B_WR     P03 //HT1621B写数据端
//#define     HT1621B_CS     P04 //HT1621B片选端
//#define     OPEN           P05  // 继电器开
//#define     CLOSE          P06  // 继电器关
//#define     LSG_LED        P07  //丢信

//                               //P10,P11,P12为ADC采样输入
//#define     LOSE_O         P13 // 丢信开
//#define     LOSE_C         P14 // 丢信关
//#define     ZZY            P15 //正作用
//#define     SET0           P45 //0%标定
//#define     SET100         P24 //100%标定

//#define     LMD            P47 // 灵敏度
//#define     ALARM          P23
//#define     LOC            P20 // 现场信号
//#define     STOP           P21 // 现场保持信号
//#define     CLE            P37 // 现场关信号
//#define     OPN            P36 // 现场开信号
//#define     ERR            P22

//                                //P30，P31为TXD,RXD
//                                //P35 为PWM
//#define     shangB         P44 // 力矩限位 开INT3
//#define     shangA         P43 // 行程限位 开INT2
//#define     xiaA           P42 // 行程限位 关INT1
//#define     xiaB           P41 // 力矩限位 关INT0

//#endif


