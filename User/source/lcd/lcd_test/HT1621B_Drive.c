/************************************************************************************
�ļ��� ��HT1621B_Drive.c
����   ������ѫ
˵��   ��HT1621B����������
��Ҫ���� �� HT1621B_WriteCommand(u8 cmd)     //��HT1621Bд����
            HT1621B_WriteData(u8 add,u8 dat) //��HT1621Bд����

*************************************************************************************/
#include <config.h>
#include "delay.h"
#include "IORedefine.h"
#include "HT1621B_Drive.h"

/************************************************************************************
������       ��WRCLK
��������     ��HT1621Bдʱ������
���ú����б� ��Delay10us()
����ֵ       ����
˵��         ����
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
������      ��HT1621B_WriteCommand
��������    ����HT1621B��д������
����        ��cmd,�޷���8λ���ͱ�����������
���õĺ���  �� WRCLK()�� Delay1us()
����ֵ      ����
˵��        ����
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
������      ��HT1621B_WriteData
��������    ����HT1621B��д������
����        ��add��6λ��ַ
              dat��4λ����
���õĺ���  �� WRCLK(),Delay1us()
����ֵ      ����
˵��        ����
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