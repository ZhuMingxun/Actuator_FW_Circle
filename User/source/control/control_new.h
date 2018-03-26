#ifndef _CONTROL_H_
#define _CONTROL_H_



#define MOM_ON  0
#define LIM_ON  0

#define TORQUE_ON 0

typedef enum
{
    MODE_DISTANT_ANALOG = 0,
    MODE_LOCAL             ,              
    MODE_DISTANT_DIGITAL   ,  
    MODE_LACK_PHASE        ,     
    MODE_DEF_ZERO          ,       
    MODE_DEF_HUNDRED       ,    
    MODE_OUT_CAL           ,       
    MODE_IN_CAL            ,        
    MODE_SET_SEN           ,
	MODE_CAL_LOW			,
	MODE_CAL_HIGH			,
	MODE_SETINPUT_LOW		,
	MODE_SETINPUT_HIGH
    
}Mode_Type;    
extern volatile Mode_Type mode; 

#define DISTANT         (LOCAL != LOCAL_ON && LOCAL_STOP != LOCAL_STOP_ON)
#define T2_MS           20  //20ms 50Hz
#define T2_CNT          (65536UL - ((T2_MS*MAIN_Fosc)/12/1000))

void Timer2_Config();


extern bit  timer2_20ms_flag;
extern bit  timer2_60ms_flag;
extern bit  timer2_500ms_flag;
extern bit  timer2_200ms_flag;
extern bit  timer2_100ms_flag;
extern bit  timer2_400ms_flag;
extern bit  timer_1s_flag;


#endif