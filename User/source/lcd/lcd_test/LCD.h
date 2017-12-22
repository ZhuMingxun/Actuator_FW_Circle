/************************************************************************************
文件名 ：LCD.h
作者   ：朱明勋
说明   ：LCD显示函数声明，用来显示屏幕上的数据

*************************************************************************************/



#ifndef _LCD_H
#define _LCD_H

#define     LCD_RED_ON          {LCD_RED = 1;}          
#define     LCD_RED_OFF         {LCD_RED = 0;}         
#define     LCD_GREEN_ON        {LCD_GREEN = 1;}            
#define     LCD_GREEN_OFF       {LCD_GREEN = 0;}    
#define     LCD_YELLOW_ON       {LCD_YELLOW = 1;}    
#define     LCD_YELLOW_OFF      {LCD_YELLOW = 0;}



//void Bit_DIS(u8 line_num, u8 bit_num, u8 num);
void LCD_Init();
//void YuanKong_Display();
//void XianChang_Display();
void RealTime_Opening_DIS(float float_data);
void LCD_FullDisplay(); 
void LCD_ClearDisplay(); 
void Def0_DIS();
void Def100_DIS();
void InputCurrent_DIS(float f_data);
void OutputCurrent_DIS(float f_data);
void Sen_DIS();
void LCD_LED_DIS();
void LCD_TEST();
void LCD_DIS();
void LO_Alarm_DIS();
void LC_Alarm_DIS();
void EP_Alarm_DIS();
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