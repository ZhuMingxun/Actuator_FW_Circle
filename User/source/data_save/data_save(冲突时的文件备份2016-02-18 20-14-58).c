/*********************************************************************
文件名：data_save.c
作者：朱明勋
说明：写FLASH和读FLASH操作 ，用于保存开到位和关到位ADC采样值
主要函数： void IapIdle();  //关闭IAP
            u8 IapReadByte(u16 addr);    //从EEPROM中读数据
            void IapProgramByte(u16 add, u8 dat);  //编程一个字节
            void IapEraseSector(u16 addr);   //扇区擦除


*********************************************************************/
#include "config.h"
#include "data_save.h"
#include "delay.h"

#define     CMD_READ        1
#define     CMD_PROGRAM     2
#define     CMD_ERASE       3
#define     ENABLE_IAP      0x83

/************************************************************************
                              函数声明
*************************************************************************/
void IapIdle();
extern u16 opening0_adc;
extern u16 opening100_adc;
extern u16 sen;
extern float R_in;
extern float R_out;
/*************************************************************
函数名：IapIdle()
描述：关闭IAP
参数：无
返回值：无
作者：STC
***************************************************************/
void IapIdle()
{
    IAP_CONTR = 0;            //关闭IAP功能
    IAP_CMD = 0;              //清除命令寄存器
    IAP_TRIG = 0;              //清除触发寄存器
    IAP_ADDRH = 0x80;          //将地址设置到非IAP区域
    IAP_ADDRL = 0;
}

/*************************************************************
函数名：IapReadByte
描述：读取EEPROM数据
参数：u16 addr,地址
返回值：u8 dat ,数据
作者：STC
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
函数名：IapProgramByte
描述：写入EEPROM数据
参数：u16 addr,地址，u8 dat ,数据
返回值：无
作者：STC
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
函数名：IapEraseSector
描述：EEPROM扇区删除
参数：u16 addr,地址
返回值：无
作者：STC
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
    
    datatemp[1] = adc_data; //低8位赋值
    datatemp[0] = adc_data>>8; //高8位赋值 
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

//    /*读取扇区1的数据*/
//    for(i=0;i<8;i++)
//    {
//        datatemp1[i] =  IapReadByte(addr1+i);
//    }


//    opening0h = datatemp1[0];
//    opening0l = datatemp1[1];
//    senh =  datatemp1[2];
//    senl =  datatemp1[3];
//     

//    /*计算数据值*/
//    opening0_adc = (u16)opening0h;  //计算opening0_adc
//    opening0_adc <<= 8;
//    opening0_adc |= (u16)opening0l;


//    sen = (u16)senh;      //计算灵敏度
//    sen <<= 8;
//    sen |= (u16)senl;

//    for(i=0;i<4;i++)         //计算R_in
//    {
//        F32_data.R_data[i] = datatemp1[i+4];
//    }
//    R_in = F32_data.Res;
//    
//    
//    
//    /*读取扇区2的数据*/
//    for(i=0;i<6;i++)
//    {
//        datatemp2[i] =  IapReadByte(addr2+i);
//    }
//    opening100h = datatemp2[0];
//    opening100l = datatemp2[1]; 

//    /*计算数据值*/

//    opening100_adc = (u16)opening100h;  //计算opening100_adc
//    opening100_adc <<= 8;
//    opening100_adc |= (u16)opening100l;  
//    for(i=0;i<4;i++)         //计算R_out
//    {
//        F32_data.R_data[i] = datatemp2[i+2];
//    }
//    R_out = F32_data.Res;

//}
