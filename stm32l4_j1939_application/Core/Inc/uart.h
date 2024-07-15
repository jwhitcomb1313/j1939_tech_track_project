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

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0')

void uart_serial_print(char* data, uint8_t size); 

#endif /* SRC_UART_H_ */
