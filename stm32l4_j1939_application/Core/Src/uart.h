/*
 * uart.h
 *
 *  Created on: Jun 25, 2024
 *      Author: Justin Whitcomb
 */

#ifndef SRC_UART_H_
#define SRC_UART_H_

#include "main.h"

extern UART_HandleTypeDef huart2;



void uart_serial_print(uint8_t* data, uint8_t size); 

#endif /* SRC_UART_H_ */
