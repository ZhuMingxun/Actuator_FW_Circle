/************************************************************************************
文件名 ：HT1621B_Drive.c
作者   ：朱明勋
说明   ：HT1621B的驱动函数
主要函数 ： HT1621B_WriteCommand(u8 cmd)     //向HT1621B写命令
            HT1621B_WriteData(u8 add,u8 dat) //向HT1621B写数据

*************************************************************************************/
#include <config.h>
#include "delay.h"
#include "IORedefine.h"
#include "HT1621B_Drive.h"

/************************************************************************************
函数名       ：WRCLK
功能描述     ：HT1621B写时序脉冲
调用函数列表 ：Delay10us()
返回值       ：无
说明         ：无
************************************************************************************/
void WRCLK()
{
    _nop_();
    HT1621B_WR_LOW
    Delay10us();
    HT1621B_WR_HIGH
    Delay10us();
}

/************************************************************************************
函数名      ：HT1621B_WriteCommand
功能描述    ：向HT1621B中写入命令
参数        ：cmd,无符号8位整型变量，命令码
调用的函数  ： WRCLK()， Delay1us()
返回值      ：无
说明        ：无
************************************************************************************/
void HT1621B_WriteCommand(u8 cmd)
{
    u8 i;
    HT1621B_CS_LOW
    Delay1us();
    
    HT1621B_DATA_HIGH
    WRCLK();
    HT1621B_DATA_LOW
    WRCLK();
    HT1621B_DATA_LOW
    WRCLK();
    
    for(i = 0;i < 9;i++)
    {
        if(cmd & 0x80)
        {
            HT1621B_DATA_HIGH									
        }
				
        else
        {
            HT1621B_DATA_LOW										
        }
        WRCLK();
        cmd<<=1;
    }
    Delay1us();
    HT1621B_CS_HIGH
    Delay1us();
}

/************************************************************************************
函数名      ：HT1621B_WriteData
功能描述    ：向HT1621B中写入数据
参数        ：add，6位地址
              dat，4位数据
调用的函数  ： WRCLK(),Delay1us()
返回值      ：无
说明        ：无
************************************************************************************/
void HT1621B_WriteData(u8 add, u8 dat)
{
    u8 i;
    HT1621B_CS_LOW
    Delay1us();
    
    HT1621B_DATA_HIGH
    WRCLK();
    HT1621B_DATA_LOW
    WRCLK();
    HT1621B_DATA_HIGH
    WRCLK();
    
    for(i = 0;i < 6;i++)
    {
        if(add & 0x20)
        { 
            HT1621B_DATA_HIGH
        }
        else 
        { 
            HT1621B_DATA_LOW
        }
        WRCLK();
        add<<=1;
    }
    Delay1us();
    for(i = 0;i < 4;i++)
    {
        if(dat & 0x01)
        {
            HT1621B_DATA_HIGH
        }
        else 
        { 
            HT1621B_DATA_LOW
        }
        WRCLK();
        dat>>=1;
    }
    Delay1us();
    HT1621B_CS_HIGH
    Delay1us();
}