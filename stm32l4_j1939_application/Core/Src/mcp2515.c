/*
 * mcp2515.c
 *
 *  Created on: 6/21/2024
 *      Author: Justin Whitcomb
 */

#include "mcp2515.h"

#include "main.h"

/******************** ********** ************************/
/******************** Prototypes ************************/
/******************** ********** ************************/
static void SPI_Tx(uint8_t data); 
static void SPI_TxBuffer(uint8_t *buffer, uint8_t length); 
static uint8_t SPI_Rx(void); 
static void SPI_RxBuffer(uint8_t *buffer, uint8_t length); 

/******************** ******************** ***********************/
/******************** Function Definitions ***********************/
/******************** ******************** ***********************/
//TODO: Create init function
void MCP2515_Init(void)
{
    
}

// Write a single byte to the MCP2515
void MCP2515_WriteByte(uint8_t data, uint8_t address, uint8_t size)
{
    MCP2515_CS_LOW();  
  
    SPI_Tx(MCP2515_WRITE);
    SPI_Tx(address);
    SPI_Tx(data);  
    
    MCP2515_CS_HIGH();
}

// Write a series of bytes to the MCP2515
void MCP2515_WriteByteSequence(uint8_t* data, uint8_t address, uint8_t length)
{
    MCP2515_CS_LOW();
  
    SPI_Tx(MCP2515_WRITE);
    SPI_Tx(address);
    SPI_TxBuffer(data, length);
  
    MCP2515_CS_HIGH();
}

uint8_t MCP2515_ReadByte(uint8_t address)
{
    uint8_t retVal = 0; 

    MCP2515_CS_LOW();
  
    SPI_Tx(MCP2515_READ);
    SPI_Tx(address);
    retVal = SPI_Rx();
  
    MCP2515_CS_HIGH();

    return retVal;
}

void MCP2515_ReadByteSequence(uint8_t instruction, uint8_t address, uint8_t* data, uint8_t length)
{
    SPI_Tx(instruction); 
    SPI_RxBuffer(data, length); 
}


/******************** Wrapper Functions ***********************/
/* SPI Tx wrapper function  */
static void SPI_Tx(uint8_t data)
{
  HAL_SPI_Transmit(&hspi1, &data, 1, SPI_TIMEOUT);    
}

/* SPI Tx wrapper function */
static void SPI_TxBuffer(uint8_t *buffer, uint8_t length)
{
  HAL_SPI_Transmit(&hspi1, buffer, length, SPI_TIMEOUT);    
}

/* SPI Rx wrapper function */
static uint8_t SPI_Rx(void)
{
  uint8_t retVal;
  HAL_SPI_Receive(&hspi1, &retVal, 1, SPI_TIMEOUT);
  return retVal;
}

/* SPI Rx wrapper function */
static void SPI_RxBuffer(uint8_t *buffer, uint8_t length)
{
  HAL_SPI_Receive(&hspi1, buffer, length, SPI_TIMEOUT);
}




