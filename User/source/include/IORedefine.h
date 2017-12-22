/********************************************************************************/
/* This I/O Redefine header file is just suitable for the PCB of Version 2.0 !  
/* date:2015.2.4
/* author:ZhuMX

/********************************************************************************/
#ifndef _IORedefine_H_
#define _IORedefine_H_

/*LCD板*/
#define     HT1621B_CS          P00 //HT1621B片选端
#define     HT1621B_WR          P01 //HT1621B写数据端
#define     HT1621B_DATA        P02 //HT1621B数据端
#define     LCD_RED             P03 //LCD红灯 
#define     LCD_GREEN           P04 //LCD绿灯 
#define     LCD_YELLOW          P34 //LCD黄灯
#define     IR_RCV              P25 //红外接收 CCP0_3

/*主控板按键*/
#define     LED_LSG             P07 //Lose signal LED#define     OPENING_0_DEF       P54 //0%标定
#define     OPENING_100_DEF     P40 //100%标定
#define     SET_SEN             P55 // 设置/灵敏度

/*拨码器*/
#define     PIN_LOSE_OPEN           P45 // 丢信开
#define     PIN_LOSE_CLOSE          P46 // 丢信关
#define     POS_REV             	P23 //正，反作用
#define     PIN_ALARM               P24 //电机10s保护
#define     ALARM_ON            0
#define     ALARM_OFF           1
/*现场信号*/
#define     LOCAL               P20 // 现场信号
#define     LOCAL_STOP          P21 // 现场停止
#define     LOCAL_CLOSE         P37 // 现场关信号
#define     LOCAL_OPEN          P36 // 现场开信号
/*远方控制信号*/
#define     PIN_DISTANT_STOP        P22   //远方保持
#define     PIN_DISTANT_OPEN        P26  //CCP1_3 远方开
#define     PIN_DISTANT_CLOSE       P27  //CCP2_3 远方关

/*行程力矩限位信号*/                               
#define     PIN_TORQUE_OPEN     P44 // 力矩限位       
#define     PIN_LIM_OPN         P43 // 行程限位 
#define     PIN_LIM_CLE         P42 // 行程限位 
#define     PIN_TORQUE_CLOSE    P41 // 力矩限位 
#define     PIN_FLAG_LIMOPN     P16
#define     PIN_FLAG_LIMCLE     P17

#define     PIN_FLAG_LIMCLE_ON  {PIN_FLAG_LIMCLE = 0;PIN_FLAG_LIMOPN = !PIN_FLAG_LIMCLE;}
#define     PIN_FLAG_LIMOPN_ON  {PIN_FLAG_LIMOPN = 0;PIN_FLAG_LIMCLE = !PIN_FLAG_LIMOPN;}
#define     PIN_FLAG_LIMCLE_OFF {PIN_FLAG_LIMCLE = 1;}
#define     PIN_FLAG_LIMOPN_OFF {PIN_FLAG_LIMOPN = 1;}

/*相序检测捕获信号*/
#define     U_CAP               P32 // INT0
#define     V_CAP               P33 // INT1

/*电机正反转控制信号*/
#define     RELAY_A              P05  // 继电器A
#define     RELAY_B              P06  // 继电器B

/*报错信号*/
#define     ERR                 P47
#define     ERR_OUT             {if(ERR != 0) ERR = 0;}
#define     ERR_OFF             {if(ERR != 1) ERR = 1;}

#define     P_PWM              P35 //为PWM

                               //P10,Input Current  Vi   ADC0    
                               //P11,Actual Opening Va   ADC1
                               //P12,Vref_2.5V      Vref ADC2
                               //P30，P31为TXD,RXD


#endif

///**************************************************************************/
///* This I/O Redefine header file is just suitable for the PCB of Version 1.0 !  
///* date:
///* author:ZhuMX

///**************************************************************************/

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



