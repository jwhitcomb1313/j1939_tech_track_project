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
#include <stdint.h>

/******************** ********** ************************/
/******************** Prototypes ************************/
/******************** ********** ************************/
void canspi_ConvertRegToID(id_reg_t regId, uint32_t *canId); 
void canspi_ConvertIDToReg(uint32_t canId, id_reg_t *regId); 


/******************** ******************** ***********************/
/******************** Function Definitions ***********************/
/******************** ******************** ***********************/
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
    char tempbuf[25]; 


    uint8_t retVal = 0; 
    id_reg_t regId;

    regId.SIDH = 0; 
    regId.SIDL = 0; 
    regId.EID8 = 0; 
    regId.EID0 = 0; 

    ctrl_status_t control_status = MCP2515_GetControlStatus (); 
    sprintf(tempbuf, "control status = %x\r\n\n", control_status.ctrl_status); 
    uart_serial_print(tempbuf, sizeof(tempbuf));

    canspi_ConvertIDToReg(can_message->frame.canId, &regId); 

    if(control_status.TXB0REQ != 1)
    {
        //Load data into the buffer
        MCP2515_WriteTxBuffer(MCP2515_LOAD_TXB0SIDH, &regId, &(can_message->frame.data0), can_message->frame.dlc);
        retVal = 1;  
    }

    if(control_status.TXB1REQ != 1)
    {
        //Load data into the buffer
        MCP2515_WriteTxBuffer(MCP2515_LOAD_TXB1SIDH, &regId, &(can_message->frame.data0), can_message->frame.dlc);
        retVal = 1; 
    }

    if(control_status.TXB2REQ != 1)
    {
        //Load data into the buffer
        MCP2515_WriteTxBuffer(MCP2515_LOAD_TXB2SIDH, &regId, &(can_message->frame.data0), can_message->frame.dlc);
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
    //todo remove
    char testBuf[15];


    uint8_t retVal = 0;
    rx_reg_t rxReg;
    rx_status_t rxStatus;
    id_reg_t idReg;
    uint32_t idExt;  

    rxStatus = MCP2515_GetRxStatus();
    /* Check receive buffer */
    if (rxStatus.rxBuffer != 0)
    {
        sprintf(testBuf, "rx flag\r\n"); 
        uart_serial_print(testBuf, sizeof(testBuf)); 
        memset(testBuf, '\0', sizeof(testBuf));
        /* finding buffer which has a message */
        if ((rxStatus.rxBuffer == MSG_IN_RXB0)|(rxStatus.rxBuffer == MSG_IN_BOTH_BUFFERS))
        {
            MCP2515_ReadRxBuffer(MCP2515_READ_RXB0SIDH, rxReg.rx_reg_array, sizeof(rxReg.rx_reg_array));
        }
        else if (rxStatus.rxBuffer == MSG_IN_RXB1)
        {
            MCP2515_ReadRxBuffer(MCP2515_READ_RXB1SIDH, rxReg.rx_reg_array, sizeof(rxReg.rx_reg_array));
        }

        //todo: remove
        MCP2515_ReadRxBuffer(MCP2515_READ_RXB0SIDH, rxReg.rx_reg_array, sizeof(rxReg.rx_reg_array));

        idReg.SIDH = rxReg.RXBnSIDH; 
        idReg.SIDL = rxReg.RXBnSIDL; 
        idReg.EID8 = rxReg.RXBnEID8; 
        idReg.EID0 = rxReg.RXBnEID0; 

        canspi_ConvertRegToID(idReg, &idExt); 

        canMsg->frame.canId     = idExt;
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

/******************************************************************************/
/*!
   \fn      void canspi_ConvertRegToID(id_reg_t regId, can_ext_id_t *extId)
   \brief   This function converts register data into usable ext ID data
   \param   id_reg_t regId: The extended ID in MCP2515 register format
   \param   uint32_t *canId: Pointer to where the ext ID will be 
                             stored in CAN format
   \return  None.

    @{
*/
/******************************************************************************/
void canspi_ConvertRegToID(id_reg_t regId, uint32_t *canId)
{   
    // Temp variable type for unpacking the registers
    can_ext_id_t extId;  
    // Ext ID 28:24
    extId.frame.priority = ((regId.SIDH & ID_MASK_PRIORITY) >> 5); 
    extId.frame.edp = (regId.SIDH & ID_MASK_EDP >> 4);
    extId.frame.dp = ((regId.SIDH & ID_MASK_DP) >> 3);  

    // Ext ID 23:16
    extId.frame.pf = ((((regId.SIDH & ID_MASK_PF_MSB) << 5)    |
                        (regId.SIDL & ID_MASK_PF_LSBUB) >> 3)   |
                        (regId.SIDL & ID_MASK_PF_LSBLB));
    // Ext ID 15:8
    extId.frame.ps = regId.EID8; 
    // Ext ID 7:0
    extId.frame.source_address = regId.EID0; 

    *canId = extId.id; 
}

/******************************************************************************/
/*!
   \fn      void canspi_ConvertIDToReg(can_ext_id_t extId, id_reg_t *regId)
   \brief   This function converts register data into usable ext ID data
   \param   can_ext_id_t extId: ext ID to be converted into regsiter format
   \param   id_reg_t *regId: Pointer to where the ext ID will be populated in 
                             register format. 
   \return  None.

    @{
*/
/******************************************************************************/
void canspi_ConvertIDToReg(uint32_t canId, id_reg_t *regId)
{
    can_ext_id_t extId; 
    extId.id = canId; 
    // SIDH 7:5
    regId->SIDH = (extId.frame.priority << 5); 
    // SIDH 4:4 
    regId->SIDH |= (extId.frame.edp << 4);
    // SIDH 3:3
    regId->SIDH |= (extId.frame.dp << 3);
    // SIDH 2:0 
    regId->SIDH |= ((extId.frame.pf & REG_MASK_PF_MSB) >> 5);

    // SIDL 7:5 
    regId->SIDL |= ((extId.frame.pf & REG_MASK_PF_LSBUB) << 3); 
    // SIDL 3:3 Set the Ext ID bit high
    regId->SIDL |= REG_MASK_IDE; 
    // SIDL 2:0 
    regId->SIDL |= ((extId.frame.pf & REG_MASK_PF_LSBLB));
    
    // EID8 7:0
    regId->EID8 = extId.frame.ps; 

    // EID0 7:0
    regId->EID0 = extId.frame.source_address;
}

void canspi_CanLoopTest(can_msg_t canMsg)
{
    char printStr[30]; 
    can_ext_id_t tempId;
    tempId.id = canMsg.frame.canId; 


    sprintf(printStr, "**** CAN ID ****\r\n"); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "priority = %x\r\n", tempId.frame.priority); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr)); 

    sprintf(printStr, "dp = %x\r\n", tempId.frame.dp); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "edp = %x\r\n", tempId.frame.edp); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "pf = %x\r\n", tempId.frame.pf); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "ps = %x\r\n", tempId.frame.ps); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "source address = %x\r\n\n", tempId.frame.source_address); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "**** DATA ****\r\n"); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr)); 

    sprintf(printStr, "dlc = %x\r\n", canMsg.frame.dlc); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "data0 = %x\r\n", canMsg.frame.data0); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "data1 = %x\r\n", canMsg.frame.data1); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "data2 = %x\r\n", canMsg.frame.data2); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "data3 = %x\r\n", canMsg.frame.data3); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "data4 = %x\r\n", canMsg.frame.data4); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "data5 = %x\r\n", canMsg.frame.data5); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "data6 = %x\r\n", canMsg.frame.data6); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "data7 = %x\r\n\n", canMsg.frame.data7); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));
}

void canspi_idCheck(uint32_t canId)
{
    char printStr[30]; 
    can_ext_id_t tempId;
    tempId.id = canId; 


    sprintf(printStr, "**** CAN ID ****\r\n"); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "priority = %x\r\n", tempId.frame.priority); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr)); 

    sprintf(printStr, "dp = %x\r\n", tempId.frame.dp); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "edp = %x\r\n", tempId.frame.edp); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "pf = %x\r\n", tempId.frame.pf); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "ps = %x\r\n", tempId.frame.ps); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "source address = %x\r\n\n", tempId.frame.source_address); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));
}