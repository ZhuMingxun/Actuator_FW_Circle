#ifndef _DATA_SAVE_H_
#define _DATA_SAVE_H_



#define     ADDR_OPENING_0      0
#define     ADDR_SEN            (ADDR_OPENING_0+2)            
#define     ADDR_R_IN           (ADDR_SEN+2)
#define		ADDR_OFFSET_CLOSE	(ADDR_OPENING_0+8)
#define     ADDR_CLOSE_DIR		(ADDR_OFFSET_CLOSE+4)
#define    ADDR_MARGIN_ADA     ADDR_OFFSET_CLOSE//◊‘  ”¶¡È√Ù∂»


#define     ADDR_OPENING_100    512
#define     ADDR_R_OUT          (ADDR_OPENING_100+2)
#define		ADDR_OFFSET_OPEN	(ADDR_OPENING_100+7)
#define 	ADDR_CAL_LOW		(ADDR_OFFSET_OPEN+4)
#define 	ADDR_CAL_HIGH		(ADDR_CAL_LOW+2)
#define 	ADDR_INPUT_LOW		(ADDR_CAL_HIGH+2)
#define     ADDR_INPUT_HIGH	    (ADDR_INPUT_LOW+2)

#define     INIT_FLAG   		0x5A

void IapWrite_Opening0_adc(u16 adc_data);
void IapWrite_Opening100_adc(u16 adc_data);
void IapWrite_Sen(u16 sen_data);
void IapWrite_R_in(float res);
void IapWrite_R_out(float res);
void IapWrite_InitFlag();
void IapWrite_OffsetClose(float offset_c);
void IapWrite_CloseDir(signed char dir);
void IapWrite_OffsetOpen(float offset_o);
void IapWrite_CalLow(u16 cntlow);
void IapWrite_CalHigh(u16 cnthigh);
void IapWrite_Margin(float offset_c);


u16 IapRead_Sen();
u16 IapRead_Opening0_adc();
u16 IapRead_Opening100_adc();
float IapRead_R_IN();
float IapRead_R_OUT();
u8 IapRead_InitFlag();
float IapRead_OffsetOpen();
float IapRead_OffsetClose();
signed char IapRead_CloseDir();

u16 IapRead_CalLow();
u16 IapRead_CalHigh();
void IapWrite_SetInputLow(u16 val);
void IapWrite_SetInputHigh(u16 val);
u16 IapRead_SetInputLow();
u16 IapRead_SetInputHigh();
float IapRead_Margin();
#endif