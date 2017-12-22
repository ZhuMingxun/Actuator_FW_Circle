#ifndef __MLX90316_H
#define __MLX90316_H

#include "spi.h"
#include "delay.h"
#include <stdio.h>
#include "USART.h"

void UART1_Config(void);
u16 MLX90316_ReadData();
void MLX90316_SPI_Config();


#endif