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
#include <stdio.h>

/******************************************************************************/
/*!
   \fn      bool canspi_Init(void)
   \brief   This function inits CAN  
   \param   None
   \return  None

    @{
*/
/******************************************************************************/
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

/******************************************************************************/
/*!
   \fn      uint8_t canspi_TransmitMessage(can_msg_t *can_message)
   \brief   This function transmits CAN messages
   \param   can_msg_t *can_message: Pointer to the CAN message to be sent
   \return  uint8_t retVal: Returns true if the message was transmitted 

    @{
*/
/******************************************************************************/
uint8_t canspi_TransmitMessage(can_msg_t *can_message)
{
    uint8_t retVal = 0; 
    ctrl_status_t control_status = MCP2515_GetControlStatus (); 

    if(control_status.TXB0REQ != 1)
    {
        //Load data into the buffer
        MCP2515_WriteTxBuffer(TX_BUF_TXB0SIDH, (uint8_t*)can_message->frame.canId, can_message->frame.dlc); 
        retVal = 1; 
    }

    if(control_status.TXB1REQ != 1)
    {
        //Load data into the buffer
        MCP2515_WriteTxBuffer(TX_BUF_TXB1SIDH, (uint8_t*)can_message->frame.canId, can_message->frame.dlc); 
        retVal = 1; 
    }

    if(control_status.TXB2REQ != 1)
    {
        //Load data into the buffer
        MCP2515_WriteTxBuffer(TX_BUF_TXB2SIDH, (uint8_t*)can_message->frame.canId, can_message->frame.dlc); 
        retVal = 1; 
    }
    return retVal;
}    

/******************************************************************************/
/*!
   \fn      uint8_t canspi_ReceiveMessage(can_msg_t *can_message)
   \brief   This function receives CAN messages
   \param   can_msg_t *can_message: Pointer to the CAN message to be received
   \return  uint8_t retVal: Returns true if there is data in the buffer

    @{
*/
/******************************************************************************/
uint8_t canspi_ReceiveMessage(can_msg_t *canMsg)
{
  uint8_t retVal = 0;
  rx_reg_t rxReg;
  rx_status_t rxStatus;
  
  rxStatus = MCP2515_GetRxStatus();

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
     
    canMsg->frame.canId     = rxReg.RXBnSIDH;
    canMsg->frame.dlc       = rxReg.RXBnDLC;
    canMsg->frame.data0     = rxReg.RXBnD0;
    canMsg->frame.data1     = rxReg.RXBnD1;
    canMsg->frame.data2     = rxReg.RXBnD2;
    canMsg->frame.data3     = rxReg.RXBnD3;
    canMsg->frame.data4     = rxReg.RXBnD4;
    canMsg->frame.data5     = rxReg.RXBnD5;
    canMsg->frame.data6     = rxReg.RXBnD6;
    canMsg->frame.data7     = rxReg.RXBnD7;
    
    retVal = 1;
  }
  return retVal; 
}

void canspi_ConvertRegToID(id_reg_t regId, can_ext_id_t *extId)
{    
    // Ext ID 28:24
    extId->frame.priority = ((regId.SIDH & ID_MASK_PRIORITY) >> 5); 
    extId->frame.edp = (regId.SIDH & ID_MASK_EDP >> 4);
    extId->frame.dp = ((regId.SIDH & ID_MASK_DP) >> 3);  

    // Ext ID 23:16
    extId->frame.pf = ((((regId.SIDH & ID_MASK_PF_MSB) << 5)    |
                        (regId.SIDL & ID_MASK_PF_LSBUB) >> 3)   |
                        (regId.SIDL & ID_MASK_PF_LSBLB));


    // Ext ID 15:8
    extId->frame.ps = regId.EID8; 
    // Ext ID 7:0
    extId->frame.source_address = regId.EID0; 
    
    
    //TODO: Need to include the EXIDE bit somewhere? bitpos 3 in SIDL

}

void canspi_ConvertIDToReg(can_ext_id_t extId, id_reg_t *regId)
{
    uint8_t tempBits = 0; 

    // SIDH 7:5
    tempBits = extId.frame.priority; 
    regId->SIDH = (tempBits << 5); 
    // SIDH 4:4
    tempBits = extId.frame.edp; 
    regId->SIDH = (tempBits << 4);
    // SIDH 3:3
    tempBits = extId.frame.dp; 
    regId->SIDH = (tempBits << 3);
    // SIDH 2:0
    tempBits = (extId.frame.pf & REG_MASK_PF_MSB); 
    regId->SIDH = (tempBits >> 5);

    // SIDL 7:5
    tempBits = (extId.frame.pf & REG_MASK_PF_LSBUB); 
    regId->SIDL = (tempBits << 5); 
    // SIDL 3:3 Set the Ext ID bit high
    regId->SIDL = (regId->SIDL | REG_IDE_BIT); 
    // SIDL 2:0

    // EID8 7:0
    regId->EID8 = extId.frame.ps; 

    // EID0 7:0
    regId->EID0 = extId.frame.source_address;
}

void canspi_CanPrintFunction(can_msg_t canMsg)
{
    char printStr[30]; 
    can_ext_id_t tempId;
    tempId.id = canMsg.frame.canId; 


    sprintf(printStr, "PGN = %u%u%u%u\r\n", tempId.frame.edp, tempId.frame.dp, 
            tempId.frame.pdu_format, tempId.frame.pdu_specific); 
    uart_serial_print((uint8_t*)printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr)); 
}