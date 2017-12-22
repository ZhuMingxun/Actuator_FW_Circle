#include <config.h>

#include "IORedefine.h"
#include "LCD.h"



void main()
{
    LCD_Init();
    HT1621B_Init();
    LCD_TEST();        
    while(1);
    
   
}