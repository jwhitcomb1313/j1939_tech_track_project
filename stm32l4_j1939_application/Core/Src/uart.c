/*
 * uart.c
 *
 *  Created on: Jun 25, 2024
 *      Author: Justin Whitcomb
 */

#include "uart.h"
#include "main.h"
#include <stdint.h>
#include <stdio.h>

/******************************************************************************/
/*!
   \fn      void uart_serial_print(uint8_t* data, uint8_t size)
   \brief   This function writes data to the serial port COM 6 
   \param   uint8_t* data: Pointer to the data that will be sent 
   \param   uint8_t size: Size of data to be sent 
   \return  uint8_t retVal: byte at the given address

    @{
*/
/******************************************************************************/
void uart_serial_print(uint8_t* data, uint8_t size)
{
    HAL_UART_Transmit(&huart2, data, size, 100);
}

/******************************************************************************/
/*!
   \fn      void uart_serialStringPrint(uint8_t* strValue, uint8_t* value, uint8_t size)
   \brief   This function writes data to the serial port COM 6 
   \param   uint8_t* strValue: Pointer to the string value that will be printed
   \param   uint8_t* value: Pointer to the value that will be printed in the string 
   \param   uint8_t size: Size of data to print
   \return  None. 

    @{
*/
/******************************************************************************/
// void uart_serialStringPrint(uint8_t* strValue, uint8_t* value, uint8_t size)
// { 
//     uint8_t data_array[50]; 
//     if(size < 50)
//     {
//         if((strValue && value) != NULL)
//         {
//             sprintf(data_array, "%s = %u\r\n", strValue, value); 
//         }
//         else if(strValue != 0)
//         {
//             sprintf(data_array, "%s\r\n", (char*)strValue); 
//         }
//         else if(value != 0)
//         {
//             sprintf(data_array, "= %u\r\n", strValue, value); 
//         }

//         if(data_array[0] != 0)
//         {
//             HAL_UART_Transmit(&huart2, data_array, sizeof(data_array), 100); 
//         }
//     }
// }

uint8_t* tempArray(void)
{
    uint8_t tempArray[30]; 
    return tempArray; 
}   