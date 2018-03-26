
#include "main.h"
#include "uart_debug.h"
#include "actuator_config.h"
void main()
{
	delay_ms(500);	
	delay_ms(500);	
	delay_ms(500);	
    LCD_Init();
    
    Motor_Config();
    
    Timer2_Config();
    
    ADC_Config(); 

    MLX90316_SPI_Config();

    PWMIO_Config();
    Timer0_Config();
    
#ifdef PHASE_SEQ
    PhaseSeq_Detect_Config();
#endif
    
    Exti23_Config();

    SystemParam_Init();
		
    SystemMode_Detect();
    UART_config();
    
    Detect_ValveInput();
    DataSampInit();
    //PosControl_Init();
    
    while(1)
    {
        switch(mode)
        {
            case MODE_DISTANT_ANALOG:   {DistantAnalog_Mode();  break;}
            case MODE_LOCAL:            {Local_Mode();          break;}
            case MODE_DISTANT_DIGITAL:  {DistantDigital_Mode(); break;}
            case MODE_DEF_ZERO:         {DefZero_Mode();        break;}
            case MODE_DEF_HUNDRED:      {DefHundred_Mode();     break;}
            case MODE_SET_SEN:          {SetSen_Mode();         break;}
            case MODE_CAL_LOW:			{CalLow_Mode();break;}
			case MODE_CAL_HIGH:			{CalHigh_Mode();break;}
			case MODE_SETINPUT_LOW:  	{SetInputLow_Mode();break;}
			case MODE_SETINPUT_HIGH: 	{SetInputHigh_Mode();break;}	
			
#ifdef PHASE_SEQ
            case MODE_LACK_PHASE:       {LackPhase_Mode();break;}
#endif
            default:break;
   
        }    
          
     
    }
  

}
       
        
