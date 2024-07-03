/*
 * can_spi.c
 *
 *  Created on: Jun 26, 2024
 *      Author: Justin Whitcomb
 */

#include "can_spi.h"
#include "main.h"
#include "mcp2515.h"
#include "uart.h"
#include <stdbool.h>
#include <string.h>

 
bool canspi_Init(void)
{
    if(!MCP2515_Init())
    {
        return false; 
    } 
    return true; 
}
uint8_t canspi_MessagesInBuffer(void)
{
    uint8_t messageCount = 0; 
    ctrl_status_t status;
    status = MCP2515_GetControlStatus(); 
    
    // Check the first buffer to see if there is data
    if(status.RX0IF != 0)
    {
        messageCount++; 
    }
    // Check the second buffer to see if there is data
    if(status.RX1IF != 0)
    {
        messageCount++; 
    }

    return messageCount; 
}

uint8_t canspi_TransmitMessage(can_msg_t *can_message)
{
    uint8_t retVal = 1; 
    ctrl_status_t control_status = MCP2515_GetControlStatus (); 

    if(control_status.TXB0REQ != 1)
    {
        //Load data into the buffer
        MCP2515_WriteTxBuffer(TX_BUF_TXB0SIDH, (uint8_t*)can_message->frame.canId.id, can_message->frame.dlc); 
    }

    if(control_status.TXB1REQ != 1)
    {
        //Load data into the buffer
        MCP2515_WriteTxBuffer(TX_BUF_TXB1SIDH, (uint8_t*)can_message->frame.canId.id, can_message->frame.dlc); 
    }

    if(control_status.TXB2REQ != 1)
    {
        //Load data into the buffer
        MCP2515_WriteTxBuffer(TX_BUF_TXB2SIDH, (uint8_t*)can_message->frame.canId.id, can_message->frame.dlc); 
    }
    return retVal;
}    

uint8_t canspi_ReceiveMessage(can_msg_t *can_message)
{
  uint8_t retVal = 0;
  rx_reg_t rxReg;
  rx_status_t rxStatus;
  
  rxStatus = MCP2515_GetRxStatus();
  uint8_t tempRxStatus = (uint8_t)rxStatus.ctrl_rx_status; 
  uart_serial_print(&tempRxStatus, sizeof(tempRxStatus)); 
  /* Check receive buffer */
  if (rxStatus.rxBuffer != 0)
  {
    /* finding buffer which has a message */
    if ((rxStatus.rxBuffer == MSG_IN_RXB0)|(rxStatus.rxBuffer == MSG_IN_BOTH_BUFFERS))
    {
      MCP2515_ReadRxBuffer(MCP2515_READ_RXB0SIDH, rxReg.rx_reg_array, sizeof(rxReg.rx_reg_array));
    }
    else if (rxStatus.rxBuffer == MSG_IN_RXB1)
    {
      MCP2515_ReadRxBuffer(MCP2515_READ_RXB1SIDH, rxReg.rx_reg_array, sizeof(rxReg.rx_reg_array));
    }
    
    // uart_serial_print(rxReg.rx_reg_array, sizeof(rxReg.rx_reg_array)); 
    can_message->frame.canId.id = rxReg.RXBnSIDH;
    can_message->frame.dlc      = rxReg.RXBnDLC;
    can_message->frame.data0    = rxReg.RXBnD0;
    can_message->frame.data1    = rxReg.RXBnD1;
    can_message->frame.data2    = rxReg.RXBnD2;
    can_message->frame.data3    = rxReg.RXBnD3;
    can_message->frame.data4    = rxReg.RXBnD4;
    can_message->frame.data5    = rxReg.RXBnD5;
    can_message->frame.data6    = rxReg.RXBnD6;
    can_message->frame.data7    = rxReg.RXBnD7;
    
    retVal = 1;
  }
  return retVal; 
}