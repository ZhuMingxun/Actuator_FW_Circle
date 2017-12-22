#ifndef _RINGBUFFER1_H
#define _RINGBUFFER1_H
#ifdef __cplusplus
 extern "C" {
#endif 

#include	"config.h"
//#include "stm32f4xx.h"
//#include <stdio.h>
#define POWER_TWO
     
typedef struct {
	u8* pBuff;
	u8 length;//2^n;
	u8 write_pos;
	u8 read_pos;
	u8 fullflag;
	//u8 emptyflag;
}RingBuffer_TypeDef;

void RingBuffer_Init(RingBuffer_TypeDef* pRingBuffer, u8* buff, u8 length); 
void RingBuffer_Push(RingBuffer_TypeDef* pRingBuffer,u8 value);
u8 RingBuffer_Pop(RingBuffer_TypeDef* pRingBuffer);
void RingBuffer_Clear(RingBuffer_TypeDef* pRingBuffer);
u8 RingBufferIsEmpty(RingBuffer_TypeDef* pRingBuffer);
u8 RingBufferIsFull(RingBuffer_TypeDef* pRingBuffer);

#ifdef __cplusplus
}
#endif
#endif

 



