/*
 * mcp2515.c
 *
 *  Created on: 6/21/2024
 *      Author: Justin Whitcomb
 */

#include "mcp2515.h"
#include "main.h"
#include "uart.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/******************** ********** ************************/
/******************** Variables  ************************/
/******************** ********** ************************/
char test[30];
uint8_t read_error; 

/******************** ********** ************************/
/******************** Prototypes ************************/
/******************** ********** ************************/
static void SPI_Tx(uint8_t data); 
// static void SPI_TxBuffer(uint8_t *buffer, uint8_t length); 
static uint8_t SPI_Rx(void); 
static void SPI_RxBuffer(uint8_t *buffer, uint8_t length); 

/******************** ******************** ***********************/
/******************** Function Definitions ***********************/
/******************** ******************** ***********************/

void MCP_test_loopback_init(void)
{
  MCP2515_SetLoopbackMode(); 
}



/******************************************************************************/
/*!
   \fn      void MCP2515_Init(void)
   \brief   This function inits settings for the mcp2515 
   \param   None
   \return  None

    @{
*/
/******************************************************************************/
bool MCP2515_Init(void)
{
  MCP2515_CS_HIGH();


  sprintf(test, "mcp init\r\n"); 
  uart_serial_print(test, sizeof(test));
  memset(test, '\0', sizeof(test));
  return true; 
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
  uint8_t retVal = false; 
  uint8_t mode; 
  mode = MCP2515_ReadByte(MCP2515_CANCTRL); 
  mode |= MODE_CONFIGURATION; 
  MCP2515_WriteByte(MCP2515_CANCTRL, mode); 

  for(int i = 0; i < 10; i ++)
  {
    if((MCP2515_ReadByte(MCP2515_CANSTAT) & MCP2515_OPMODE_MASK) == MODE_CONFIGURATION)
    {
      sprintf(test, "config mode\r\n"); 
      uart_serial_print(test, sizeof(test));
      memset(test, '\0', sizeof(test));
      retVal = true; 
      break;
    } 
    else
    {
      mode = MCP2515_ReadByte(MCP2515_CANSTAT); 
      sprintf(test, "mode = %x\r\n", mode); 
      uart_serial_print(test, sizeof(test));
      memset(test, '\0', sizeof(test));
    }
  }
  return retVal;
}

/******************************************************************************/
/*!
   \fn      bool MCP2515_SetNormalMode(void)  MCP2515_CS_LOW();
   \brief   This function sets the mcp2515 into normal mode 
   \param   None
   \return  bool: determines if the mode was set or not

    @{
*/
/******************************************************************************/
bool MCP2515_SetNormalMode(void)
{
  bool retVal = false; 
  uint8_t mode; 
  mode = MCP2515_ReadByte(MCP2515_CANCTRL); 
  mode &= MODE_MASK_NORMAL; 
  MCP2515_WriteByte(MCP2515_CANCTRL, mode); 
  
  for(int i = 0; i < 10; i ++)
  {
    if((MCP2515_ReadByte(MCP2515_CANSTAT) & MCP2515_OPMODE_MASK) == MODE_NORMAL)
    {
      sprintf(test, "normal mode\r\n"); 
      uart_serial_print(test, sizeof(test));
      memset(test, '\0', sizeof(test));
      retVal = true; 
      break; 
    }
    else
    {
      mode = MCP2515_ReadByte(MCP2515_CANSTAT); 
      sprintf(test, "mode = %x\r\n", mode); 
      uart_serial_print(test, sizeof(test));
      memset(test, '\0', sizeof(test));
    } 
  }
  return retVal;
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
  bool retVal = false; 
  uint8_t mode = 0; 

  mode = MCP2515_ReadByte(MCP2515_CANSTAT);
  mode &= MODE_MASK_NORMAL; 
  mode |= MODE_MASK_LOOPBACK; 
  MCP2515_WriteByte(MCP2515_CANCTRL, mode); 

  for(int i = 0; i < 10; i ++)
  {
    if((MCP2515_ReadByte(MCP2515_CANSTAT) & MCP2515_OPMODE_MASK) == MODE_LOOPBACK)
    {
      mode = MCP2515_ReadByte(MCP2515_CANSTAT); 
      sprintf(test, "loopback mode"); 
      uart_serial_print(test, sizeof(test));
      retVal = true; 
      break;
    } 
    else
    {
      mode = MCP2515_ReadByte(MCP2515_CANSTAT); 
      sprintf(test, "mode = %x\r\n", mode); 
      uart_serial_print(test, sizeof(test));
      memset(test, '\0', sizeof(test));
    }
  }
  return retVal; 
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
   \fn      void MCP2515_WriteMultipleBytes(uint8_t address, uint8_t data)
   \brief   This function writes a multiple bytes to a register at the selected adddress
            and continue writing for the length indicated 
   \param   uint8_t address: address to write the byte 
   \param   uint8_t data: byte that is to be written
   \param   uint8_t length: number of bytes to be written
   \return   None

    @{
*/
/******************************************************************************/
void MCP2515_WriteMultipleBytes(uint8_t address, uint8_t* data, uint8_t length)
{
  MCP2515_CS_LOW();  

  SPI_Tx((uint8_t)INSTRUCTION_WRITE);
  SPI_Tx(address);
  SPI_TxBuffer(data, length);  
  
  MCP2515_CS_HIGH();
}

/******************************************************************************/
/*!
   \fn      void MCP2515_WriteTxBuffer(tx_buffer_instruct_t instruction, uint8_t* data, uint8_t length)
   \brief   This function loads data into one of the tx buffers, overhead is reduced by includeing the 
            instruction into the address. 
   \param   load_tx_buf_instr_t instruction: This type is both the address and write buffer instruction with 
            6 possible instructions. This points to the ID or data adress of any of the 3 tx buffers
   \param   uint8_t* data: Pointer to the data that is to be written to the buffer  
   \param   uint8_t length: Length of the data  
   \return   None

    @{
*/
/******************************************************************************/
void MCP2515_WriteTxBuffer(load_tx_buf_instr_t instruction, uint8_t* idReg, uint8_t* data, uint8_t dlc)
{
  MCP2515_CS_LOW();

  SPI_Tx((uint8_t)instruction);
  SPI_TxBuffer(idReg, 4); 
  SPI_Tx(dlc); 
  SPI_TxBuffer(data, dlc);

  MCP2515_CS_HIGH();
}

void tempMCP2515_WriteTxBuffer(load_tx_buf_instr_t instruction, uint8_t SIDH, uint8_t SIDL, 
                               uint8_t EID8, uint8_t EID0, uint8_t* data, uint8_t dlc)
{
  MCP2515_CS_LOW();

  SPI_Tx((uint8_t)instruction);
  // SPI_Tx(MCP2515_WRITE);
  // SPI_Tx(MCP2515_TXB0SIDH); 
  SPI_Tx(SIDH); 
  SPI_Tx(SIDL); 
  SPI_Tx(EID8); 
  SPI_Tx(EID0);
  SPI_Tx(dlc); 
  SPI_TxBuffer(data, dlc);

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
   \fn      uint8_t MCP2515_ReadMultipleBytes(uint8_t address)
   \brief   This function reads a single byte from a register at the selected adddress 
   \param   uint8_t address: address to read the byte 
   \return  uint8_t retVal: byte at the given address

    @{
*/
/******************************************************************************/
void MCP2515_ReadMultipleBytes(uint8_t address, uint8_t* data, uint8_t length)
{

    MCP2515_CS_LOW();
  
    SPI_Tx((uint8_t)INSTRUCTION_READ);
    SPI_Tx(address);
    SPI_RxBuffer(data, length); 
  
    MCP2515_CS_HIGH();
}

/******************************************************************************/
/*!
   \fn      void MCP2515_ReadRxBuffer(rx_buffer_instruct_t instruction, uint8_t* data, uint8_t length)
   \brief   This function reads data from one of the rx buffers, overhead is reduced by includeing the 
            instruction into the address. 
   \param   read_rx_buf_instr_t instruction: This type is both the address and read buffer instruction with 
            4 possible instructions. 
   \param   uint8_t* data: Pointer that the data will be written to  
   \param   uint8_t length: Length of the data  
   \return   None

    @{
*/
/******************************************************************************/
void MCP2515_ReadRxBuffer(read_rx_buf_instr_t instruction, uint8_t* data, uint8_t length)
{
    MCP2515_CS_LOW();

    SPI_Tx(instruction); 
    SPI_RxBuffer(data, length); 

    MCP2515_CS_HIGH();
}

/******************************************************************************/
/*!
   \fn      rx_status_t MCP2515_GetRxStatus(void)
   \brief   This function is a quick polling command that indicates filter match 
            and message type (either standard, extended, and/or remote) of received message  
            -------------------------
            0-2: Filter Match [filter]
            3-4: Message Type Received [msgType]
              5: Unused bit
            6-7: Received Message [rxBufStatus]
            -------------------------
   \param   None. 
   \return  rx_status_t retVal: Rx status byte

    @{
*/
/******************************************************************************/
rx_status_t MCP2515_GetRxStatus(void)
{
  rx_status_t retVal;
  
  MCP2515_CS_LOW();
  
  SPI_Tx(MCP2515_RX_STATUS);
  retVal.ctrl_rx_status = SPI_Rx();
        
  MCP2515_CS_HIGH();
  
  return retVal;
}

/******************************************************************************/
/*!
   \fn      ctrl_status_t MCP2515_GetControlStatus(void)
   \brief   This function is a quick polling command that reads several status bits 
            for transmit and receive functions 
            (Read Status in datasheet)
            -------------------------
            0: RX0IF (CANINTF[0])
            1: RX1IF (CANINTF[1])
            2: TXREQ (TXB0CNTRL[3])
            3: TX0IF (CANINTF[2])
            4: TXREQ (TXB1CNTRL[3])
            5: TX1IF (CANINTF[3])
            6: TXREQ (TXB2CNTRL[3])
            7: TX2IF (CANINTF[4])
            -------------------------
   \param   None. 
   \return  ctrl_status_t retVal: Control Status byte

    @{
*/
/******************************************************************************/
ctrl_status_t MCP2515_GetControlStatus(void)
{
  ctrl_status_t retVal;
  
  MCP2515_CS_LOW();
  
  SPI_Tx(MCP2515_READ_STATUS);
  retVal.ctrl_status = SPI_Rx();
        
  MCP2515_CS_HIGH();
  
  return retVal;
}

/* request to send */
void MCP2515_RequestToSend(uint8_t instruction)
{
  MCP2515_CS_LOW();
  
  SPI_Tx(instruction);
      
  MCP2515_CS_HIGH();
}

/******************** Wrapper Functions ***********************/
/******************************************************************************/
/*!
   \fn      static void SPI_Tx(uint8_t data)
   \brief   This function is a wrapper that transmits a single byte using HAL_SPI
   \param   uint8_t data: byte to send
   \return  None.

    @{
*/
/******************************************************************************/
static void SPI_Tx(uint8_t data)
{ 

  HAL_SPI_Transmit(&hspi1, &data, 1, SPI_TIMEOUT);  

}

/******************************************************************************/
/*!
   \fn      static void SPI_TxBuffer(uint8_t *buffer, uint8_t length)
   \brief   This function is a wrapper that transmits multiple bytes using HAL_SPI
   \param   uint8_t *buffer: pointer to a buffer of data to send
   \param   uint8_t length: length of data to send
   \return  None.

    @{
*/
/******************************************************************************/
void SPI_TxBuffer(uint8_t *buffer, uint8_t length)
{
  HAL_SPI_Transmit(&hspi1, buffer, length, SPI_TIMEOUT);    
}

/******************************************************************************/
/*!
   \fn      static uint8_t SPI_Rx(void)
   \brief   This function is a wrapper that receives a byte of data
            using HAL_SPI
   \param   None.
   \return  uint8_t retVal: a byte of data being received

    @{
*/
/******************************************************************************/
static uint8_t SPI_Rx(void)
{
  uint8_t retVal;
  
  read_error = (uint8_t) HAL_SPI_Receive(&hspi1, &retVal, 1, SPI_TIMEOUT);

  return retVal;
}

/******************************************************************************/
/*!
   \fn      static void SPI_RxBuffer(uint8_t *buffer, uint8_t length)
   \brief   This function is a wrapper that receives multiple bytes of data
            using HAL_SPI
   \param   uint8_t *buffer: pointer to a buffer to receive the data
   \param   uint8_t length: length of data to receive
   \return  None

    @{
*/
/******************************************************************************/
static void SPI_RxBuffer(uint8_t *buffer, uint8_t length)
{
  HAL_SPI_Receive(&hspi1, buffer, length, SPI_TIMEOUT);
}




