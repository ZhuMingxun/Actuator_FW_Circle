#ifndef _IR_H
#define _IR_H


#include "PCA.h"
#include "ringbuffer.h"
#define IR_COMMAND_100      0x0d04
#define IR_COMMAND_0        0x0d02
#define IR_COMMAND_OPEN     0x0d20
#define IR_COMMAND_CLOSE    0x0d10
#define IR_COMMAND_STOP     0x0c84
#define IR_COMMAND_4        0x0d08
#define IR_COMMAND_0_100    (IR_COMMAND_0 | IR_COMMAND_100)
#define IR_COMMAND_4_0      (IR_COMMAND_4 | IR_COMMAND_0)
#define IR_COMMAND_4_100    (IR_COMMAND_4 | IR_COMMAND_100)   
#define IR_COMMAND_4_OPEN   (IR_COMMAND_4 | IR_COMMAND_OPEN)
#define IR_COMMAND_4_CLOSE  (IR_COMMAND_4 | IR_COMMAND_CLOSE)




void IR_PCA_Config();
void IR_Disable();
void IR_RingBuffer_Init();
u8 IRisReadyToRead();
void IRCmd_To_LocalCmd();
//void IR_Test();
void IR_Update();
extern u16 data ircmd;
extern RingBuffer_TypeDef  IR_RingBuffer;









#endif


