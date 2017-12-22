/************************************************************************************
�ļ��� ��LCD.c
����   ������ѫ
˵��   ��LCD��ʾ������������ʾ��Ļ�ϵ�����

*************************************************************************************/
#include <config.h>
#include "delay.h"
#include "GPIO.h"
#include "IORedefine.h"
#include "HT1621B.h"
#include "HT1621B_Drive.h"
#include "LCD.h"
#include "data_input.h"
#include "data_save.h"
#include "motor.h"
#include "phase_detect.h"
#include "current_output.h"
#include "key.h"

/************************************************************************************
                                   ��������
************************************************************************************/

/************************************************************************************
                                   ��������
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
    GPIO_InitStructure.Mode = GPIO_OUT_PP;//�������
    GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);
    GPIO_InitStructure.Pin = GPIO_Pin_4;
    GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);
}
    
/************************************************************************************
������      ��HT1621B_Init
��������    ����ʾ����ʼ��
����        ����
���õĺ���  ��HT1621B_WriteData(u8 add,u8 dat)
               HT1621B_WriteCommand(u8 cmd)
               LCD_ClearDisplay()
               Delay1us()
               Delay50us()
����ֵ      ����
˵��        ����
************************************************************************************/
void LCD_Init()
{  
    LCDIO_Config();
    
    LCD_RED_OFF        
    LCD_GREEN_OFF
    LCD_YELLOW_OFF
    HT1621B_CS_HIGH
    Delay1us();
    HT1621B_WR_HIGH
    HT1621B_DATA_HIGH
    Delay50us();
    HT1621B_WriteCommand(BIAS);
    HT1621B_WriteCommand(RC256);    
    HT1621B_WriteCommand(SYSTEM_ON);        
    HT1621B_WriteCommand(LCD_ON);
    LCD_ClearDisplay();
    //LCD_FullDisplay();
}

/************************************************************************************
������      ��LCD_FullDisplay
��������    ����ʾ��ȫ����ʾ
����        ����
���õĺ���  ��HT1621B_WriteData(u8 add,u8 dat)
����ֵ      ����
˵��        ����
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
������      ��LCD_ClearDisplay
��������    ����ʾ������
����        ����
���õĺ���  ��HT1621B_WriteData(u8 add,u8 dat)
����ֵ      ����
˵��        ����
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
������      ��LO_Alarm_DIS
��������    �������ر�����ʾ
����        ����
���õĺ���  ��HT1621B_WriteData(u8 add,u8 dat)
����ֵ      ����
˵��        ����
************************************************************************************/
void LO_Alarm_DIS()
{
    HT1621B_WriteData(0,11);
    HT1621B_WriteData(1,0);
    HT1621B_WriteData(2,11);
    HT1621B_WriteData(3,14);   
}

/************************************************************************************
������      ��LC_Alarm_DIS
��������    �������ر�����ʾ
����        ����
���õĺ���  ��HT1621B_WriteData(u8 add,u8 dat)
����ֵ      ����
˵��        ����
************************************************************************************/
void LC_Alarm_DIS()
{
    HT1621B_WriteData(0,11);
    HT1621B_WriteData(1,0);
    HT1621B_WriteData(2,11);
    HT1621B_WriteData(3,8);
   
}

/************************************************************************************
������      ��EP_Alarm_DIS
��������    ��ȱ�౨����ʾ
����        ����
���õĺ���  ��HT1621B_WriteData(u8 add,u8 dat)
����ֵ      ����
˵��        ����
************************************************************************************/
void EP_Alarm_DIS()
{
    HT1621B_WriteData(0,15);
    HT1621B_WriteData(1,8);
    HT1621B_WriteData(2,14);
    HT1621B_WriteData(3,12); 
}

/************************************************************************************
������      ��Clear_Alarm_DIS
��������    ����˸ʱ������
����        ����
���õĺ���  ��HT1621B_WriteData(u8 add,u8 dat)
����ֵ      ����
˵��        ����
************************************************************************************/
void Clear_Alarm_DIS()
{
    HT1621B_WriteData(0,0);
    HT1621B_WriteData(1,0);
    HT1621B_WriteData(2,0);
    HT1621B_WriteData(3,0); 
}/************************************************************************************
������      ��SET0_DIS()
��������    ���궨0%��ʾL
����        ����
���õĺ���  ��HT1621B_WriteData(u8 add,u8 dat)
����ֵ      ����
˵��        ����
************************************************************************************/
void Def0_DIS()
{
    LCD_ClearDisplay();
    HT1621B_WriteData(0,11);        
}
/************************************************************************************
������      ��SET100_DIS()
��������    ���궨100%��ʾH
����        ����
���õĺ���  ��HT1621B_WriteData(u8 add,u8 dat)
����ֵ      ����
˵��        ����
************************************************************************************/void Def100_DIS()
{
    LCD_ClearDisplay();
    HT1621B_WriteData(2,14);
    HT1621B_WriteData(3,6);
}
   
/************************************************************************************
������      ��RealTime_Opening_DIS
��������    ����ʾ����ʵʱ����
����        :
���õĺ���  ��HT1621B_WriteData(u8 add,u8 dat)
����ֵ      ����
˵��        ����
************************************************************************************/
void RealTime_Opening_DIS(float float_data)
{
     u8 temp;
     u8 bai,shi,ge;
     //float opening_tmp;
    
    //opening_tmp = Get_Opening(OPENING_REALTIME);

     /*��������õ���ʾֵ*/
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
������      ��InputCurrent_DIS()
��������    ����ʾ�����4-20mA����
����        ��kaidu_input,ȡֵ��Χ��0-100����ʾ����ķ��ſ��Ȱٷֱ�
���õĺ���  ��HT1621B_WriteData(u8 add,u8 dat)
����ֵ      ����
˵��        ����
************************************************************************************/
void InputCurrent_DIS(float f_data)
{
    u16 temp;
    //float current_tmp;
    u8 bai,shi,ge;
    
    //current_tmp = Get_InputCurrent();
    temp = (u16)(f_data * 10 + 0.5); //С�����һλ
  
    bai = temp / 100; 
    shi = (temp / 10) % 10;
    ge  = temp % 10;
    
               
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
            case 0:HT1621B_WriteData(6,11);HT1621B_WriteData(7,15);break;
            case 1:HT1621B_WriteData(6,0);HT1621B_WriteData(7,7);break;
            case 2:HT1621B_WriteData(6,7);HT1621B_WriteData(7,13);break;
            case 3:HT1621B_WriteData(6,5);HT1621B_WriteData(7,15);break;
            case 4:HT1621B_WriteData(6,12);HT1621B_WriteData(7,7);break;
            case 5:HT1621B_WriteData(6,13);HT1621B_WriteData(7,11);break;
            case 6:HT1621B_WriteData(6,15);HT1621B_WriteData(7,11);break;
            case 7:HT1621B_WriteData(6,0);HT1621B_WriteData(7,15);break;
            case 8:HT1621B_WriteData(6,15);HT1621B_WriteData(7,15);break;
            case 9:HT1621B_WriteData(6,13);HT1621B_WriteData(7,15);break;
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
������      ��OutputCurrent_DIS
��������    ����ʾ�����4-20mA����
����        ��
���õĺ���  ��HT1621B_WriteData(u8 add,u8 dat)
����ֵ      ����
˵��        ����
************************************************************************************/
void OutputCurrent_DIS(float f_data)
{
    u16 temp;
    //float current_tmp;
    u8 bai,shi, ge;
    
    //current_tmp = Get_OutputCurrent();
    temp = (u16)(f_data * 10 + 0.5);  //С�����һλ   
    
    bai = temp / 100; 
    shi = (temp / 10) % 10;
    ge  = temp % 10;
    
    
    if(bai == 0)
    {
          current_out_flag = 0;
          switch(shi)
          {
            case 0:HT1621B_WriteData(10,11);HT1621B_WriteData(11,15);break;
            case 1:HT1621B_WriteData(10,0);HT1621B_WriteData(11,7);break;
            case 2:HT1621B_WriteData(10,7);HT1621B_WriteData(11,13);break;
            case 3:HT1621B_WriteData(10,5);HT1621B_WriteData(11,15);break;
            case 4:HT1621B_WriteData(10,12);HT1621B_WriteData(11,7);break;
            case 5:HT1621B_WriteData(10,13);HT1621B_WriteData(11,11);break;
            case 6:HT1621B_WriteData(10,15);HT1621B_WriteData(11,11);break;
            case 7:HT1621B_WriteData(10,0);HT1621B_WriteData(11,15);break;
            case 8:HT1621B_WriteData(10,15);HT1621B_WriteData(11,15);break;
            case 9:HT1621B_WriteData(10,13);HT1621B_WriteData(11,15);break;
            default:break;
          }


          switch(ge)
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
            
        if(bai > 1)
        {         
          current_out_flag = 0;
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
           current_out_flag = 1;
           
           if(LOCAL & LOCAL_STOP)
           {
               HT1621B_WriteData(14,4+8);    
           } 
           else
           {
               HT1621B_WriteData(14,2+8);                   
           }
           
           switch(shi)
           {
                case 0:HT1621B_WriteData(10,11);HT1621B_WriteData(11,15);break;
                case 1:HT1621B_WriteData(10,0);HT1621B_WriteData(11,7);break;
                case 2:HT1621B_WriteData(10,7);HT1621B_WriteData(11,13);break;
                case 3:HT1621B_WriteData(10,5);HT1621B_WriteData(11,15);break;
                case 4:HT1621B_WriteData(10,12);HT1621B_WriteData(11,7);break;
                case 5:HT1621B_WriteData(10,13);HT1621B_WriteData(11,11);break;
                case 6:HT1621B_WriteData(10,15);HT1621B_WriteData(11,11);break;
                case 7:HT1621B_WriteData(10,0);HT1621B_WriteData(11,15);break;
                case 8:HT1621B_WriteData(10,15);HT1621B_WriteData(11,15);break;
                case 9:HT1621B_WriteData(10,13);HT1621B_WriteData(11,15);break;
                default:break;
           }

              switch(ge)
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

       
}

/************************************************************************************
������      ��SEN_DIS
��������    ����ʾ������
����        ��none
���õĺ���  ��HT1621B_WriteData(u8 add,u8 dat)
����ֵ      ����
˵��        ����
************************************************************************************/
void Sen_DIS()
{
      u16 sen_tmp;
      sen_tmp = IapRead_Sen();
      switch(sen_tmp)
      {
        case SEN0_01MA:HT1621B_WriteData(2,11);HT1621B_WriteData(3,15);break;
        case SEN0_1MA:HT1621B_WriteData(2,0);HT1621B_WriteData(3,7);break;
        case SEN0_2MA:HT1621B_WriteData(2,7);HT1621B_WriteData(3,13);break;
        case SEN0_4MA:HT1621B_WriteData(2,5);HT1621B_WriteData(3,15);break;
        case SEN0_6MA:HT1621B_WriteData(2,12);HT1621B_WriteData(3,7);break;
        case SEN0_8MA:HT1621B_WriteData(2,13);HT1621B_WriteData(3,11);break;
        default:break;
      }
    
}
//void LCD_LED_DIS(float rt_opening, )
//{
//     float opening_rt_tmp;
//    
//     opening_rt_tmp = Get_Opening(OPENING_REALTIME);
//     if(opening_rt_tmp <= 0.005)
//     {
//         LCD_GREEN_ON

//     }
//     if(opening_rt_tmp >= 0.995)
//     {
//         LCD_RED_ON
//     }
//     
//     if(opening_rt_tmp > 0.005 && opening_rt_tmp < 0.995)
//     {
//         if(motor_status == MOTOR_STATUS_STOP)
//         {
//             LCD_RED_OFF
//             LCD_GREEN_OFF
//         }
//     }
//          
//   
//}



//void LCD_DIS()
//{
//     if(motor_status == MOTOR_STATUS_UP || motor_status == MOTOR_STATUS_DOWN || phase_status == PHASE_STATUS_LACK )
//     {
//         _nop_();
//     }
//     else
//     {
//         RealTime_Opening_DIS();
//     }
//    
//    InputCurrent_DIS();
//    OutputCurrent_DIS();
//    
//    if(LOCAL & LOCAL_STOP)
//    {
//        if(current_out_flag)
//        {
//            HT1621B_WriteData(14,4+8);
//        }
//        else
//        {
//            HT1621B_WriteData(14,4);
//        }     
//    }
//    else
//    {
//        if(current_out_flag)
//        {
//            HT1621B_WriteData(14,2+8);
//        }
//        else
//        {
//            HT1621B_WriteData(14,2);
//        }           
//    }
//    LCD_LED_DIS();

//}

//void Bit_DIS(u8 line_num, u8 bit_num, u8 num)
//{
//    if(line_num == 1)// the first line
//    {
//        switch(bit_num)
//        {
//            case 1:
//            {
//                if(num == 1)
//                {
//                    HT1621B_WriteData(1,1);
//                }
//                else
//                {
//                    HT1621B_WriteData(1,0);
//                }
//                break;
//            }
//            
//            case 2:
//            {
//                switch(num)
//                {
//                    case 0:HT1621B_WriteData(1,0);HT1621B_WriteData(1,0);
//                    case 1:
//                    case 2:    
//                    case 3:
//                    case 4:
//                    case 5:    
//                    case 6:
//                    case 7:
//                    case 8:    
//                    case 9:
//                    default:break;
//        
//    }
//    if(line_num == 2)// display on the second line
//    {
//        
//    }
//    
//    
//    
//}

void LCD_TEST()
{
    u8 i;
    u8 j;
    for(i=0;i<15;i++)
    {
        LCD_ClearDisplay();
        for(j=1;j<=8;j<<=1)
        {
            HT1621B_WriteData(i,j);
            delay_ms(500);
            
        }  
    }
}
        