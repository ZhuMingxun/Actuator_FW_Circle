#include "ringbuffer.h"



void RingBuffer_Init(RingBuffer_TypeDef* pRingBuffer, u8* buff, u8 length)
{
    pRingBuffer->pBuff = buff;
	pRingBuffer->length = length;
	pRingBuffer->write_pos = 0;
	pRingBuffer->read_pos = 0;
	pRingBuffer->fullflag = 0;
	//pRingBuffer->emptyflag = 0;	
}

void RingBuffer_Push(RingBuffer_TypeDef* pRingBuffer,u8 value)
{
#ifdef POWER_TWO
        u8 write_pos_next = (pRingBuffer->write_pos + 1)&(pRingBuffer->length - 1);
    #else
        u8 write_pos_next = (pRingBuffer->write_pos + 1)%(pRingBuffer->length);
#endif
	
	if(pRingBuffer->read_pos != write_pos_next)
	{
		(pRingBuffer->pBuff)[pRingBuffer->write_pos] = value;
		pRingBuffer->write_pos = write_pos_next;	
	}
	else 
	{
		if(pRingBuffer->fullflag != 1)
            pRingBuffer->fullflag = 1;
	}

}

u8 RingBuffer_Pop(RingBuffer_TypeDef* pRingBuffer)
{
#ifdef POWER_TWO
        u8 read_pos_next = (pRingBuffer->read_pos + 1)&(pRingBuffer->length - 1);
    #else
        u8 read_pos_next = (pRingBuffer->read_pos + 1)%(pRingBuffer->length);
#endif
	u8 temp = 0;
	
	//if(pRingBuffer->fullflag == 1)	pRingBuffer->fullflag = 0;
	if(pRingBuffer->write_pos == pRingBuffer->read_pos)	return 0xff;// ringbuffer is empty
	 
	 temp = (pRingBuffer->pBuff)[pRingBuffer->read_pos];
	 pRingBuffer->read_pos = read_pos_next;
	return temp;
	
}

//void RingBuffer_Clear(RingBuffer_TypeDef* pRingBuffer)
//{
//	pRingBuffer->write_pos = 0;
//	pRingBuffer->read_pos = 0;
//	pRingBuffer->fullflag = 0;
//	
//}

u8 RingBufferIsEmpty(RingBuffer_TypeDef* pRingBuffer)
{
	if(pRingBuffer->write_pos == pRingBuffer->read_pos)
    return 1;
    else    return 0;
		
}

//u8 RingBufferIsFull(RingBuffer_TypeDef* pRingBuffer)
//{
//#ifdef POWER_TWO
//    return ((pRingBuffer->write_pos + 1) & (pRingBuffer->length-1)) == pRingBuffer->read_pos;
//    #else
//    return ((pRingBuffer->write_pos + 1) % (pRingBuffer->length)) == pRingBuffer->read_pos;
//#endif	
//}













