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
    /* Intialize MCP2515, check SPI */
    // if(!MCP2515_Init())
    // {
    //     return false;
    // }
        
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






























void canspi_printTxRxErrorReg(void)
{
    char printStr[30];  
    uint8_t readByte; 
    readByte = MCP2515_ReadByte(MCP2515_EFLG); 

    sprintf(printStr, "**** Error Registers ****\r\n"); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "eflg reg = %x\r\n", readByte); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr)); 

    readByte = MCP2515_ReadByte(MCP2515_TXB0CTRL);

    sprintf(printStr, "TXB0CTRL = %x\r\n", readByte); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));
}

void canspi_ReadTxRegisterPrint(void)
{
    char buf[34]; 
    uint8_t readByte = 0;  

    sprintf(buf, "\r\n\r\n***Register Values***\r\n\n");
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    readByte = MCP2515_ReadByte(MCP2515_TXB0SIDH);
    sprintf(buf, "\rTXB0SIDH = %x\r\n", readByte); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    readByte = MCP2515_ReadByte(MCP2515_TXB0SIDL);
    sprintf(buf, "TXB0SIDL = %x\r\n", readByte); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    readByte = MCP2515_ReadByte(MCP2515_TXB0EID8);
    sprintf(buf, "TXB0EID8 = %x\r\n", readByte); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    readByte = MCP2515_ReadByte(MCP2515_TXB0EID0);
    sprintf(buf, "TXB0EID0 = %x\r\n", readByte); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    // readByte = MCP2515_ReadByte(MCP2515_TXB0DLC);
    // sprintf(buf, "TXB0DLC = %x\r\n", readByte); 
    // uart_serial_print(buf, sizeof(buf));
    // memset(buf, '\0', sizeof(buf));

    // readByte = MCP2515_ReadByte(MCP2515_TXB0D0);
    // sprintf(buf, "TXB0D0 = %x\r\n", readByte); 
    // uart_serial_print(buf, sizeof(buf));
    // memset(buf, '\0', sizeof(buf));

    // readByte = MCP2515_ReadByte(MCP2515_TXB0D1);
    // sprintf(buf, "TXB0D1 = %x\r\n", readByte); 
    // uart_serial_print(buf, sizeof(buf));
    // memset(buf, '\0', sizeof(buf));

    // readByte = MCP2515_ReadByte(MCP2515_TXB0D2);
    // sprintf(buf, "TXB0D2 = %x\r\n", readByte); 
    // uart_serial_print(buf, sizeof(buf));
    // memset(buf, '\0', sizeof(buf));

    // readByte = MCP2515_ReadByte(MCP2515_TXB0D3);
    // sprintf(buf, "TXB0D3 = %x\r\n", readByte); 
    // uart_serial_print(buf, sizeof(buf));
    // memset(buf, '\0', sizeof(buf));

    // readByte = MCP2515_ReadByte(MCP2515_TXB0D4);
    // sprintf(buf, "TXB0D4 = %x\r\n", readByte); 
    // uart_serial_print(buf, sizeof(buf));
    // memset(buf, '\0', sizeof(buf));

    // readByte = MCP2515_ReadByte(MCP2515_TXB0D5);
    // sprintf(buf, "TXB0D5 = %x\r\n", readByte); 
    // uart_serial_print(buf, sizeof(buf));
    // memset(buf, '\0', sizeof(buf));

    // readByte = MCP2515_ReadByte(MCP2515_TXB0D6);
    // sprintf(buf, "TXB0D6 = %x\r\n", readByte); 
    // uart_serial_print(buf, sizeof(buf));
    // memset(buf, '\0', sizeof(buf));

    // readByte = MCP2515_ReadByte(MCP2515_TXB0D7);
    // sprintf(buf, "TXB0D7 = %x\r\n", readByte); 
    // uart_serial_print(buf, sizeof(buf));
    // memset(buf, '\0', sizeof(buf));
}

void printRegister(id_reg_t regId)
{
    char buf[30];  
    sprintf(buf, "***Bit Packed Registers***\r\n\n"); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    sprintf(buf, "SIDH = %x\r\n", regId.SIDH); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    sprintf(buf, "SIDL = %x\r\n", regId.SIDL); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    sprintf(buf, "EID8 = %x\r\n", regId.EID8); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    sprintf(buf, "EID0 = %x\r\n", regId.EID0); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));
}

void canspi_ReadRegIdPrint(id_reg_t regId)
{
    char bufprint[50]; 
    // uint8_t readByte = 0;  

    sprintf(bufprint, "\r\n\r\n***Rx Register Values***\r\n\n");
    uart_serial_print(bufprint, sizeof(bufprint));
    memset(bufprint, '\0', sizeof(bufprint));

    sprintf(bufprint, "\rRXB0SIDH = %x\r\n", regId.SIDH); 
    uart_serial_print(bufprint, sizeof(bufprint));
    memset(bufprint, '\0', sizeof(bufprint));

    sprintf(bufprint, "RXB0SIDL = %x\r\n", regId.SIDL); 
    uart_serial_print(bufprint, sizeof(bufprint));
    memset(bufprint, '\0', sizeof(bufprint));

    sprintf(bufprint, "RXB0EID8 = %x\r\n", regId.EID8); 
    uart_serial_print(bufprint, sizeof(bufprint));
    memset(bufprint, '\0', sizeof(bufprint));

    sprintf(bufprint, "RXB0EID0 = %x\r\n", regId.EID0); 
    uart_serial_print(bufprint, sizeof(bufprint));
    memset(bufprint, '\0', sizeof(bufprint));
}

void testRegisterWrite(void)
{ 
    uint8_t writeByte = 0; 
    uint8_t readByte = 0; 
    char buf[40]; 
    // memset(nullData, '\0', sizeof(nullData)); 

    // MCP2515_WriteMultipleBytes(MCP2515_TXB0SIDH, nullData, sizeof(nullData)); 
    // canspi_ReadTxRegisterPrint(); 
    // MCP2515_WriteMultipleBytes(MCP2515_TXB0SIDH, regData, sizeof(regData)); 
    // canspi_ReadTxRegisterPrint(); 
    sprintf(buf, "****** START ******\r\n\n"); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    // for(uint8_t i = 0x00; i < 0xFF; i++)
    // {  
    //     MCP2515_WriteByte(MCP2515_TXB0SIDL, i); 
    //     readByte = MCP2515_ReadByte(MCP2515_TXB0SIDL);
    //     sprintf(buf, "TXB0SIDL = %x\r\n", readByte); 
    //     uart_serial_print(buf, sizeof(buf));
    //     memset(buf, '\0', sizeof(buf));
    // }

    writeByte = 0x08; 
    MCP2515_WriteByte(MCP2515_TXB0SIDH, writeByte);

    writeByte = 0xC8; 
    MCP2515_WriteByte(MCP2515_TXB0SIDL, writeByte);

    readByte = MCP2515_ReadByte(MCP2515_TXB0SIDL);
    sprintf(buf, "\rTXB0SIDL = %x\r\n", readByte); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    writeByte = 0xFE; 
    MCP2515_WriteByte(MCP2515_TXB0EID8, writeByte);
    writeByte = 0xFC; 
    MCP2515_WriteByte(MCP2515_TXB0EID0, writeByte);
    writeByte = 0x01; 
    MCP2515_WriteByte(MCP2515_TXB0DLC, writeByte);
    writeByte = 0xFF; 
    MCP2515_WriteByte(MCP2515_TXB0D0, writeByte);
    // writeByte = 0x01; 
    // MCP2515_WriteByte(MCP2515_TXB0D1, writeByte);
    // writeByte = 0x02; 
    // MCP2515_WriteByte(MCP2515_TXB0D2, writeByte);
    // writeByte = 0x03; 
    // MCP2515_WriteByte(MCP2515_TXB0D3, writeByte);
    // writeByte = 0x04; 
    // MCP2515_WriteByte(MCP2515_TXB0D4, writeByte);
    // writeByte = 0x05; 
    // MCP2515_WriteByte(MCP2515_TXB0D5, writeByte);
    // writeByte = 0x06; 
    // MCP2515_WriteByte(MCP2515_TXB0D6, writeByte);
    // writeByte = 0x07; 
    // MCP2515_WriteByte(MCP2515_TXB0D7, writeByte);

    // canspi_ReadTxRegisterPrint();   
    MCP2515_RequestToSend(MCP2515_RTS_TX0);
    // sprintf(buf, "****** END ******\r\n\r\n"); 
    // uart_serial_print(buf, sizeof(buf));
    // memset(buf, '\0', sizeof(buf));
}

void canspi_ReadRx0RegisterPrint(void)
{
    char buf[34]; 
    uint8_t readByte = 0;  

    sprintf(buf, "\r\n\r\n***RX0 Register Values***\r\n\n");
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    readByte = MCP2515_ReadByte(MCP2515_RXB0SIDH);
    sprintf(buf, "\rRXB0SIDH = %x\r\n", readByte); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    readByte = MCP2515_ReadByte(MCP2515_RXB0SIDL);
    sprintf(buf, "RXB0SIDL = %x\r\n", readByte); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    readByte = MCP2515_ReadByte(MCP2515_RXB0EID8);
    sprintf(buf, "RXB0EID8 = %x\r\n", readByte); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    readByte = MCP2515_ReadByte(MCP2515_RXB0EID0);
    sprintf(buf, "RXB0EID0 = %x\r\n", readByte); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    readByte = MCP2515_ReadByte(MCP2515_RXB0DLC);
    sprintf(buf, "RXB0DLC = %x\r\n", readByte); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    readByte = MCP2515_ReadByte(MCP2515_RXB0D0);
    sprintf(buf, "RXB0D0 = %x\r\n", readByte); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));
}

void canspi_ReadRx1RegisterPrint(void)
{
    char buf[34]; 
    uint8_t readByte = 0;  

    sprintf(buf, "\r\n\r\n***RX1 Register Values***\r\n\n");
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    readByte = MCP2515_ReadByte(MCP2515_RXB1SIDH);
    sprintf(buf, "\rRXB1SIDH = %x\r\n", readByte); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    readByte = MCP2515_ReadByte(MCP2515_RXB1SIDL);
    sprintf(buf, "RXB1SIDL = %x\r\n", readByte); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    readByte = MCP2515_ReadByte(MCP2515_RXB1EID8);
    sprintf(buf, "RXB1EID8 = %x\r\n", readByte); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    readByte = MCP2515_ReadByte(MCP2515_RXB1EID0);
    sprintf(buf, "RXB1EID0 = %x\r\n", readByte); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    readByte = MCP2515_ReadByte(MCP2515_RXB1DLC);
    sprintf(buf, "RXB1DLC = %x\r\n", readByte); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    readByte = MCP2515_ReadByte(MCP2515_RXB1D0);
    sprintf(buf, "RXB1D0 = %x\r\n", readByte); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));
}

void test_PrintTXnCtrl(uint8_t value)
{
    char buf[34]; 
    uint8_t readByte = 0;  

    if((value >= 0) && (value <= 2))
    {
        switch(value)
        {
            case 0: 
                readByte = MCP2515_ReadByte(MCP2515_TXB0CTRL);
                sprintf(buf, "\rTX0Ctrl = %x\r\n", readByte); 
                uart_serial_print(buf, sizeof(buf));
                memset(buf, '\0', sizeof(buf));
                break;
            case 1: 
                readByte = MCP2515_ReadByte(MCP2515_TXB1CTRL);
                sprintf(buf, "\rTX1Ctrl = %x\r\n", readByte); 
                uart_serial_print(buf, sizeof(buf));
                memset(buf, '\0', sizeof(buf));
                break;
            case 2: 
                readByte = MCP2515_ReadByte(MCP2515_TXB2CTRL);
                sprintf(buf, "\rTX2Ctrl = %x\r\n", readByte); 
                uart_serial_print(buf, sizeof(buf));
                memset(buf, '\0', sizeof(buf));
                break; 
        }
    }
}

void test_print_ID(can_ext_id_t tempId)
{
    char printStr[30]; 
    sprintf(printStr, "**** CAN ID ****\r\n"); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "\rpriority = %x\r\n", tempId.frame.priority); 
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

void MCP_test_loopback_function(void)
{   
    can_msg_t tx_message;  
    can_ext_id_t tx_id; 
    tx_id.id = 0; 

    // Load ID
    tx_id.frame.priority = 6; 
    tx_id.frame.edp = 0;
    tx_id.frame.dp = 0; 
    tx_id.frame.pf = 0xFA; 
    tx_id.frame.ps = 0xBC; 
    tx_id.frame.source_address = 0x33; 
    // Load Frame
    tx_message.frame.canId = tx_id.id; 
    tx_message.frame.dlc = 8;
    tx_message.frame.data0 = 0x01; 
    tx_message.frame.data1 = 0x11; 
    tx_message.frame.data2 = 0x21;
    tx_message.frame.data3 = 0x31;
    tx_message.frame.data4 = 0x41;
    tx_message.frame.data5 = 0x51;
    tx_message.frame.data6 = 0x61;
    tx_message.frame.data7 = 0x71; 

    canspi_TransmitMessage(&tx_message);
    
    canspi_ReceiveMessage();
 
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
