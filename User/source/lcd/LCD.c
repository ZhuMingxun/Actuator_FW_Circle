/************************************************************************************
文件名 ：LCD.c
作者   ：朱明勋
说明   ：LCD显示函数，用来显示屏幕上的数据

*************************************************************************************/
#include <config.h>
#include "delay.h"
#include "GPIO.h"
#include "IORedefine.h"
#include "HT1621B.h"
#include "HT1621B_Drive.h"

#include "data_input.h"
#include "data_save.h"
#include "motor.h"
#include "phase_detect.h"
#include "current_output.h"
#include "control_new.h"
#include "LCD.h"
#include "local.h"
/************************************************************************************
                                   函数声明
************************************************************************************/

/************************************************************************************
                                   变量声明
************************************************************************************/
u8  ep_discnt;
u8  lo_discnt;
u8  lc_discnt;
u8  lcd_redcnt;
u8  lcd_greencnt;
u8  ep_dis;
u8  lo_dis;
u8  lc_dis;
u8  current_out_flag;

void LCDIO_Config()
{
    GPIO_InitTypeDef	GPIO_InitStructure;
    GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.Mode = GPIO_OUT_PP;//推挽输出
    GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);
    GPIO_InitStructure.Pin = GPIO_Pin_4;
    GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);
}
    
/************************************************************************************
函数名      ：HT1621B_Init
功能描述    ：显示屏初始化
参数        ：无
调用的函数  ：HT1621B_WriteData(u8 add,u8 dat)
               HT1621B_WriteCommand(u8 cmd)
               LCD_ClearDisplay()
               Delay1us()
               Delay50us()
返回值      ：无
说明        ：无
************************************************************************************/
void LCD_Init()
{  
    LCDIO_Config();
    
    LCD_RED_OFF        
    LCD_GREEN_OFF
    LCD_YELLOW_OFF
    HT1621B_CS_HIGH
    delay_us(1);
    HT1621B_WR_HIGH
    HT1621B_DATA_HIGH
    delay_us(50);
    HT1621B_WriteCommand(BIAS);
    HT1621B_WriteCommand(RC256);    
    HT1621B_WriteCommand(SYSTEM_ON);        
    HT1621B_WriteCommand(LCD_ON);
    LCD_ClearDisplay();
    //LCD_FullDisplay();
}

/************************************************************************************
函数名      ：LCD_FullDisplay
功能描述    ：显示屏全屏显示
参数        ：无
调用的函数  ：HT1621B_WriteData(u8 add,u8 dat)
返回值      ：无
说明        ：无
************************************************************************************/
void LCD_FullDisplay()
{
    u8 i;
    for(i=0;i<15;i++)
    {
        HT1621B_WriteData(i,15);
       
    }
}

/************************************************************************************
函数名      ：LCD_ClearDisplay
功能描述    ：显示屏清屏
参数        ：无
调用的函数  ：HT1621B_WriteData(u8 add,u8 dat)
返回值      ：无
说明        ：无
************************************************************************************/
void LCD_ClearDisplay()
{
    u8 i;
    for(i=0;i<15;i++)
    {
        HT1621B_WriteData(i,0);
       
    }
}

/************************************************************************************
函数名      ：LO_Alarm_DIS
功能描述    ：开力矩报警显示
参数        ：无
调用的函数  ：HT1621B_WriteData(u8 add,u8 dat)
返回值      ：无
说明        ：无
************************************************************************************/
void LO_Alarm_DIS()
{
    HT1621B_WriteData2(0,11);
    HT1621B_WriteData2(1,0);
    HT1621B_WriteData2(2,11);
    HT1621B_WriteData2(3,14);   
}

/************************************************************************************
函数名      ：LC_Alarm_DIS
功能描述    ：关力矩报警显示
参数        ：无
调用的函数  ：HT1621B_WriteData(u8 add,u8 dat)
返回值      ：无
说明        ：无
************************************************************************************/
void LC_Alarm_DIS()
{
    HT1621B_WriteData2(0,11);
    HT1621B_WriteData2(1,0);
    HT1621B_WriteData2(2,11);
    HT1621B_WriteData2(3,8);
   
}

/************************************************************************************
函数名      ：EP_Alarm_DIS
功能描述    ：缺相报警显示
参数        ：无
调用的函数  ：HT1621B_WriteData(u8 add,u8 dat)
返回值      ：无
说明        ：无
************************************************************************************/
void EP_Alarm_DIS()
{
    HT1621B_WriteData2(0,15);
    HT1621B_WriteData2(1,8);
    HT1621B_WriteData2(2,14);
    HT1621B_WriteData2(3,12); 
}

void EE_Alarm_DIS()
{
    HT1621B_WriteData2(0,15);
    HT1621B_WriteData2(1,8);
    HT1621B_WriteData2(2,15);
    HT1621B_WriteData2(3,8); 
    
}

/************************************************************************************
函数名      ：Clear_Alarm_DIS
功能描述    ：闪烁时的清屏
参数        ：无
调用的函数  ：HT1621B_WriteData(u8 add,u8 dat)
返回值      ：无
说明        ：无
************************************************************************************/
void Clear_Alarm_DIS()
{
    HT1621B_WriteData2(0,0);
    HT1621B_WriteData2(1,0);
    HT1621B_WriteData2(2,0);
    HT1621B_WriteData2(3,0); 
}
/************************************************************************************
函数名      ：SET0_DIS()
功能描述    ：标定0%显示L
参数        ：无
调用的函数  ：HT1621B_WriteData(u8 add,u8 dat)
返回值      ：无
说明        ：无
************************************************************************************/
void Def0_DIS()
{
    LCD_ClearDisplay();
    HT1621B_WriteData(0,11);        
}
/************************************************************************************
函数名      ：SET100_DIS()
功能描述    ：标定100%显示H
参数        ：无
调用的函数  ：HT1621B_WriteData(u8 add,u8 dat)
返回值      ：无
说明        ：无
************************************************************************************/
void Def100_DIS()
{
    LCD_ClearDisplay();
    HT1621B_WriteData(2,14);
    HT1621B_WriteData(3,6);
}
void CalLow_DIS()//"LL"
{
	LCD_ClearDisplay();
    HT1621B_WriteData(0,11);
    HT1621B_WriteData(1,0);
	HT1621B_WriteData(2,11);
	HT1621B_WriteData(3,0);
	
}

void CalHigh_DIS()//"HH"
{
	LCD_ClearDisplay();
    HT1621B_WriteData(0,14);
    HT1621B_WriteData(1,6);
	HT1621B_WriteData(2,14);
	HT1621B_WriteData(3,6);
}

void Set_InputLow_DIS()//"1L"
{
	LCD_ClearDisplay();
	HT1621B_WriteData(0,0);
	HT1621B_WriteData(1,6);
	HT1621B_WriteData(2,11);
	HT1621B_WriteData(3,0);
}

void Set_InputHigh_DIS()//"1H"
{
	LCD_ClearDisplay();
	HT1621B_WriteData(0,0);
	HT1621B_WriteData(1,6);
	HT1621B_WriteData(2,14);
	HT1621B_WriteData(3,6);
}

/************************************************************************************
函数名      ：RealTime_Opening_DIS
功能描述    ：显示阀门实时开度
参数        :
调用的函数  ：HT1621B_WriteData(u8 add,u8 dat)
返回值      ：无
说明        ：无
************************************************************************************/
void RealTime_Opening_DIS(float float_data)
{
     u8 temp;
     u8 bai,shi,ge;

     /*四舍五入得到显示值*/
	 temp = (u8)(100 * float_data + 0.5);     
     
	 bai = temp / 100;
	 shi = (temp / 10) % 10;
	 ge  = temp % 10;
			
	if(temp >= 100)
    {
        HT1621B_WriteData(1,15);
        HT1621B_WriteData(0,11);
        HT1621B_WriteData(3,15);
        HT1621B_WriteData(2,11);
    }
    else
    {
        if(temp < 10)
        {
             HT1621B_WriteData(1,0);
             HT1621B_WriteData(0,0);
            
            switch(temp)
          {
            case 0:HT1621B_WriteData(3,15);HT1621B_WriteData(2,11);break;
            case 1:HT1621B_WriteData(3,7);HT1621B_WriteData(2,0);break;
            case 2:HT1621B_WriteData(3,13);HT1621B_WriteData(2,7);break;
            case 3:HT1621B_WriteData(3,15);HT1621B_WriteData(2,5);break;
            case 4:HT1621B_WriteData(3,7);HT1621B_WriteData(2,12);break;
            case 5:HT1621B_WriteData(3,11);HT1621B_WriteData(2,13);break;
            case 6:HT1621B_WriteData(3,11);HT1621B_WriteData(2,15);break;
            case 7:HT1621B_WriteData(3,15);HT1621B_WriteData(2,0);break;
            case 8:HT1621B_WriteData(3,15);HT1621B_WriteData(2,15);break;
            case 9:HT1621B_WriteData(3,15);HT1621B_WriteData(2,13);break;
            default:break;
          }
        }
            
        else
        {         
          switch(shi)
          {
       
            case 1:HT1621B_WriteData(1,6);HT1621B_WriteData(0,0);break;
            case 2:HT1621B_WriteData(1,12);HT1621B_WriteData(0,7);break;
            case 3:HT1621B_WriteData(1,14);HT1621B_WriteData(0,5);break;
            case 4:HT1621B_WriteData(1,6);HT1621B_WriteData(0,12);break;
            case 5:HT1621B_WriteData(1,10);HT1621B_WriteData(0,13);break;
            case 6:HT1621B_WriteData(1,10);HT1621B_WriteData(0,15);break;
            case 7:HT1621B_WriteData(1,14);HT1621B_WriteData(0,0);break;
            case 8:HT1621B_WriteData(1,14);HT1621B_WriteData(0,15);break;
            case 9:HT1621B_WriteData(1,14);HT1621B_WriteData(0,13);break;
            default:break;
          }
      
         switch(ge)
          {
            case 0:HT1621B_WriteData(3,15);HT1621B_WriteData(2,11);break;
            case 1:HT1621B_WriteData(3,7);HT1621B_WriteData(2,0);break;
            case 2:HT1621B_WriteData(3,13);HT1621B_WriteData(2,7);break;
            case 3:HT1621B_WriteData(3,15);HT1621B_WriteData(2,5);break;
            case 4:HT1621B_WriteData(3,7);HT1621B_WriteData(2,12);break;
            case 5:HT1621B_WriteData(3,11);HT1621B_WriteData(2,13);break;
            case 6:HT1621B_WriteData(3,11);HT1621B_WriteData(2,15);break;
            case 7:HT1621B_WriteData(3,15);HT1621B_WriteData(2,0);break;
            case 8:HT1621B_WriteData(3,15);HT1621B_WriteData(2,15);break;
            case 9:HT1621B_WriteData(3,15);HT1621B_WriteData(2,13);break;
            default:break;
         }
       }
     }
}

/************************************************************************************
函数名      ：InputCurrent_DIS()
功能描述    ：显示输入的4-20mA电流
参数        ：kaidu_input,取值范围是0-100，表示输入的阀门开度百分比
调用的函数  ：HT1621B_WriteData(u8 add,u8 dat)
返回值      ：无
说明        ：无
************************************************************************************/
void InputCurrent_DIS(float f_data)
{
    u16 temp16;
	u8 temp8;
    u8 bai,shi;
    
    temp8 = (u8)(f_data+0.5); //四舍五入
    temp16 = (u16)((u16)temp8*10);
	
    bai = temp16 / 100; 
    shi = (temp16 / 10) % 10;
    //ge  = temp % 10;
    
               
    switch(bai)
    {       
    case 0:HT1621B_WriteData(4,0);HT1621B_WriteData(5,0);break;
    case 1:HT1621B_WriteData(4,0);HT1621B_WriteData(5,6);break;
    case 2:HT1621B_WriteData(4,7);HT1621B_WriteData(5,12);break;
    case 3:HT1621B_WriteData(4,5);HT1621B_WriteData(5,14);break;
    case 4:HT1621B_WriteData(4,12);HT1621B_WriteData(5,6);break;
    case 5:HT1621B_WriteData(4,13);HT1621B_WriteData(5,10);break;
    case 6:HT1621B_WriteData(4,15);HT1621B_WriteData(5,10);break;
    case 7:HT1621B_WriteData(4,0);HT1621B_WriteData(5,14);break;
    case 8:HT1621B_WriteData(4,15);HT1621B_WriteData(5,14);break;
    case 9:HT1621B_WriteData(4,13);HT1621B_WriteData(5,14);break;
    default:break;  
    }             
       
       
    switch(shi)
    {
        case 0:HT1621B_WriteData(6,11);HT1621B_WriteData(7,15-1);break;
        case 1:HT1621B_WriteData(6,0);HT1621B_WriteData(7,7-1);break;
        case 2:HT1621B_WriteData(6,7);HT1621B_WriteData(7,13-1);break;
        case 3:HT1621B_WriteData(6,5);HT1621B_WriteData(7,15-1);break;
        case 4:HT1621B_WriteData(6,12);HT1621B_WriteData(7,7-1);break;
        case 5:HT1621B_WriteData(6,13);HT1621B_WriteData(7,11-1);break;
        case 6:HT1621B_WriteData(6,15);HT1621B_WriteData(7,11-1);break;
        case 7:HT1621B_WriteData(6,0);HT1621B_WriteData(7,15-1);break;
        case 8:HT1621B_WriteData(6,15);HT1621B_WriteData(7,15-1);break;
        case 9:HT1621B_WriteData(6,13);HT1621B_WriteData(7,15-1);break;
        default:break;
    }
    HT1621B_WriteData(8,0);
    HT1621B_WriteData(9,0);
              
//          switch(ge)
//          {
//            
//            case 0:HT1621B_WriteData(8,11);HT1621B_WriteData(9,14);break;
//            case 1:HT1621B_WriteData(8,0);HT1621B_WriteData(9,6);break;
//            case 2:HT1621B_WriteData(8,7);HT1621B_WriteData(9,12);break;
//            case 3:HT1621B_WriteData(8,5);HT1621B_WriteData(9,14);break;
//            case 4:HT1621B_WriteData(8,12);HT1621B_WriteData(9,6);break;
//            case 5:HT1621B_WriteData(8,13);HT1621B_WriteData(9,10);break;
//            case 6:HT1621B_WriteData(8,15);HT1621B_WriteData(9,10);break;
//            case 7:HT1621B_WriteData(8,0);HT1621B_WriteData(9,14);break;
//            case 8:HT1621B_WriteData(8,15);HT1621B_WriteData(9,14);break;
//            case 9:HT1621B_WriteData(8,13);HT1621B_WriteData(9,14);break;
//            default:break;   
//              
//          }
       

}
/************************************************************************************
函数名      ：Voltage_DIS_DIS()
功能描述    ：显示电位器电压值
参数        ：float f_data, 电位器电压值
调用的函数  ：HT1621B_WriteData(u8 add,u8 dat)
返回值      ：无
说明        ：无
************************************************************************************/
void Voltage_DIS(float f_data)
{
    u16 temp;

    u8 bai,shi,ge;
    
    temp = (u16)(f_data * 100.0f + 0.5f); //小数点后两位
  
    bai = temp / 100; 
    shi = (temp / 10) % 10;
    ge  = temp % 10;
    
               
    switch(bai)
    {       
        case 0:HT1621B_WriteData(4,11);HT1621B_WriteData(5,15);break;
        case 1:HT1621B_WriteData(4,0);HT1621B_WriteData(5,6+1);break;
        case 2:HT1621B_WriteData(4,7);HT1621B_WriteData(5,12+1);break;
        case 3:HT1621B_WriteData(4,5);HT1621B_WriteData(5,14+1);break;
        case 4:HT1621B_WriteData(4,12);HT1621B_WriteData(5,6+1);break;
        case 5:HT1621B_WriteData(4,13);HT1621B_WriteData(5,10+1);break;
        case 6:HT1621B_WriteData(4,15);HT1621B_WriteData(5,10+1);break;
        case 7:HT1621B_WriteData(4,0);HT1621B_WriteData(5,14+1);break;
        case 8:HT1621B_WriteData(4,15);HT1621B_WriteData(5,14+1);break;
        case 9:HT1621B_WriteData(4,13);HT1621B_WriteData(5,14+1);break;
        default:break;  
    }             
       
       
    switch(shi)
    {
        case 0:HT1621B_WriteData(6,11);HT1621B_WriteData(7,15-1);break;
        case 1:HT1621B_WriteData(6,0);HT1621B_WriteData(7,7-1);break;
        case 2:HT1621B_WriteData(6,7);HT1621B_WriteData(7,13-1);break;
        case 3:HT1621B_WriteData(6,5);HT1621B_WriteData(7,15-1);break;
        case 4:HT1621B_WriteData(6,12);HT1621B_WriteData(7,7-1);break;
        case 5:HT1621B_WriteData(6,13);HT1621B_WriteData(7,11-1);break;
        case 6:HT1621B_WriteData(6,15);HT1621B_WriteData(7,11-1);break;
        case 7:HT1621B_WriteData(6,0);HT1621B_WriteData(7,15-1);break;
        case 8:HT1621B_WriteData(6,15);HT1621B_WriteData(7,15-1);break;
        case 9:HT1621B_WriteData(6,13);HT1621B_WriteData(7,15-1);break;
        default:break;
    }
      
    switch(ge)
    {

        case 0:HT1621B_WriteData(8,11);HT1621B_WriteData(9,14);break;
        case 1:HT1621B_WriteData(8,0);HT1621B_WriteData(9,6);break;
        case 2:HT1621B_WriteData(8,7);HT1621B_WriteData(9,12);break;
        case 3:HT1621B_WriteData(8,5);HT1621B_WriteData(9,14);break;
        case 4:HT1621B_WriteData(8,12);HT1621B_WriteData(9,6);break;
        case 5:HT1621B_WriteData(8,13);HT1621B_WriteData(9,10);break;
        case 6:HT1621B_WriteData(8,15);HT1621B_WriteData(9,10);break;
        case 7:HT1621B_WriteData(8,0);HT1621B_WriteData(9,14);break;
        case 8:HT1621B_WriteData(8,15);HT1621B_WriteData(9,14);break;
        case 9:HT1621B_WriteData(8,13);HT1621B_WriteData(9,14);break;
        default:break;   
      
    }
       

}
/************************************************************************************
函数名      ：Encoder_DIS()
功能描述    ：显示编码器数据，0-999
参数        ：u16 adcdat,编码器14位数据
调用的函数  ：HT1621B_WriteData(u8 add,u8 dat)
返回值      ：无
说明        ：无
************************************************************************************/
void Encoder_DIS(u16 adcdat)
{
    u16 dat = (u16)((float)adcdat/16.3994f+0.5f);//0-16383 --> 0-999
    u8 bai,shi,ge;
    
    bai = dat / 100; 
    shi = (dat / 10) % 10;
    ge  = dat % 10;
    
    switch(bai)
    {       
        case 0:HT1621B_WriteData(4,11);HT1621B_WriteData(5,14);break;
        case 1:HT1621B_WriteData(4,0);HT1621B_WriteData(5,6);break;
        case 2:HT1621B_WriteData(4,7);HT1621B_WriteData(5,12);break;
        case 3:HT1621B_WriteData(4,5);HT1621B_WriteData(5,14);break;
        case 4:HT1621B_WriteData(4,12);HT1621B_WriteData(5,6);break;
        case 5:HT1621B_WriteData(4,13);HT1621B_WriteData(5,10);break;
        case 6:HT1621B_WriteData(4,15);HT1621B_WriteData(5,10);break;
        case 7:HT1621B_WriteData(4,0);HT1621B_WriteData(5,14);break;
        case 8:HT1621B_WriteData(4,15);HT1621B_WriteData(5,14);break;
        case 9:HT1621B_WriteData(4,13);HT1621B_WriteData(5,14);break;
        default:break;  
    }             
       
       
    switch(shi)
    {
        case 0:HT1621B_WriteData(6,11);HT1621B_WriteData(7,15-1);break;
        case 1:HT1621B_WriteData(6,0);HT1621B_WriteData(7,7-1);break;
        case 2:HT1621B_WriteData(6,7);HT1621B_WriteData(7,13-1);break;
        case 3:HT1621B_WriteData(6,5);HT1621B_WriteData(7,15-1);break;
        case 4:HT1621B_WriteData(6,12);HT1621B_WriteData(7,7-1);break;
        case 5:HT1621B_WriteData(6,13);HT1621B_WriteData(7,11-1);break;
        case 6:HT1621B_WriteData(6,15);HT1621B_WriteData(7,11-1);break;
        case 7:HT1621B_WriteData(6,0);HT1621B_WriteData(7,15-1);break;
        case 8:HT1621B_WriteData(6,15);HT1621B_WriteData(7,15-1);break;
        case 9:HT1621B_WriteData(6,13);HT1621B_WriteData(7,15-1);break;
        default:break;
    }
      
    switch(ge)
    {

        case 0:HT1621B_WriteData(8,11);HT1621B_WriteData(9,14);break;
        case 1:HT1621B_WriteData(8,0);HT1621B_WriteData(9,6);break;
        case 2:HT1621B_WriteData(8,7);HT1621B_WriteData(9,12);break;
        case 3:HT1621B_WriteData(8,5);HT1621B_WriteData(9,14);break;
        case 4:HT1621B_WriteData(8,12);HT1621B_WriteData(9,6);break;
        case 5:HT1621B_WriteData(8,13);HT1621B_WriteData(9,10);break;
        case 6:HT1621B_WriteData(8,15);HT1621B_WriteData(9,10);break;
        case 7:HT1621B_WriteData(8,0);HT1621B_WriteData(9,14);break;
        case 8:HT1621B_WriteData(8,15);HT1621B_WriteData(9,14);break;
        case 9:HT1621B_WriteData(8,13);HT1621B_WriteData(9,14);break;
        default:break;   
      
    }
    
    
    
}

//瀹炴椂鐢靛帇鏄剧ず
void LCD_SetDIS()
{
    float f_data;
    u16 encoder_data;
    
    if(use_encoder_flag==0)
    {
        f_data = pData_Voltage->voltage_valve;
        Voltage_DIS(f_data);
        Local_DIS();
    }
    else
    {
        encoder_data = (u16)pData_ADC->adcvalue_encoder_filtered;
        Encoder_DIS(encoder_data);
        Local_DIS();
    }
    
}
void Local_DIS()
{
    HT1621B_WriteData(14,2);  
}






/************************************************************************************
函数名      ：OutputCurrent_DIS
功能描述    ：显示输出的4-20mA电流
参数        ：
调用的函数  ：HT1621B_WriteData(u8 add,u8 dat)
返回值      ：无
说明        ：无
************************************************************************************/
void OutputCurrent_DIS(float f_data)
{
    u16 temp16;
	  u8 temp8;
    u8 bai,shi;
    
    temp8 = (u8)(f_data+0.5);  //小数点后一位   
    temp16 = (u16)((u16)temp8*10);
	
    bai = temp16 / 100; 
    shi = (temp16 / 10) % 10;
    //ge  = temp % 10;
    
    
    if(bai == 0)
    {
          
        if(DISTANT)     HT1621B_WriteData(14,4);
        else            HT1621B_WriteData(14,2);
          switch(shi)
          {
            case 0:HT1621B_WriteData(10,11);HT1621B_WriteData(11,15-1);break;
            case 1:HT1621B_WriteData(10,0);HT1621B_WriteData(11,7-1);break;
            case 2:HT1621B_WriteData(10,7);HT1621B_WriteData(11,13-1);break;
            case 3:HT1621B_WriteData(10,5);HT1621B_WriteData(11,15-1);break;
            case 4:HT1621B_WriteData(10,12);HT1621B_WriteData(11,7-1);break;
            case 5:HT1621B_WriteData(10,13);HT1621B_WriteData(11,11-1);break;
            case 6:HT1621B_WriteData(10,15);HT1621B_WriteData(11,11-1);break;
            case 7:HT1621B_WriteData(10,0);HT1621B_WriteData(11,15-1);break;
            case 8:HT1621B_WriteData(10,15);HT1621B_WriteData(11,15-1);break;
            case 9:HT1621B_WriteData(10,13);HT1621B_WriteData(11,15-1);break;
            default:break;
          }
        HT1621B_WriteData(12,0);HT1621B_WriteData(13,0);

//          switch(ge)
//          {
//            case 0:HT1621B_WriteData(12,11);HT1621B_WriteData(13,14);break;
//            case 1:HT1621B_WriteData(12,0);HT1621B_WriteData(13,6);break;
//            case 2:HT1621B_WriteData(12,7);HT1621B_WriteData(13,12);break;
//            case 3:HT1621B_WriteData(12,5);HT1621B_WriteData(13,14);break;
//            case 4:HT1621B_WriteData(12,12);HT1621B_WriteData(13,6);break;
//            case 5:HT1621B_WriteData(12,13);HT1621B_WriteData(13,10);break;
//            case 6:HT1621B_WriteData(12,15);HT1621B_WriteData(13,10);break;
//            case 7:HT1621B_WriteData(12,0);HT1621B_WriteData(13,14);break;
//            case 8:HT1621B_WriteData(12,15);HT1621B_WriteData(13,14);break;
//            case 9:HT1621B_WriteData(12,13);HT1621B_WriteData(13,14);break;
//            default:break;
//         }        
     }
            
        if(bai > 1)
        {         
            if(DISTANT)     HT1621B_WriteData(14,4);
            else            HT1621B_WriteData(14,2);
            switch(bai)
            {
                case 0:HT1621B_WriteData(10,11);HT1621B_WriteData(11,14);break;
                case 1:HT1621B_WriteData(10,0);HT1621B_WriteData(11,6);break;
                case 2:HT1621B_WriteData(10,7);HT1621B_WriteData(11,12);break;
                case 3:HT1621B_WriteData(10,5);HT1621B_WriteData(11,14);break;
                case 4:HT1621B_WriteData(10,12);HT1621B_WriteData(11,6);break;
                case 5:HT1621B_WriteData(10,13);HT1621B_WriteData(11,10);break;
                case 6:HT1621B_WriteData(10,15);HT1621B_WriteData(11,10);break;
                case 7:HT1621B_WriteData(10,0);HT1621B_WriteData(11,14);break;
                case 8:HT1621B_WriteData(10,15);HT1621B_WriteData(11,14);break;
                case 9:HT1621B_WriteData(10,13);HT1621B_WriteData(11,14);break;
                default:break;
            }
      
            switch(shi)
            {
                case 0:HT1621B_WriteData(12,11);HT1621B_WriteData(13,14);break;
                case 1:HT1621B_WriteData(12,0);HT1621B_WriteData(13,6);break;
                case 2:HT1621B_WriteData(12,7);HT1621B_WriteData(13,12);break;
                case 3:HT1621B_WriteData(12,5);HT1621B_WriteData(13,14);break;
                case 4:HT1621B_WriteData(12,12);HT1621B_WriteData(13,6);break;
                case 5:HT1621B_WriteData(12,13);HT1621B_WriteData(13,10);break;
                case 6:HT1621B_WriteData(12,15);HT1621B_WriteData(13,10);break;
                case 7:HT1621B_WriteData(12,0);HT1621B_WriteData(13,14);break;
                case 8:HT1621B_WriteData(12,15);HT1621B_WriteData(13,14);break;
                case 9:HT1621B_WriteData(12,13);HT1621B_WriteData(13,14);break;
                default:break;
            }  
       }


       if(bai == 1)
       {
                      
           if(!DISTANT)     HT1621B_WriteData(14,2+8);
           else             HT1621B_WriteData(14,4+8);
 
           switch(shi)
           {
                case 0:HT1621B_WriteData(10,11);HT1621B_WriteData(11,15-1);break;
                case 1:HT1621B_WriteData(10,0);HT1621B_WriteData(11,7-1);break;
                case 2:HT1621B_WriteData(10,7);HT1621B_WriteData(11,13-1);break;
                case 3:HT1621B_WriteData(10,5);HT1621B_WriteData(11,15-1);break;
                case 4:HT1621B_WriteData(10,12);HT1621B_WriteData(11,7-1);break;
                case 5:HT1621B_WriteData(10,13);HT1621B_WriteData(11,11-1);break;
                case 6:HT1621B_WriteData(10,15);HT1621B_WriteData(11,11-1);break;
                case 7:HT1621B_WriteData(10,0);HT1621B_WriteData(11,15-1);break;
                case 8:HT1621B_WriteData(10,15);HT1621B_WriteData(11,15-1);break;
                case 9:HT1621B_WriteData(10,13);HT1621B_WriteData(11,15-1);break;
                default:break;
           }
            HT1621B_WriteData(12,0);HT1621B_WriteData(13,0);
//              switch(ge)
//              {
//                case 0:HT1621B_WriteData(12,11);HT1621B_WriteData(13,14);break;
//                case 1:HT1621B_WriteData(12,0);HT1621B_WriteData(13,6);break;
//                case 2:HT1621B_WriteData(12,7);HT1621B_WriteData(13,12);break;
//                case 3:HT1621B_WriteData(12,5);HT1621B_WriteData(13,14);break;
//                case 4:HT1621B_WriteData(12,12);HT1621B_WriteData(13,6);break;
//                case 5:HT1621B_WriteData(12,13);HT1621B_WriteData(13,10);break;
//                case 6:HT1621B_WriteData(12,15);HT1621B_WriteData(13,10);break;
//                case 7:HT1621B_WriteData(12,0);HT1621B_WriteData(13,14);break;
//                case 8:HT1621B_WriteData(12,15);HT1621B_WriteData(13,14);break;
//                case 9:HT1621B_WriteData(12,13);HT1621B_WriteData(13,14);break;
//                default:break;
//             }
    
       }

       
}

/************************************************************************************
函数名      ：SEN_DIS
功能描述    ：显示灵敏度
参数        ：none
调用的函数  ：HT1621B_WriteData(u8 add,u8 dat)
返回值      ：无
说明        ：无
************************************************************************************/
void Sen_DIS(u16 sen_data)
{
      
      switch(sen_data)
      {
        case SEN0_05MA: HT1621B_WriteData(0,0);HT1621B_WriteData(2,0);HT1621B_WriteData(3,6);break;//1
        case SEN0_1MA:  HT1621B_WriteData(0,0);HT1621B_WriteData(2,7);HT1621B_WriteData(3,12);break;//2
        case SEN0_15MA: HT1621B_WriteData(0,0);HT1621B_WriteData(2,5);HT1621B_WriteData(3,14);break;//3
        case SEN0_2MA:  HT1621B_WriteData(0,0);HT1621B_WriteData(2,12);HT1621B_WriteData(3,6);break;//4
        case SEN0_25MA: HT1621B_WriteData(0,0);HT1621B_WriteData(2,13);HT1621B_WriteData(3,10);break;//5
        case SEN0_3MA:  HT1621B_WriteData(0,0);HT1621B_WriteData(2,15);HT1621B_WriteData(3,10);break;//6
        case SEN0_35MA: HT1621B_WriteData(0,0);HT1621B_WriteData(2,0);HT1621B_WriteData(3,14);break;//7
        case SEN0_4MA : HT1621B_WriteData(0,0);HT1621B_WriteData(2,15);HT1621B_WriteData(3,14);break;//8
        case SEN0_45MA: HT1621B_WriteData(0,0);HT1621B_WriteData(2,13);HT1621B_WriteData(3,14);break;//9
        case SEN0_5MA : HT1621B_WriteData(0,10);HT1621B_WriteData(2,11);HT1621B_WriteData(3,14);break;//10      
        default:break;
      }
      //"L"
      HT1621B_WriteData(10,11);

}
/*************绯荤粺LCD鏄剧ず***************/
void LCD_DIS()
{
    float f_data;
    
    if(torquelock_open_flag == 0 && torquelock_close_flag == 0 && motorerr_flag==0) 
    {        
        f_data = pData_Acquire->opening;
        RealTime_Opening_DIS(f_data); 

        f_data = pData_Acquire->current_input;
        InputCurrent_DIS(f_data); 

        f_data = pData_Acquire->current_output;
        OutputCurrent_DIS(f_data); 
    }

}

//鐢垫満杩愯鐘舵�丩ED鏄剧ず
void LED_MotorDIS_500ms()
{
    u8 valve_position;
    
    switch(motor_status)
    {
        case MOTOR_STATUS_OPEN:
        {
            LCD_RED = !LCD_RED;
            LCD_GREEN_OFF
            break;
        }
        case MOTOR_STATUS_CLOSE:
        {
            LCD_GREEN = !LCD_GREEN;
            LCD_RED_OFF
            break;
        }
        case MOTOR_STATUS_STOP:
        {
            valve_position = LimitPosition_Output();
            if(0 == valve_position)
            {
                LCD_GREEN_ON
                LCD_RED_OFF
            }
            if(1 == valve_position)
            {
                LCD_RED_ON
                LCD_GREEN_OFF
            }
            if(2 == valve_position)
            {
                LCD_RED_OFF
                LCD_GREEN_OFF
            }
            break;
        }
        default:break;
    }
  
}
void LCD_ALARM_DIS()
{
    float f_data;

    f_data = pData_Acquire->current_input;
    InputCurrent_DIS(f_data); //

    f_data = pData_Acquire->current_output;
    OutputCurrent_DIS(f_data); //

}

//鍔涚煩淇濇姢鎶ヨ闂儊鏄剧ず,鎵ц鍛ㄦ湡500ms
void LCD_TorqueDIS_500ms()
{
    static lo_on = 1;
    static lc_on = 1;
    if(torquelock_open_flag == 1)
    {
        if(lo_on == 1)
        {
            Clear_Alarm_DIS();
            Clear_Alarm_DIS();
            lo_on = 0;
        }
        else
        {
            LO_Alarm_DIS();
            LO_Alarm_DIS();
            lo_on = 1;
        }
        LCD_ALARM_DIS();        
        
    }

    if(torquelock_close_flag == 1)
    {
        if(lc_on)
        {
            Clear_Alarm_DIS();
            Clear_Alarm_DIS();
            lc_on = 0;
        }
        else
        {
            LC_Alarm_DIS();
            LC_Alarm_DIS();
            lc_on = 1;
        }
        LCD_ALARM_DIS();        
    } 
}
//鐢垫満鎶ヨ鏄剧ず"EE"
void LCD_MotorErrDIS()
{
    static u8 on = 0;
    
    if(motorerr_flag == 1)
    {
        if(on)
        {
            EE_Alarm_DIS();
            EE_Alarm_DIS();
            on = 0;
        }
        else
        {
            Clear_Alarm_DIS();
            Clear_Alarm_DIS();
            on = 1;
        }
    }
              
}


        