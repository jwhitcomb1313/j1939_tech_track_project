/*
 * j1939.c
 *
 *  Created on: Jul 31, 2024
 *      Author: jwhitcomb
 */

#include "j1939.h"
#include "application.h"


/******************** ********** ************************/
/******************** Variables  ************************/
/******************** ********** ************************/
/** 
 * rxMsgCircBuffer is a circular buffer that stores all CAN bus messages, processing in FIFO order. 
 **/
j1939_buffer_t rxMsgCircBuffer;
/** 
 * txPacketList contains all the data that will be sent out on the CAN bus depending on the state. 
 * The main loop will transmit the 'State Not Started' data until the user sends the correct message back
 * then it will progress to the next state.
 * 
 * Notation for SPN = (start bit, bit length)
 **/
j1939_message_t txPacketList[] = 
{
    // State Not Started: SPN = (0, 8)
    {.message_id = { .PGN = STATE_0_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x05, 0x04, 0x08, 0x07, 0x02, 0x08, 0x04, 0x03 } , .length = 8, .isMessageNew = false},
    // State 1: SPN = (0, 8)
    {.message_id = { .PGN = STATE_1_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x37, 0x31, 0x33, 0x36, 0x38, 0x31, 0x39, 0x35 } , .length = 8, .isMessageNew = false},
    // State 2: SPN = (24, 2)
    {.message_id = { .PGN = STATE_2_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x01, 0x04, 0x03, 0x02, 0x05, 0x0F, 0x0A, 0x02 } , .length = 8, .isMessageNew = false},
    // State 3: SPN = (24, 16)
    {.message_id = { .PGN = STATE_3_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x44, 0x02, 0x62, 0x23, 0x10, 0x86, 0x34, 0x19 } , .length = 8, .isMessageNew = false},
    // State 4: SPN = (0, 32)
    {.message_id = { .PGN = STATE_4_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x78, 0x56, 0x34, 0x12, 0x14, 0x93, 0x52, 0x85 } , .length = 8, .isMessageNew = false},
};

/** 
 * rxPacketArray contains all the data that is expected from the user on the CAN bus depending on the state.
 * 
 * Notation for SPN = (start bit, bit length)
 **/
j1939_message_t rxPacketList[] = 
{
    // State Not Started: SPN = (8, 8)
    {.message_id = { .PGN = STATE_0_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } , .length = 8, .isMessageNew = false},
    // State 1: SPN = (8, 8)
    {.message_id = { .PGN = STATE_1_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } , .length = 8, .isMessageNew = false},
    // State 2 SPN = (26, 2)
    {.message_id = { .PGN = STATE_2_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00 } , .length = 8, .isMessageNew = false},
    // State 3: SPN = (32, 16)
    {.message_id = { .PGN = STATE_3_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x00, 0x00, 0x00, 0x00, 0x00, 0x23, 0x10, 0x00 } , .length = 8, .isMessageNew = false},
    // State 4: SPN = (32, 32)
    {.message_id = { .PGN = STATE_4_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x00, 0x00, 0x00, 0x00, 0x9B, 0x66, 0x34, 0x12 } , .length = 8, .isMessageNew = false},
};


/******************** ********** ************************/
/******************** Prototypes ************************/
/******************** ********** ************************/



/******************** ******************** ***********************/
/******************** Function Definitions ***********************/
/******************** ******************** ***********************/
/******************************************************************************/
/*!
   \fn      void j1939_user_init(void)
   \brief   This function inits the j1939 file
   \param   None
   \return  None

    @{
*/
/******************************************************************************/
void j1939_user_init(void)
{ 
    rxMsgCircBuffer.head = 0; 
    rxMsgCircBuffer.tail = 0; 

    for(int i = 0; i < J1939_MAX_BUFFER_SIZE; i++)
    {
        rxMsgCircBuffer.message[i].isMessageNew = false; 
    }
}

/******************************************************************************/
/*!
   \fn      void j1939_AddMessageToTable(j1939_message_t newMessage)
   \brief   This function adds j1939 messages to the table
   \param   j1939_message_t newMessage: j1939 message to add to the table
   \return  None

    @{
*/
/******************************************************************************/
void j1939_AddMessageToTable(j1939_message_t newMessage)
{
    if(rxMsgCircBuffer.tail != (J1939_MAX_BUFFER_SIZE - 1))
    {   
        rxMsgCircBuffer.message[rxMsgCircBuffer.tail] = newMessage;
        rxMsgCircBuffer.message[rxMsgCircBuffer.tail].isMessageNew = true; 
        rxMsgCircBuffer.tail++;  
    }
    else
    {
        rxMsgCircBuffer.message[rxMsgCircBuffer.tail] = newMessage;
        rxMsgCircBuffer.message[rxMsgCircBuffer.tail].isMessageNew = true; 
        rxMsgCircBuffer.tail = 0; 
    }
}

/******************************************************************************/
/*!
   \fn      bool j1939_PullMessageFromTable(j1939_message_t *storedMessage)
   \brief   This function pulls a j1939 message from the table
   \param   j1939_message_t *storedMessage: Pointer to a j1939 message to 
            store from the table. Table operates with FIFO logic
   \return  bool retVal: Returns true if a message was pulled and stored

    @{
*/
/******************************************************************************/
bool j1939_PullMessageFromTable(j1939_message_t *storedMessage)
{
    bool retVal = false; 
    if(j1939_AnyNewMessages())
    {
        if(rxMsgCircBuffer.head != (J1939_MAX_BUFFER_SIZE - 1))
        {
            *storedMessage = rxMsgCircBuffer.message[rxMsgCircBuffer.head]; 
            rxMsgCircBuffer.message[rxMsgCircBuffer.head].isMessageNew = false; 
            canspi_CanLoopTest(rxMsgCircBuffer.message[rxMsgCircBuffer.head]);
            rxMsgCircBuffer.head++;  
        }
        else
        {
            *storedMessage = rxMsgCircBuffer.message[rxMsgCircBuffer.head]; 
            rxMsgCircBuffer.message[rxMsgCircBuffer.head].isMessageNew = false;
            canspi_CanLoopTest(rxMsgCircBuffer.message[rxMsgCircBuffer.head]); 
            rxMsgCircBuffer.head = 0;   
        }
        retVal = true;
    }
    return retVal;
}

/******************************************************************************/
/*!
   \fn      bool j1939_AnyNewMessages(void)
   \brief   This function checks for new messages in the buffer
   \param   None.
   \return  bool retVal: Returns true if there are any messages in the buffer

    @{
*/
/******************************************************************************/
bool j1939_AnyNewMessages(void)
{
    bool retVal = false; 

    for(int i = 0; i < J1939_MAX_BUFFER_SIZE; i++)
    {
        if(rxMsgCircBuffer.message[i].isMessageNew == true)
        {
            retVal = true; 
            break; 
        }
    }
    return retVal; 
}

/******************************************************************************/
/*!
   \fn      void j1939_TxSendPacket(void)
   \brief   This function will send out a j1939 message based on the
            current state of the state machine. 
   \param   None.
   \return  None. 

    @{
*/
/******************************************************************************/
void j1939_TxSendPacket(void)
{
    can_msg_t canMsg; 
    can_ext_id_t canId; 
    app_state_machine_t state = GetApplicationState(); 
    j1939_message_t txPacket = txPacketList[state];
    
    // Load up the ID
    canId.frame.priority = 6; 
    canId.frame.edp = 0;
    canId.frame.dp = 0; 
    canId.frame.pf = ((txPacket.message_id.PGN & 0xFF00) >> 8);
    canId.frame.ps = ((txPacket.message_id.PGN & 0x00FF) >> 0);
    canId.frame.source_address = txPacket.message_id.source_address; 
    // Load up the data
    canMsg.frame.canId = canId.id;
    canMsg.frame.dlc = txPacket.length;  
    canMsg.frame.data0 = txPacket.data_buffer[0]; 
    canMsg.frame.data1 = txPacket.data_buffer[1];
    canMsg.frame.data2 = txPacket.data_buffer[2];
    canMsg.frame.data3 = txPacket.data_buffer[3];
    canMsg.frame.data4 = txPacket.data_buffer[4];
    canMsg.frame.data5 = txPacket.data_buffer[5];
    canMsg.frame.data6 = txPacket.data_buffer[6];
    canMsg.frame.data7 = txPacket.data_buffer[7];

    canspi_TransmitMessage(&canMsg);
}

/******************************************************************************/
/*!
   \fn      void j1939_RxReceivePacket(void)
   \brief   This is a wrapper function for canspi_ReceiveMessage() 
   \param   None.
   \return  None. 

    @{
*/
/******************************************************************************/
void j1939_RxReceivePacket(void)
{
    canspi_ReceiveMessage(); 
}

/******************************************************************************/
/*!
   \fn      bool j1939_PGNCompare(uint16_t pgn1, uint16_t pgn2)
   \brief   This function compares two PGNs 
   \param   uint16_t pgn1
   \param   uint16_t pgn2
   \return  bool retVal: Returns true if they are the same value 

    @{
*/
/******************************************************************************/
bool j1939_PGNCompare(uint16_t pgn1, uint16_t pgn2)
{
    bool retVal = false; 
    if(pgn1 == pgn2)
    {
        retVal = true; 
    }
    return retVal; 
}

void canspi_CanLoopTest(j1939_message_t canMsg)
{
    char printStr[30]; 
    char buf[30]; 
    char pgn_buf[] = "pgn = "; 
    char source_buf[] = "source address = "; 
    memset(printStr, '\0', sizeof(printStr));
    sprintf(printStr, "**** CAN ID ****\r\n"); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    memset(buf, '\0', sizeof(buf));
    // sprintf(buf, "pgn = %x\r\n", canMsg.message_id.PGN);
    sprintf(buf, "%s %x\r\n", pgn_buf, canMsg.message_id.PGN); 
    uart_serial_print(buf, sizeof(buf));
    memset(buf, '\0', sizeof(buf));

    sprintf(printStr, "source address = %x\r\n\r\n", canMsg.message_id.source_address); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "**** DATA ****\r\n"); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr)); 

    sprintf(printStr, "dlc = %x\r\n", canMsg.length); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "data0 = %x\r\n", canMsg.data_buffer[0]); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "data1 = %x\r\n", canMsg.data_buffer[1]); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "data2 = %x\r\n", canMsg.data_buffer[2]); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "data3 = %x\r\n", canMsg.data_buffer[3]); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "data4 = %x\r\n", canMsg.data_buffer[4]); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "data5 = %x\r\n", canMsg.data_buffer[5]); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "data6 = %x\r\n", canMsg.data_buffer[6]); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    sprintf(printStr, "data7 = %x\r\n\r\n", canMsg.data_buffer[7]); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

    // unsigned int    data = canMsg.data_buffer[4] << 24; 
    //                 data |= (canMsg.data_buffer[5] << 16); 
    //                 data |= (canMsg.data_buffer[6] << 8);
    //                 data |= (canMsg.data_buffer[7] << 0);

    // sprintf(printStr, "Data base 10 = %u\r\n\r\n", data); 
    // uart_serial_print(printStr, sizeof(printStr));
    // memset(printStr, '\0', sizeof(printStr));

    }