/************************************************************************************
文件名 ：HT1621B_Drive.h
作者   ：朱明勋
说明   ：HT1621B的驱动函数声明
主要函数 ：HT1621B_WriteCommand(u8 cmd)     //向HT1621B写命令
           HT1621B_WriteData(u8 add,u8 dat) //向HT1621B写数据
*************************************************************************************/


#ifndef __HT1621B_Drive_H_
#define __HT1621B_Drive_H_


#define     HT1621B_WR_HIGH         {HT1621B_WR = 1;}
#define     HT1621B_WR_LOW          {HT1621B_WR = 0;}
#define     HT1621B_CS_HIGH         {HT1621B_CS = 1;}
#define     HT1621B_CS_LOW          {HT1621B_CS = 0;}
#define     HT1621B_DATA_HIGH       {HT1621B_DATA = 1;}
#define     HT1621B_DATA_LOW        {HT1621B_DATA = 0;}


void  HT1621B_WriteCommand(unsigned char  cmd);
void  HT1621B_WriteData(unsigned char add,unsigned char dat);    


#endif