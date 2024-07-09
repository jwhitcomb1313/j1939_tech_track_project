/*
 * uart.c
 *
 *  Created on: Jun 25, 2024
 *      Author: Justin Whitcomb
 */

#include "uart.h"
#include "main.h"
#include <stdint.h>


void uart_serial_print(uint8_t* data, uint8_t size)
{
    HAL_UART_Transmit(&huart2, data, size, 50); 
}
