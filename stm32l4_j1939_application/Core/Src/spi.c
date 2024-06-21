/*
 * spi.c
 *
 *  Created on: 6/21/2024
 *      Author: Justin Whitcomb
 */

#include "spi.h"
#include "main.h"
#include "string.h"

char spi_write_buffer[20]; 
char spi_read_buffer[20]; 


void Mcp_SetCsPin(GPIO_PinState state)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, state);
}

spi_error_t Mcp_WriteCommand(uint8_t* data, uint8_t* address, uint8_t size)
{
    spi_error_t retVal = SPI_ERROR; 
    // Set pin low to transmit
    Mcp_SetCsPin(GPIO_PIN_RESET); 
    
    if(HAL_SPI_Transmit(&hspi1, (uint8_t*) data, size, MCP_TIMEOUT) == HAL_OK)
    {
        retVal = SPI_OK; 
    }
    return retVal; 
}

