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

void canspi_ConvertRegToID(id_reg_t regId, can_ext_id_t *canId)
{
    uint8_t tempPduFormat = 0; 
    uint8_t tempPduSpecific = 0;
    uint8_t tempSourceAddr = 0; 
    //
    canId->frame.priority = ((regId.tempSIDL & ID_MASK_PRIORITY) >> 5); 
    canId->frame.edp = (regId.tempSIDH & ID_MASK_EDP);
    canId->frame.dp = ((regId.tempSIDH & ID_MASK_DP) >> 1);  

    tempPduFormat = ((regId.tempSIDH & ID_MASK_PDU_FORM_UPPER) >> 2); 
    tempPduFormat = ((regId.tempEID0 & ID_MASK_PDU_FORM_LOWER) | tempPduFormat); 
    canId->frame.pdu_format = tempPduFormat; 

    tempPduSpecific = ((regId.tempEID0 & ID_MASK_PDU_SPEC_UPPER) >> 2); 
    tempPduSpecific = ((regId.tempEID8 & ID_MASK_PDU_SPEC_LOWER) | tempPduSpecific); 
    canId->frame.pdu_specific = tempPduSpecific; 

    tempSourceAddr = ((regId.tempEID8 & ID_MASK_SA_UPPER) >>2); 
    tempSourceAddr = ((regId.tempSIDL & ID_MASK_SA_LOWER) | tempSourceAddr); 
    canId->frame.source_address = tempSourceAddr;
}

void canspi_ConvertIDToReg(can_ext_id_t extId, id_reg_t *regId)
{

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