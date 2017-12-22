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
	signed char close_dir;//�ط���-1�ݼ���+1����
    u16 adcvalue_valvelow;
    u16 adcvalue_valvehigh;
    u16 sen;//������
    float res_input;
    float res_output;
    
}SystemParameter_TypeDef;//ϵͳ�����ṹ��
extern SystemParameter_TypeDef* pSystemParam;
void SystemParam_Init();
void ReadSystemPara();

typedef struct
{
    float adcvalue_refer_filtered;//�ο���ѹADC�˲�ֵ
    float adcvalue_input_filtered;//�����ѹADC�˲�ֵ
    float adcvalue_valve_filtered;//��λ��ѹADC�˲�ֵ
    float adcvalue_encoder_filtered;//������ADC�˲�ֵ
    
    u16 adcvalue_refer;//�ο���ѹADC����ֵ
    u16 adcvalue_input;//�����ѹADC����ֵ
    u16 adcvalue_valve;//��λ��ѹADC����ֵ
    u16 adcvalue_encoder;//������ADC����ֵ
    
}DataADC_TypeDef;
extern DataADC_TypeDef* pData_ADC;

typedef struct
{
    float   voltage_vcc;//VCC��Դ��ѹ
    float   voltage_valve;//��λʵʱ��ѹ
    float   voltage_input;//��������ɼ��ĵ�ѹ
    
}DataVoltage_TypeDef;
extern DataVoltage_TypeDef* pData_Voltage;

typedef struct
{
    float   current_input;//�������
    float   opening;//ʵʱ����
    float   current_output;//�������
    float   motor_vel;//�����ٶ�
    
}DataAcquire_TypeDef;//���ݲɼ��ṹ��
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