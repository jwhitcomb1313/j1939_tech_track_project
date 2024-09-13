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
#include "j1939.h"


#define REG_MASK_RX_BUFFER  0x03
uint8_t nullData[13];
char buffer[20]; 
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
    /* Change mode as configuration mode */
    if(!MCP2515_SetConfigurationMode())
    {
        return false;
    }
    
    // MCP2515_WriteByte(0x60, 0x60);
    // MCP2515_WriteByte(0x70, 0x60);

    //TODO: Need to update the math here to match my register values
    /* 
    * tq = 2 * (brp(0) + 1) / 16000000 = 0.125us
    * tbit = (SYNC_SEG(1 fixed) + PROP_SEG + PS1 + PS2)
    * tbit = 1tq + 5tq + 6tq + 4tq = 16tq
    * 16tq = 2us = 500kbps
    */
    
    // MCP2515_WriteByte(MCP2515_CANCTRL, 0x80);
    /* 00(SJW 1tq) 000000 */  
    MCP2515_WriteByte(MCP2515_CNF1, 0x00);
    
    /* 1 1 100(5tq) 101(6tq) */  
    MCP2515_WriteByte(MCP2515_CNF2, 0xBF);
    
    /* 1 0 000 011(4tq) */  
    MCP2515_WriteByte(MCP2515_CNF3, 0x02);   

    MCP2515_WriteByte(MCP2515_CANCTRL, 0x00);
    MCP2515_SetNormalMode(); 
    return true; 
}
// uint8_t canspi_MessagesInBuffer(void)
// {
//     uint8_t messageCount = 0; 
//     ctrl_status_t status;
//     status = MCP2515_GetControlStatus(); 
    
//     // Check the first buffer to see if there is data
//     if(status.RX0IF != 0)
//     {
//         messageCount++; 
//     }
//     // Check the second buffer to see if there is data
//     if(status.RX1IF != 0)
//     {
//         messageCount++; 
//     }

//     return messageCount; 
// }

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
    // char buf[30]; 
    uint8_t retVal = 0; 
    id_reg_t regId;
    regId.SIDH = 0; 
    regId.SIDL = 0; 
    regId.EID8 = 0; 
    regId.EID0 = 0;  

    ctrl_status_t control_status = MCP2515_GetControlStatus(); 

    canspi_ConvertIDToReg(can_message->frame.canId, &regId); 

    if(control_status.TXB0REQ != 1)
    {
        //Load data into the buffer
        MCP2515_WriteTxBuffer(MCP2515_LOAD_TXB0SIDH, &(regId.SIDH), &(can_message->frame.data0), can_message->frame.dlc); 
        MCP2515_RequestToSend(MCP2515_RTS_TX0);
        retVal = 1;  
    }

    else if(control_status.TXB1REQ != 1)
    {
        //Load data into the buffer
        MCP2515_WriteTxBuffer(MCP2515_LOAD_TXB1SIDH, &(regId.SIDH), &(can_message->frame.data0), can_message->frame.dlc);
        MCP2515_RequestToSend(MCP2515_RTS_TX1);
        retVal = 1; 
    }

    else if(control_status.TXB2REQ != 1)
    {
        //Load data into the buffer
        MCP2515_WriteTxBuffer(MCP2515_LOAD_TXB2SIDH, &(regId.SIDH), &(can_message->frame.data0), can_message->frame.dlc);
        MCP2515_RequestToSend(MCP2515_RTS_TX2);
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
uint8_t canspi_ReceiveMessage(void)
{
    uint8_t retVal = 0;
    rx_reg_t rxReg;
    rx_status_t rxStatus;
    id_reg_t idReg;
    uint32_t idExt; 
    j1939_message_t message; 
    // char buf[30]; 

    uint8_t bufferByte = (REG_MASK_RX_BUFFER & MCP2515_ReadByte(MCP2515_CANINTF));
    // bufferByte &= REG_MASK_RX_BUFFER;

    rxStatus = MCP2515_GetRxStatus();
    /* Check receive buffer */
    if (rxStatus.rxBuffer != 0)
    {
         
        //remove after debug
        // sprintf(buf, "CANINTF = %x\r\n", bufferByte); 
        // uart_serial_print(buf, sizeof(buf));
        // memset(buf, '\0', sizeof(buf));

        /* finding buffer which has a message */
        //if ((rxStatus.rxBuffer == MSG_IN_RXB0)|(rxStatus.rxBuffer == MSG_IN_BOTH_BUFFERS))
        if((bufferByte == MSG_IN_RXB0) | (bufferByte == MSG_IN_BOTH_BUFFERS))
        {
            canspi_readRxBuffer(&rxReg, 0);
        }
        if (rxStatus.rxBuffer == MSG_IN_RXB1)
        {
            canspi_readRxBuffer(&rxReg, 1);
        }

        idReg.SIDH = rxReg.RXBnSIDH; 
        idReg.SIDL = rxReg.RXBnSIDL; 
        idReg.EID8 = rxReg.RXBnEID8; 
        idReg.EID0 = rxReg.RXBnEID0; 
         
        canspi_ConvertRegToID(idReg, &idExt); 
        can_ext_id_t canId;
        canId.id = idExt;  
   
        uint16_t pgn = canId.frame.pf; 
        pgn = (pgn << 8) | canId.frame.ps; 

        message.message_id.PGN = pgn; 
        message.message_id.destination_address = canId.frame.ps; 
        message.message_id.source_address = canId.frame.source_address;
        message.message_id.priority = canId.frame.priority; 

        message.length = rxReg.RXBnDLC;
        message.data_buffer[0] = rxReg.RXBnD0;; 
        message.data_buffer[1] = rxReg.RXBnD1;
        message.data_buffer[2] = rxReg.RXBnD2;
        message.data_buffer[3] = rxReg.RXBnD3;
        message.data_buffer[4] = rxReg.RXBnD4;
        message.data_buffer[5] = rxReg.RXBnD5;
        message.data_buffer[6] = rxReg.RXBnD6;
        message.data_buffer[7] = rxReg.RXBnD7;

        j1939_AddMessageToTable(message);
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
    extId.frame.edp = ((regId.SIDH & ID_MASK_EDP) >> 4);
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
    // SIDL 3:3 Always set the Ext ID bit high
    regId->SIDL |= REG_MASK_IDE; 
    // SIDL 2:0 
    regId->SIDL |= ((extId.frame.pf & REG_MASK_PF_LSBLB));
    
    // EID8 7:0
    regId->EID8 = extId.frame.ps; 

    // EID0 7:0
    regId->EID0 = extId.frame.source_address;
}

void canspi_readRxBuffer(rx_reg_t *rxData, uint8_t regPosition)
{
    uint8_t readByte = 0; 

    if(regPosition == 0)
    {
        for(int i = 0; i < 13; i++)
        {
            rxData->rx_reg_array[i] = MCP2515_ReadByte(MCP2515_RXB0SIDH + i); 
        }
        //Clear interrupt flag: bit 0
        readByte = MCP2515_ReadByte(MCP2515_CANINTF);
        readByte &= ~(1 << 0);
        MCP2515_WriteByte(MCP2515_CANINTF, readByte);
    }

    else if(regPosition == 1)
    {
        for(int i = 0; i < 13; i++)
        {
            rxData->rx_reg_array[i] = MCP2515_ReadByte(MCP2515_RXB1SIDH + i); 
        }
        //Clear interrupt flag: bit 1
        readByte = MCP2515_ReadByte(MCP2515_CANINTF);
        readByte &= ~(1 << 1);
        MCP2515_WriteByte(MCP2515_CANINTF, readByte);
    }
}