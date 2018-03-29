#include "ADC.h"
#include "IORedefine.h"
#include "delay.h"
#include "data_input.h"
#include "data_save.h"
#include "LCD.h"
#include "control_new.h"
#include "current_output.h"
#include "butterworth_filter.h"
#include "mlx90316.h"
#include <math.h>
#include "local.h"
#include "distant_analog.h"
/*************************************************************
                          变量定义
**************************************************************/

SystemParameter_TypeDef idata SystemParam;
SystemParameter_TypeDef* pSystemParam = &SystemParam;

DataADC_TypeDef xdata Data_ADC;
DataADC_TypeDef* pData_ADC = &Data_ADC;

DataVoltage_TypeDef  data Data_Voltage;
DataVoltage_TypeDef* pData_Voltage = &Data_Voltage;

DataAcquire_TypeDef  idata Data_Acquire;
DataAcquire_TypeDef* pData_Acquire = &Data_Acquire;

u8 use_encoder_flag = 1;//编码器使用标志


void ADC_Config()
{
    ADC_InitTypeDef  ADC_InitStructure;
    ADC_InitStructure.ADC_Px = ADC_P10 | ADC_P11 | ADC_P12; 
	ADC_InitStructure.ADC_Speed = ADC_90T;
    ADC_InitStructure.ADC_Power = ENABLE;
    ADC_InitStructure.ADC_AdjResult = ADC_RES_H8L2;
    ADC_InitStructure.ADC_Polity = PolityHigh;
    ADC_InitStructure.ADC_Interrupt = DISABLE;
    ADC_Inilize(&ADC_InitStructure);
    ADC_PowerControl(ENABLE);	
}


u16 Get_Vx_adc(u8 Vx_CH)  //获取某一通道的ADC采样值
{
    u16 adctemp;

    adctemp = Get_ADC10bitResult(Vx_CH);

    return adctemp;      
  
}

//系统参数初始化
void SystemParam_Init()
{
    u8 Initflag = IapRead_InitFlag();
    if(Initflag==INIT_FLAG)
    {
        ReadSystemPara();  
    }
    else
    {
	    pSystemParam->close_dir = -1;
        pSystemParam->adcvalue_valvelow = 255;
        pSystemParam->adcvalue_valvehigh = 800;
        pSystemParam->sen = SEN0_5MA;
        pSystemParam->res_input = 200.0;
        pSystemParam->res_output = 200.0;

        
		pSystemParam->cal_low = 400;
		pSystemParam->cal_high = 2000;
        
		pPosCTR->offset_open_filter = 10.0f;
		pPosCTR->offset_close_filter = -10.0f; 
		
        pSystemParam->setinput_low = 164;
		pSystemParam->setinput_high = 819;
        
        pSystemParam->margin = 1.6;
        
		IapWrite_CloseDir(-1);
        IapWrite_Opening0_adc(255);
        IapWrite_Opening100_adc(800);
        IapWrite_Sen(SEN0_5MA);
        IapWrite_R_in(200.0);
        IapWrite_R_out(200.0);
        IapWrite_InitFlag();
		IapWrite_OffsetClose(-10.0f);
		IapWrite_OffsetOpen(10.0f);
        
		IapWrite_CalLow(400);
		IapWrite_CalHigh(2000);
		
		IapWrite_SetInputLow(164);
		IapWrite_SetInputHigh(819);
        
        IapWrite_Margin(1.6);
    }
}
//读取Flash中的系统参数
void ReadSystemPara()
{
    pSystemParam->close_dir = IapRead_CloseDir();
	pSystemParam->adcvalue_valvelow = IapRead_Opening0_adc();
    pSystemParam->adcvalue_valvehigh = IapRead_Opening100_adc();
    pSystemParam->sen = IapRead_Sen();
    pSystemParam->res_input = IapRead_R_IN();
    pSystemParam->res_output = IapRead_R_OUT();  
    pPosCTR->offset_open_filter = IapRead_OffsetOpen();
	pPosCTR->offset_close_filter = IapRead_OffsetClose();
	
	pSystemParam->cal_low = IapRead_CalLow();
	pSystemParam->cal_high = IapRead_CalHigh();
    
	pSystemParam->setinput_low = IapRead_SetInputLow();
	pSystemParam->setinput_high = IapRead_SetInputHigh();
    
    pSystemParam->margin = IapRead_Margin();
    
	if(pSystemParam->setinput_low<50 || pSystemParam->setinput_low>300)
		pSystemParam->setinput_low = 164;
	
	if(pSystemParam->setinput_high<700 || pSystemParam->setinput_high>950)
		pSystemParam->setinput_high = 819;
    
	if(pSystemParam->cal_low>2500)
		pSystemParam->cal_low = 400;
	
	if(pSystemParam->cal_high>2500)
		pSystemParam->cal_high = 2000;
    
//	if(pPosCTR->offset_open_filter>10 || pPosCTR->offset_open_filter<0)
//	{
//		pPosCTR->offset_open_filter = 10;
//		IapWrite_OffsetOpen(10.0f);
//	}
//	if(pPosCTR->offset_close_filter<-10 || pPosCTR->offset_close_filter>0)
//	{
//		pPosCTR->offset_close_filter = -10;
//		IapWrite_OffsetClose(-10.0f);
//	}
}


//float   Get_Vcc(const DataADC_TypeDef* padc)
//{
//    float vcc_tmp;
//    
//    vcc_tmp = (float)(VREF * 1024.0) / (float)(padc->adcvalue_refer_filtered);
//    return vcc_tmp;
//    
//}
float   Get_Va(const DataADC_TypeDef* padc,const DataVoltage_TypeDef* pvoltage)
{
    float va_tmp;
        
    va_tmp =  (pvoltage->voltage_vcc * padc->adcvalue_valve_filtered) / 1024.0; 
    return va_tmp;
    
}
//float   Get_Vi(const DataADC_TypeDef* padc, const DataVoltage_TypeDef* pvoltage)
//{
//    float vi_tmp;

//    vi_tmp = (pvoltage->voltage_vcc * padc->adcvalue_input_filtered) / 1024.0;
//    return vi_tmp;
//}


//float Get_InputCurrent(const DataVoltage_TypeDef* pvoltage,const SystemParameter_TypeDef* psystem)
//{
//    float current_temp;
//    
//    current_temp = (pvoltage->voltage_input * 1000.0 / psystem->res_input); // xx.xxxxx mA
//    if(current_temp<1.0)    return 0.0;
//    else return current_temp; 
//}

//计算实时阀门开度
extern signed char zero_flag;
float min_opening = 0.0;
float Get_RealimeOpening(const DataADC_TypeDef*padc,const SystemParameter_TypeDef* psystem)
{
    float opening_tmp;
    float x1,x2,x;
	  
		//判断行程过零
		if((int)(pSystemParam->adcvalue_valvelow - pSystemParam->adcvalue_valvehigh)*(pSystemParam->close_dir)>0)
			zero_flag = -1;
		if((int)(pSystemParam->adcvalue_valvelow - pSystemParam->adcvalue_valvehigh)*(pSystemParam->close_dir)<0)
			zero_flag = 1;
	
    if(use_encoder_flag)
    {
			if(zero_flag==-1)//不过零
			{
				x1 = (float)(psystem->adcvalue_valvelow);
				x2 = (float)(psystem->adcvalue_valvehigh);
				x = (float)(padc->adcvalue_encoder);
				if((x1-x)*(x1-x2)<=0)   return 0.0;
				if((x2-x)*(x2-x1)<=0)   return 1.0;
				opening_tmp = (x-x1) / (x2-x1);
				min_opening = 1/fabs(x2-x1);
				return opening_tmp;
			}
			else//过零
			{
				x1 = (float)(psystem->adcvalue_valvelow);
				x2 = (float)(psystem->adcvalue_valvehigh);
				x = (float)(padc->adcvalue_encoder);
				
				if(x1 < x2)//L<H
				{
					min_opening = 1/fabs((float)(16383-(x2-x1)));
					
					if(x<x1)
					{
						opening_tmp = (x1-x)/(float)(16383-(x2-x1));
						return opening_tmp;
					}
					if(x>x2)
					{
						opening_tmp = (16383-(x-x1))/(float)(16383-(x2-x1));
						return opening_tmp;
					}
					if(fabs(x-x1)<fabs(x-x2))
						return 0.0;
					else
						return 1.0;
				}
				else//L>H
				{
					min_opening = 1/fabs((float)(16383-(x1-x2)));
					if(x<x2)
					{
						opening_tmp = (16383.0f-(x1-x))/(16383.0f-(x1-x2));
						return opening_tmp;
						
					}
					if(x>x1)
					{
						opening_tmp = (x-x1)/(16383.0f-(x1-x2));
						return opening_tmp;
					}
					if(fabs(x-x1)<fabs(x-x2))
						return 0.0;
					else
						return 1.0;
					
				}
			}
    }
    else
    {
		min_opening = 1/fabs(psystem->adcvalue_valvelow - psystem->adcvalue_valvehigh);
        x1 = (float)(psystem->adcvalue_valvelow);
        x2 = (float)(psystem->adcvalue_valvehigh);
        x = (float)(padc->adcvalue_valve_filtered);
        if((x1-x)*(x1-x2)<=0)   return 0.0;
        if((x2-x)*(x2-x1)<=0)   return 1.0;
        opening_tmp = (x-x1) / (x2-x1);
        
        opening_tmp = (float)((u8)(opening_tmp*100))/100.0;//保留两位小数
        
        return opening_tmp;
    }
    

}
float Get_OutputCurrent(const DataAcquire_TypeDef* pacquire)
{
    float current_temp;
    
    current_temp = 16.0 * pacquire->opening + 4.0;
    return current_temp;
    
}
void Acquire_Data()
{
    //pData_ADC->adcvalue_refer = Get_Vx_adc(VREF_CH);
    pData_ADC->adcvalue_refer = 515;
    //pData_ADC->adcvalue_valve = Get_Vx_adc(VA_CH);
    pData_ADC->adcvalue_encoder = MLX90316_ReadData();
    
    //pData_ADC->adcvalue_refer_filtered = Butterworth_Filter_Refer((float)pData_ADC->adcvalue_refer);
    pData_ADC->adcvalue_refer_filtered = 515.0;
    pData_ADC->adcvalue_valve_filtered = Butterworth_Filter_Valve((float)pData_ADC->adcvalue_valve);
    pData_ADC->adcvalue_encoder_filtered = Butterworth_Filter_Encoder((float)pData_ADC->adcvalue_encoder);

    
    //pData_Voltage->voltage_vcc   =  Get_Vcc(pData_ADC);
    pData_Voltage->voltage_vcc  = 5.0;
    pData_Voltage->voltage_valve =  Get_Va(pData_ADC,pData_Voltage);

    pData_Acquire->opening        =  Get_RealimeOpening(pData_ADC,pSystemParam);
    pData_Acquire->current_output =  Get_OutputCurrent(pData_Acquire);
    
}





void Update_InputCurrent()
{
    static float cur_in;
    static float pre_in;
    static u8 cnt = 0;
    static u8 firstflag = 1;
    float k;
    
    pData_ADC->adcvalue_input = Get_Vx_adc(VI_CH);
    pData_ADC->adcvalue_input_filtered = Butterworth_Filter_Input((float)pData_ADC->adcvalue_input);
//    pData_Voltage->voltage_input =  Get_Vi(pData_ADC,pData_Voltage);
//    cur_in  =  Get_InputCurrent(pData_Voltage,pSystemParam);

    k = 16.0/((float)(pSystemParam->setinput_high-pSystemParam->setinput_low));
    cur_in = k*(pData_ADC->adcvalue_input_filtered-pSystemParam->setinput_low)+4.0;
    
    
    if(firstflag==1)
    {
        pData_Acquire->current_input = pre_in = cur_in;
        firstflag = 0;
    }
    else
    {
        if(fabs(cur_in-pre_in)>=0.16)
        {
            cnt++;
            if(cnt>50)
            {
                pData_Acquire->current_input = cur_in;
                pre_in = cur_in;
                cnt = 0;
            }
        }
        else
        {
            pData_Acquire->current_input = pre_in;
            cnt = 0;
        }              
        
    }
   
}


void Detect_ValveInput()
{
    u8 i;
    u8 errcnt = 0;
    u16 adc_data;
    
    for(i=0;i<8;i++)
    {
        delay_ms(10);
        adc_data =  MLX90316_ReadData();
        if(adc_data == 0xC000)  errcnt++;
    }
    
    if(errcnt>=5)   use_encoder_flag = 0;
    else use_encoder_flag = 1;
  
}


void DataSampInit()
{
    u8 i;
    for(i=0;i<100;i++)
    {
	    delay_ms(20);
        Acquire_Data();
        Update_InputCurrent(); 
        #ifdef WATCH_DOG
        WDT_CONTR = WATCH_DOG_RSTVAL;
        #endif
    }
    
    
}






