/************************************************************************************
文件名 ：LCD.h
作者   ：朱明勋
说明   ：LCD显示函数声明，用来显示屏幕上的数据

*************************************************************************************/



#ifndef _LCD_H
#define _LCD_H

#define     LCD_RED_ON          {if(LCD_RED != 1) LCD_RED = 1;}          
#define     LCD_RED_OFF         {if(LCD_RED != 0) LCD_RED = 0;}         
#define     LCD_GREEN_ON        {if(LCD_GREEN != 1) LCD_GREEN = 1;}            
#define     LCD_GREEN_OFF       {if(LCD_GREEN != 0) LCD_GREEN = 0;}    
#define     LCD_YELLOW_ON       {if(LCD_YELLOW != 0) LCD_YELLOW = 0;}    
#define     LCD_YELLOW_OFF      {if(LCD_YELLOW != 1) LCD_YELLOW = 1;}



//void Bit_DIS(u8 line_num, u8 bit_num, u8 num);
void LCD_Init();
void RealTime_Opening_DIS(float float_data);
void LCD_FullDisplay(); 
void LCD_ClearDisplay(); 
void Def0_DIS();
void Def100_DIS();
void InputCurrent_DIS(float f_data);
void Voltage_DIS(float f_data);
void Local_DIS();
void OutputCurrent_DIS(float f_data);
void Sen_DIS(u16 sen_data);
void LCD_LED_DIS();
void LCD_TEST();
void LCD_DIS();
void LCD_SetDIS();
void LCD_TorqueDIS_500ms();
void LED_MotorDIS_500ms();
void LO_Alarm_DIS();
void LC_Alarm_DIS();
void EP_Alarm_DIS();
void LCD_MotorErrDIS();
void Clear_Alarm_DIS();

extern u8  ep_discnt;
extern u8  lo_discnt;
extern u8  lc_discnt;
extern u8  lcd_redcnt;
extern u8  lcd_greencnt;
extern u8  ep_dis;
extern u8  lo_dis;
extern u8  lc_dis;
extern u8  current_out_flag;

#endif