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

/******************************************************************************/
/*!
   \fn      void MCP2515_Init(void)
   \brief   This function inits settings for the mcp2515 
   \param   None
   \return  None

    @{
*/
/******************************************************************************/
void MCP2515_Init(void)
{
  MCP2515_SetConfigurationMode(); 
}

/******************************************************************************/
/*!
   \fn      bool MCP2515_SetConfigurationMode(void)
   \brief   This function sets the mcp2515 into configuration mode 
   \param   None
   \return  bool: determines if the mode was set or not

    @{
*/
/******************************************************************************/
bool MCP2515_SetConfigurationMode(void)
{
  MCP2515_WriteByte(MCP2515_CANCTRL, MODE_CONFIGURATION); 

  for(int i = 10; i < 10; i ++)
  {
    if((MCP2515_ReadByte(MCP2515_CANSTAT) & MCP2515_OPMODE_MASK) == MODE_CONFIGURATION)
    {
      return true; 
    } 
  }
  return false;
}

/******************************************************************************/
/*!
   \fn      bool MCP2515_SetNormalMode(void)
   \brief   This function sets the mcp2515 into normal mode 
   \param   None
   \return  bool: determines if the mode was set or not

    @{
*/
/******************************************************************************/
bool MCP2515_SetNormalMode(void)
{
  MCP2515_WriteByte(MCP2515_CANCTRL, MODE_NORMAL); 

  for(int i = 10; i < 10; i ++)
  {
    if((MCP2515_ReadByte(MCP2515_CANSTAT) & MCP2515_OPMODE_MASK) == MODE_NORMAL)
    {
      return true; 
    } 
  }
  return false;
}

/******************************************************************************/
/*!
   \fn      bool MCP2515_SetLoopbackMode(void)
   \brief   This function sets the mcp2515 into loopback mode 
   \param   None
   \return  bool: determines if the mode was set or not

    @{
*/
/******************************************************************************/

bool MCP2515_SetLoopbackMode(void)
{
  MCP2515_WriteByte(MCP2515_CANCTRL, MODE_LOOPBACK); 

  for(int i = 10; i < 10; i ++)
  {
    if((MCP2515_ReadByte(MCP2515_CANSTAT) & MCP2515_OPMODE_MASK) == MODE_LOOPBACK)
    {
      return true; 
    } 
  }
  return false; 
}

/******************************************************************************/
/*!
   \fn      void MCP2515_WriteByte(uint8_t address, uint8_t data)
   \brief   This function writes a single byte to a register at the selected adddress 
   \param   uint8_t address: address to write the byte 
   \param   uint8_t data: byte that is to be written
   \return   None

    @{
*/
/******************************************************************************/
void MCP2515_WriteByte(uint8_t address, uint8_t data)
{
  MCP2515_CS_LOW();  

  SPI_Tx((uint8_t)INSTRUCTION_WRITE);
  SPI_Tx(address);
  SPI_Tx(data);  
  
  MCP2515_CS_HIGH();
}

/******************************************************************************/
/*!
   \fn      void MCP2515_WriteTxBuffer(tx_buffer_instruct_t instruction, uint8_t* data, uint8_t length)
   \brief   This function loads data into one of the tx buffers, overhead is reduced by includeing the 
            instruction into the address. 
   \param   tx_buffer_instruct_t instruction: This type is both the address and write buffer instruction with 
            6 possible instructions. This points to the ID or data adress of any of the 3 tx buffers
   \param   uint8_t* data: Pointer to the data that is to be written to the buffer  
   \param   uint8_t length: Length of the data  
   \return   None

    @{
*/
/******************************************************************************/
void MCP2515_WriteTxBuffer(tx_buffer_instruct_t instruction, uint8_t* data, uint8_t length)
{
  MCP2515_CS_LOW();

  SPI_Tx((uint8_t)instruction);
  SPI_TxBuffer(data, length);

  MCP2515_CS_HIGH();
}

/******************************************************************************/
/*!
   \fn      uint8_t MCP2515_ReadByte(uint8_t address)
   \brief   This function reads a single byte from a register at the selected adddress 
   \param   uint8_t address: address to read the byte 
   \return  uint8_t retVal: byte at the given address

    @{
*/
/******************************************************************************/
uint8_t MCP2515_ReadByte(uint8_t address)
{
    uint8_t retVal = 0; 

    MCP2515_CS_LOW();
  
    SPI_Tx((uint8_t)INSTRUCTION_READ);
    SPI_Tx(address);
    retVal = SPI_Rx();
  
    MCP2515_CS_HIGH();

    return retVal;
}

/******************************************************************************/
/*!
   \fn      void MCP2515_ReadRxBuffer(rx_buffer_instruct_t instruction, uint8_t* data, uint8_t length)
   \brief   This function reads data from one of the rx buffers, overhead is reduced by includeing the 
            instruction into the address. 
   \param   rx_buffer_instruct_t instruction: This type is both the address and read buffer instruction with 
            4 possible instructions. 
   \param   uint8_t* data: Pointer that the data will be written to  
   \param   uint8_t length: Length of the data  
   \return   None

    @{
*/
/******************************************************************************/
void MCP2515_ReadRxBuffer(rx_buffer_instruct_t instruction, uint8_t* data, uint8_t length)
{
    SPI_Tx(instruction); 
    SPI_RxBuffer(data, length); 
}

/******************************************************************************/
/*!
   \fn      uint8_t MCP2515_GetRxStatus(void)
   \brief   This function is a quick polling command that indicates filter match 
            and message type (either standard, extended, and/or remote) of received message  
   \param   None. 
   \return  uint8_t retVal: Rx status byte

    @{
*/
/******************************************************************************/
uint8_t MCP2515_GetRxStatus(void)
{
  uint8_t retVal;
  
  MCP2515_CS_LOW();
  
  SPI_Tx(MCP2515_RX_STATUS);
  retVal = SPI_Rx();
        
  MCP2515_CS_HIGH();
  
  return retVal;
}

/******************************************************************************/
/*!
   \fn      uint8_t MCP2515_GetReadStatus(void)
   \brief   This function is a quick polling command that reads several status bits 
            for transmit and receive functions 
            7: TX2IF (CANINTF[4])
            6: TXREQ (TXB2CNTRL[3])
            5: TX1IF (CANINTF[3])
            4: TXREQ (TXB1CNTRL[3])
            3: TX0IF (CANINTF[2])
            2: TXREQ (TXB0CNTRL[3])
            1: RX1IF (CANINTF[1])
            0: RX0IF (CANINTF[0]) 
   \param   None. 
   \return  uint8_t retVal: Read status byte

    @{
*/
/******************************************************************************/
uint8_t MCP2515_GetReadStatus(void)
{
  uint8_t retVal;
  
  MCP2515_CS_LOW();
  
  SPI_Tx(MCP2515_READ_STATUS);
  retVal = SPI_Rx();
        
  MCP2515_CS_HIGH();
  
  return retVal;
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




