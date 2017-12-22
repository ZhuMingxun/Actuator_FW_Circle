#ifndef _DATA_INPUT_H_
#define _DATA_INPUT_H_
//#include "config.h"


#define     VI_CH           0
#define     VA_CH           1
#define     VREF_CH         2

#define     VREF            2.4999

#define     LED_LSG_ON      {if(LCD_YELLOW != 0) LCD_YELLOW = 0;}
#define     LED_LSG_OFF     {if(LCD_YELLOW != 1) LCD_YELLOW = 1;}

#define     OPENING_INPUT      0
#define     OPENING_REALTIME   1

//#define     USE_ENCODER

typedef struct
{
	signed char close_dir;//关方向，-1递减，+1递增
    u16 adcvalue_valvelow;
    u16 adcvalue_valvehigh;
    u16 sen;//灵敏度
    float res_input;
    float res_output;
    
}SystemParameter_TypeDef;//系统参数结构体
extern SystemParameter_TypeDef* pSystemParam;
void SystemParam_Init();
void ReadSystemPara();

typedef struct
{
    float adcvalue_refer_filtered;//参考电压ADC滤波值
    float adcvalue_input_filtered;//输入电压ADC滤波值
    float adcvalue_valve_filtered;//阀位电压ADC滤波值
    float adcvalue_encoder_filtered;//编码器ADC滤波值
    
    u16 adcvalue_refer;//参考电压ADC采样值
    u16 adcvalue_input;//输入电压ADC采样值
    u16 adcvalue_valve;//阀位电压ADC采样值
    u16 adcvalue_encoder;//编码器ADC采样值
    
}DataADC_TypeDef;
extern DataADC_TypeDef* pData_ADC;

typedef struct
{
    float   voltage_vcc;//VCC电源电压
    float   voltage_valve;//阀位实时电压
    float   voltage_input;//输入电流采集的电压
    
}DataVoltage_TypeDef;
extern DataVoltage_TypeDef* pData_Voltage;

typedef struct
{
    float   current_input;//输入电流
    float   opening;//实时开度
    float   current_output;//输出电流
    float   motor_vel;//阀门速度
    
}DataAcquire_TypeDef;//数据采集结构体
extern DataAcquire_TypeDef* pData_Acquire;

extern u8 use_encoder_flag;
void  ADC_Config();
u16   Get_Vx_adc(u8 Vx_CH);
void Acquire_Data();
void Detect_ValveInput();
void Update_InputCurrent();
void DataSampInit();
//void Pos_Polar_Detect(u8* flag);
//void EncoderValue_Compare();

#endif