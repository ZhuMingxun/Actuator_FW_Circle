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
    u8 datatemp[20];

    for(i=0;i<20;i++)
    { 
        datatemp[i]= IapReadByte(addr+i);
    }
    datatemp[0] = (u8)(adc_data>>8); //高8位赋值 
    datatemp[1] = (u8)adc_data; //低8位赋值

		
    IapEraseSector(ADDR_OPENING_0);
    for(i=0;i<20;i++)
    {
        IapProgramByte(addr+i, datatemp[i]);
    }
   
}

void IapWrite_Sen(u16 sen_data)
{
    u16 addr = ADDR_OPENING_0;
    u8 i;
    u8 datatemp[20];

    for(i=0;i<20;i++)
    {
       datatemp[i] = IapReadByte(addr+i);    
    }
    datatemp[2] = (u8)(sen_data>>8);
    datatemp[3] = (u8)sen_data;

    IapEraseSector(0);
    for(i=0;i<20;i++)
    {
        IapProgramByte(addr+i, datatemp[i]);     
    }
                       
}

void IapWrite_R_in(float res_data)
{
    u16 addr = ADDR_OPENING_0;
    u8 i;
    u8 datatemp[20];
    union
    {
        float Res;
        u8 R_data[4];
    }F32_data;
    
    F32_data.Res = res_data;
		
    for(i=0;i<20;i++)
    {
       datatemp[i] = IapReadByte(addr+i);    
    }
    datatemp[4] = F32_data.R_data[0];
		datatemp[5] = F32_data.R_data[1];
		datatemp[6] = F32_data.R_data[2];
		datatemp[7] = F32_data.R_data[3];

    IapEraseSector(0);
    
    for(i=0;i<20;i++)
    {
        IapProgramByte(addr+i, datatemp[i]);
    }

}

void IapWrite_OffsetClose(float offset_c)
{
    u16 addr = ADDR_OPENING_0;
    u8 i;
    u8 datatemp[20];
	  union
    {
        float Res;
        u8 R_data[4];
    }F32_data;
		
		F32_data.Res = offset_c;
		
    for(i=0;i<20;i++)
    {      
        datatemp[i]= IapReadByte(i+addr);
    }
		datatemp[8] = F32_data.R_data[0];
    datatemp[9] = F32_data.R_data[1];
		datatemp[10] = F32_data.R_data[2];
		datatemp[11] = F32_data.R_data[3];
		
    IapEraseSector(addr);
    
    for(i=0;i<20;i++)
    {
        IapProgramByte(i+addr,datatemp[i]);
    }

}

void IapWrite_Margin(float offset_c)
{
    u16 addr = ADDR_OPENING_0;
    u8 i;
    u8 datatemp[20];
	  union
    {
        float Res;
        u8 R_data[4];
    }F32_data;
		
		F32_data.Res = offset_c;
		
    for(i=0;i<20;i++)
    {      
        datatemp[i]= IapReadByte(i+addr);
    }
    datatemp[8] = F32_data.R_data[0];
    datatemp[9] = F32_data.R_data[1];
    datatemp[10] = F32_data.R_data[2];
    datatemp[11] = F32_data.R_data[3];
		
    IapEraseSector(addr);
    
    for(i=0;i<20;i++)
    {
        IapProgramByte(i+addr,datatemp[i]);
    }

}

void IapWrite_CloseDir(signed char dir)
{
    u16 addr = ADDR_OPENING_0;
    u8 i;
    u8 datatemp[20];
	
	  //读数据
    for(i=0;i<20;i++)
    {      
        datatemp[i]= IapReadByte(i+addr);
    }
		datatemp[12] = (u8)dir;
		
		IapEraseSector(addr);
		
    for(i=0;i<20;i++)
    {
        IapProgramByte(i+addr,datatemp[i]);
    }
}




//=============================================================
//===========================512===============================
//=============================================================
void IapWrite_Opening100_adc(u16 adc_data)
{

    u16 addr = ADDR_OPENING_100;
    u8 i;
    u8 datatemp[20];
    
    for(i=0;i<20;i++)
    {
        datatemp[i] = IapReadByte(i+addr);
    }
    datatemp[1] = adc_data;
    datatemp[0] = adc_data>>8;
    
    IapEraseSector(ADDR_OPENING_100);
    for(i=0;i<20;i++)
    {
        IapProgramByte(addr+i, datatemp[i]);
    }

  
}


void IapWrite_R_out(float res_data)
{
    u16 addr = ADDR_OPENING_100;
    u8 i;
    u8 datatemp[20];
    union
    {
        float Res;
        u8 R_data[4];
    }F32_data;
    F32_data.Res = res_data;
    
    for(i=0;i<20;i++)
    {
			datatemp[i]= IapReadByte(i+addr);
    }
		datatemp[2] = F32_data.R_data[0];
    datatemp[3] = F32_data.R_data[1];
		datatemp[4] = F32_data.R_data[2];
		datatemp[5] = F32_data.R_data[3];
		
    IapEraseSector(addr);
    for(i=0;i<20;i++)
    {
        IapProgramByte(i+addr,datatemp[i]);
    }

}

void IapWrite_InitFlag()
{
    u16 addr = ADDR_OPENING_100;
    u8 i;
    u8 datatemp[20];

    for(i=0;i<20;i++)
    {      
        datatemp[i]= IapReadByte(i+addr);
    }
		datatemp[6] = INIT_FLAG;
    
    IapEraseSector(addr);
    
    for(i=0;i<20;i++)
    {
        IapProgramByte(i+addr,datatemp[i]);
    }

}


void IapWrite_OffsetOpen(float offset_o)
{
    u16 addr = ADDR_OPENING_100;
    u8 i;
    u8 datatemp[20];
	  union
    {
        float Res;
        u8 R_data[4];
    }F32_data;
		
		F32_data.Res = offset_o;
		
    for(i=0;i<20;i++)
    {      
        datatemp[i]= IapReadByte(i+addr);
    }
		datatemp[7] = F32_data.R_data[0];
    datatemp[8] = F32_data.R_data[1];
		datatemp[9] = F32_data.R_data[2];
		datatemp[10] = F32_data.R_data[3];
		
    IapEraseSector(addr);
    
    for(i=0;i<20;i++)
    {
        IapProgramByte(i+addr,datatemp[i]);
    }

}

void IapWrite_CalLow(u16 cntlow)
{
	u16 addr = ADDR_OPENING_100;
	u8 datatemp[20];
	u8 i;
	
	//read and save data first
    for(i=0;i<20;i++)
    {      
        datatemp[i]= IapReadByte(i+addr);
    }
	datatemp[11] = (u8)(cntlow>>8);
	datatemp[12] = (u8)(cntlow&0x00FF);
	
	IapEraseSector(addr);
	
    for(i=0;i<20;i++)
    {
        IapProgramByte(i+addr,datatemp[i]);
    }
	
}

void IapWrite_CalHigh(u16 cnthigh)
{
	u16 addr = ADDR_OPENING_100;
	u8 datatemp[20];
	u8 i;
	
	//read and save data first
    for(i=0;i<20;i++)
    {      
        datatemp[i]= IapReadByte(i+addr);
    }
	datatemp[13] = (u8)(cnthigh>>8);
	datatemp[14] = (u8)(cnthigh&0x00FF);
	
	IapEraseSector(addr);
	
    for(i=0;i<20;i++)
    {
        IapProgramByte(i+addr,datatemp[i]);
    }
	
}

void IapWrite_SetInputLow(u16 val)
{
	u16 addr = ADDR_OPENING_100;
	u8 datatemp[20];
	u8 i;

	//read and save data first
    for(i=0;i<20;i++)
    {      
        datatemp[i]= IapReadByte(i+addr);
    }
	datatemp[15] = (u8)(val>>8);
	datatemp[16] = (u8)(val&0x00FF);
	IapEraseSector(addr);
	
    for(i=0;i<20;i++)
    {
        IapProgramByte(i+addr,datatemp[i]);
    }
	
	
}
void IapWrite_SetInputHigh(u16 val)
{
	u16 addr = ADDR_OPENING_100;
	u8 datatemp[20];
	u8 i;

	//read and save data first
    for(i=0;i<20;i++)
    {      
        datatemp[i]= IapReadByte(i+addr);
    }
	datatemp[17] = (u8)(val>>8);
	datatemp[18] = (u8)(val&0x00FF);
	IapEraseSector(addr);
	
    for(i=0;i<20;i++)
    {
        IapProgramByte(i+addr,datatemp[i]);
    }

}

//=============================================
//================Read=========================
//=============================================
u16 IapRead_Sen()
{
    u16 sen_tmp;
    u8 tmph;
    u8 tmpl;
    tmph = IapReadByte(ADDR_SEN);
    tmpl = IapReadByte(ADDR_SEN+1);
    sen_tmp = (u16)tmph;
    sen_tmp <<= 8 ;
    sen_tmp |= ((u16)tmpl & 0x00FF);
    return sen_tmp;  
}

float IapRead_OffsetClose()
{
     union
     {
        float Res;
        u8 R_data[4];
     }F32_data;

     F32_data.R_data[0] = IapReadByte(ADDR_OFFSET_CLOSE);
     F32_data.R_data[1] = IapReadByte(ADDR_OFFSET_CLOSE+1);
     F32_data.R_data[2] = IapReadByte(ADDR_OFFSET_CLOSE+2);
     F32_data.R_data[3] = IapReadByte(ADDR_OFFSET_CLOSE+3);
		 
    return F32_data.Res;  
}

float IapRead_Margin()
{
     union
     {
        float Res;
        u8 R_data[4];
     }F32_data;

     F32_data.R_data[0] = IapReadByte(ADDR_OFFSET_CLOSE);
     F32_data.R_data[1] = IapReadByte(ADDR_OFFSET_CLOSE+1);
     F32_data.R_data[2] = IapReadByte(ADDR_OFFSET_CLOSE+2);
     F32_data.R_data[3] = IapReadByte(ADDR_OFFSET_CLOSE+3);
		 
    return F32_data.Res;  
}

signed char IapRead_CloseDir()
{
	signed char dir;
	
	dir = (signed char)IapReadByte(ADDR_CLOSE_DIR);
	return dir;
	
	
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

float IapRead_OffsetOpen()
{
     union
     {
        float Res;
        u8 R_data[4];
     }F32_data;
		 
     F32_data.R_data[0] = IapReadByte(ADDR_OFFSET_OPEN);
     F32_data.R_data[1] = IapReadByte(ADDR_OFFSET_OPEN+1);
     F32_data.R_data[2] = IapReadByte(ADDR_OFFSET_OPEN+2);
     F32_data.R_data[3] = IapReadByte(ADDR_OFFSET_OPEN+3);
	
    return F32_data.Res;  
}


u16 IapRead_CalLow()
{
	u8 tmpl;
	u8 tmph;
	u16 tmp;
	
	tmph = IapReadByte(ADDR_CAL_LOW);
	tmpl = IapReadByte(ADDR_CAL_LOW+1);
	
	tmp = (((u16)tmph<<8)&0xFF00) | (tmpl&0x00FF);
	
	return tmp;
}

u16 IapRead_CalHigh()
{
	u8 tmpl;
	u8 tmph;
	u16 tmp;
	
	tmph = IapReadByte(ADDR_CAL_HIGH);
	tmpl = IapReadByte(ADDR_CAL_HIGH+1);
	
	tmp = (((u16)tmph<<8)&0xFF00) | (tmpl&0x00FF);
	
	return tmp;
}

u16 IapRead_SetInputLow()
{
	u8 tmpl;
	u8 tmph;
	u16 tmp;
	
	tmph = IapReadByte(ADDR_INPUT_LOW);
	tmpl = IapReadByte(ADDR_INPUT_LOW+1);
	
	tmp = (((u16)tmph<<8)&0xFF00) | (tmpl&0x00FF);
	
	return tmp;
}

u16 IapRead_SetInputHigh()
{
	u8 tmpl;
	u8 tmph;
	u16 tmp;
	
	tmph = IapReadByte(ADDR_INPUT_HIGH);
	tmpl = IapReadByte(ADDR_INPUT_HIGH+1);
	
	tmp = (((u16)tmph<<8)&0xFF00) | (tmpl&0x00FF);
	
	return tmp;	
}
