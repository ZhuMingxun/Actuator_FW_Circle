/*********************************************************************
�ļ�����data_save.c
���ߣ�����ѫ
˵����дFLASH�Ͷ�FLASH���� �����ڱ��濪��λ�͹ص�λADC����ֵ
��Ҫ������ void IapIdle();  //�ر�IAP
            u8 IapReadByte(u16 addr);    //��EEPROM�ж�����
            void IapProgramByte(u16 add, u8 dat);  //���һ���ֽ�
            void IapEraseSector(u16 addr);   //��������


*********************************************************************/
#include "config.h"
#include "data_save.h"
#include "delay.h"

#define     CMD_READ        1
#define     CMD_PROGRAM     2
#define     CMD_ERASE       3
#define     ENABLE_IAP      0x83

/************************************************************************
                              ��������
*************************************************************************/
void IapIdle();
extern u16 opening0_adc;
extern u16 opening100_adc;
extern u16 sen;
extern float R_in;
extern float R_out;
/*************************************************************
��������IapIdle()
�������ر�IAP
��������
����ֵ����
���ߣ�STC
***************************************************************/
void IapIdle()
{
    IAP_CONTR = 0;            //�ر�IAP����
    IAP_CMD = 0;              //�������Ĵ���
    IAP_TRIG = 0;              //��������Ĵ���
    IAP_ADDRH = 0x80;          //����ַ���õ���IAP����
    IAP_ADDRL = 0;
}

/*************************************************************
��������IapReadByte
��������ȡEEPROM����
������u16 addr,��ַ
����ֵ��u8 dat ,����
���ߣ�STC
***************************************************************/
u8 IapReadByte(u16 addr)
{
    u8 dat;
    IAP_CONTR = ENABLE_IAP;
    IAP_CMD = CMD_READ;
    IAP_ADDRL = addr;
    IAP_ADDRH = addr >> 8;
    IAP_TRIG = 0x5a;
    IAP_TRIG = 0xa5;
    delay_us(1);
    dat = IAP_DATA;
    IapIdle();
    
    return dat;
}

/*************************************************************
��������IapProgramByte
������д��EEPROM����
������u16 addr,��ַ��u8 dat ,����
����ֵ����
���ߣ�STC
***************************************************************/
void IapProgramByte(u16 addr, u8 dat)
{
    IAP_CONTR = ENABLE_IAP;
    IAP_CMD = CMD_PROGRAM;
    IAP_ADDRL = addr;
    IAP_ADDRH = addr >> 8;
    IAP_DATA = dat;
    IAP_TRIG = 0x5a;
    IAP_TRIG = 0xa5;
    delay_us(1);    
    IapIdle();
    
}

/*************************************************************
��������IapEraseSector
������EEPROM����ɾ��
������u16 addr,��ַ
����ֵ����
���ߣ�STC
***************************************************************/
void IapEraseSector(u16 addr)
{
    IAP_CONTR = ENABLE_IAP;
    IAP_CMD = CMD_ERASE;
    IAP_ADDRL = addr;
    IAP_ADDRH = addr >> 8;
    IAP_TRIG = 0x5a;
    IAP_TRIG = 0xa5;
    delay_us(1);    
    IapIdle();
}
    
void IapWrite_Opening0_adc(u16 adc_data)
{

    u16 addr = ADDR_OPENING_0;
    u8 i;
    u8 datatemp[8];
    
    datatemp[1] = adc_data; //��8λ��ֵ
    datatemp[0] = adc_data>>8; //��8λ��ֵ 
    for(i=2;i<8;i++)
    { 
        datatemp[i]= IapReadByte(addr+i);
    }
    
    
    IapEraseSector(ADDR_OPENING_0);
    for(i=0;i<8;i++)
    {
        IapProgramByte(addr+i, datatemp[i]);
    }
   
}

void IapWrite_Sen(u16 sen_data)
{
    u16 addr = ADDR_OPENING_0;
    u8 i;
    u8 datatemp[8];

    datatemp[0] = IapReadByte(ADDR_OPENING_0);
    datatemp[1] = IapReadByte(ADDR_OPENING_0+1);
    datatemp[3] = (u8)sen_data;
    datatemp[2] = (u8)(sen_data>>8);
    for(i=4;i<8;i++)
    {
       datatemp[i] = IapReadByte(addr+i);    
    }

    IapEraseSector(0);
    for(i=0;i<8;i++)
    {
        IapProgramByte(addr+i, datatemp[i]);     
    }
                       
}

void IapWrite_R_in(float res_data)
{
    u16 addr = ADDR_OPENING_0;
    u8 i;
    u8 datatemp[8];
    union
    {
        float Res;
        u8 R_data[4];
    }F32_data;
    
    F32_data.Res = res_data;
    
    for(i=0;i<8;i++)
    {
        if(i>=4)
        {
            datatemp[i] = F32_data.R_data[i-4];
        }            
        else 
        {
            datatemp[i] = IapReadByte(addr+i);
        }
    }
    
    IapEraseSector(0);
    
    for(i=0;i<8;i++)
    {
        IapProgramByte(addr+i, datatemp[i]);
    }

}

void IapWrite_Opening100_adc(u16 adc_data)
{

    u16 addr = ADDR_OPENING_100;
    u8 i;
    u8 datatemp[7];
    
    for(i=2;i<7;i++)
    {
        datatemp[i] = IapReadByte(i+addr);
    }
    datatemp[1] = adc_data;
    datatemp[0] = adc_data>>8;
    
    IapEraseSector(ADDR_OPENING_100);
    for(i=0;i<7;i++)
    {
        IapProgramByte(addr+i, datatemp[i]);
    }

  
}


void IapWrite_R_out(float res_data)
{
    u16 addr = ADDR_OPENING_100;
    u8 i;
    u8 datatemp[7];
    union
    {
        float Res;
        u8 R_data[4];
    }F32_data;
    F32_data.Res = res_data;
    
    for(i=0;i<7;i++)
    {
        if(i>=2 && i<6)
        {
            datatemp[i] = F32_data.R_data[i-2];
        }
        else
        {
            datatemp[i]= IapReadByte(i+addr);
        }
    }
    
    IapEraseSector(addr);
    for(i=0;i<7;i++)
    {
        IapProgramByte(i+addr,datatemp[i]);
    }

}

void IapWrite_InitFlag()
{
    u16 addr = ADDR_OPENING_100;
    u8 i;
    u8 datatemp[6];

    for(i=0;i<6;i++)
    {      
        datatemp[i]= IapReadByte(i+addr);
    }
    
    IapEraseSector(addr);
    
    for(i=0;i<6;i++)
    {
        IapProgramByte(i+addr,datatemp[i]);
    }
    IapProgramByte(addr+6,0xAA);
}

u16 IapRead_Sen()
{
    u16 sen_tmp;
    u8 tmph;
    u8 tmpl;
    tmph = IapReadByte(ADDR_SEN);
    tmpl = IapReadByte(ADDR_SEN+1);
    sen_tmp = (u16)tmph;
    sen_tmp <<= 8 ;
    sen_tmp |= (u16)tmpl;
    return sen_tmp;  
}

u16 IapRead_Opening0_adc()
{
    u16 tmp;
    u8  tmph;
    u8  tmpl;
    
     tmph = IapReadByte(ADDR_OPENING_0);
     tmpl = IapReadByte(ADDR_OPENING_0+1);
     tmp = (u16)tmph;
     tmp <<= 8;
     tmp |= (u16)tmpl;
    
    return tmp;
    
}

u16 IapRead_Opening100_adc()
{
    u16 tmp;
    u8  tmph;
    u8  tmpl;
    
     tmph = IapReadByte(ADDR_OPENING_100);
     tmpl = IapReadByte(ADDR_OPENING_100+1);
     tmp = (u16)tmph;
     tmp <<= 8;
     tmp |= (u16)tmpl;
    
    return tmp;
    
}

float IapRead_R_IN()
{
    float tmp;
     union
     {
        float Res;
        u8 R_data[4];
     }F32_data;
    
     F32_data.R_data[0] = IapReadByte(ADDR_R_IN);
     F32_data.R_data[1] = IapReadByte(ADDR_R_IN+1);
     F32_data.R_data[2] = IapReadByte(ADDR_R_IN+2);
     F32_data.R_data[3] = IapReadByte(ADDR_R_IN+3);
     tmp = F32_data.Res;
     
     return tmp;
    
}

float IapRead_R_OUT()
{
    float tmp;
     union
     {
        float Res;
        u8 R_data[4];
     }F32_data;
    
     F32_data.R_data[0] = IapReadByte(ADDR_R_OUT);
     F32_data.R_data[1] = IapReadByte(ADDR_R_OUT+1);
     F32_data.R_data[2] = IapReadByte(ADDR_R_OUT+2);
     F32_data.R_data[3] = IapReadByte(ADDR_R_OUT+3);
     tmp = F32_data.Res;
     
     return tmp;
    
}

u8 IapRead_InitFlag()
{
    u8 temp;
    temp = IapReadByte(ADDR_OPENING_100+6);
    return temp; 
}

//void IapReadData()
//{
//     u8 opening0h,opening0l;
//     u8 opening100h,opening100l;
//     u8 senh,senl;
//     u8 i;
//     u8 datatemp1[8];
//     u8 datatemp2[6];
//     u16 addr1 = ADDR_OPENING_0;
//     u16 addr2 = ADDR_OPENING_100;
//     union
//     {
//        float Res;
//        u8 R_data[4];
//     }F32_data;
//    

//    /*��ȡ����1������*/
//    for(i=0;i<8;i++)
//    {
//        datatemp1[i] =  IapReadByte(addr1+i);
//    }


//    opening0h = datatemp1[0];
//    opening0l = datatemp1[1];
//    senh =  datatemp1[2];
//    senl =  datatemp1[3];
//     

//    /*��������ֵ*/
//    opening0_adc = (u16)opening0h;  //����opening0_adc
//    opening0_adc <<= 8;
//    opening0_adc |= (u16)opening0l;


//    sen = (u16)senh;      //����������
//    sen <<= 8;
//    sen |= (u16)senl;

//    for(i=0;i<4;i++)         //����R_in
//    {
//        F32_data.R_data[i] = datatemp1[i+4];
//    }
//    R_in = F32_data.Res;
//    
//    
//    
//    /*��ȡ����2������*/
//    for(i=0;i<6;i++)
//    {
//        datatemp2[i] =  IapReadByte(addr2+i);
//    }
//    opening100h = datatemp2[0];
//    opening100l = datatemp2[1]; 

//    /*��������ֵ*/

//    opening100_adc = (u16)opening100h;  //����opening100_adc
//    opening100_adc <<= 8;
//    opening100_adc |= (u16)opening100l;  
//    for(i=0;i<4;i++)         //����R_out
//    {
//        F32_data.R_data[i] = datatemp2[i+2];
//    }
//    R_out = F32_data.Res;

//}
