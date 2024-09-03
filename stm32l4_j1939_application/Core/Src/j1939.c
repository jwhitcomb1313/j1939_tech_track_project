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
 **/
j1939_message_t txPacketList[] = 
{
    // State Not Started
    {.message_id = { .PGN = STATE_0_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } , .length = 8, .isMessageNew = false},
    // State 1
    {.message_id = { .PGN = STATE_1_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } , .length = 8, .isMessageNew = false},
    // State 2
    {.message_id = { .PGN = STATE_2_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } , .length = 8, .isMessageNew = false},
    // State 3
    {.message_id = { .PGN = STATE_3_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } , .length = 8, .isMessageNew = false},
    // State 4
    {.message_id = { .PGN = STATE_4_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } , .length = 8, .isMessageNew = false},
    // State 5
    {.message_id = { .PGN = STATE_5_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } , .length = 8, .isMessageNew = false},
    // State 6
    {.message_id = { .PGN = STATE_6_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } , .length = 8, .isMessageNew = false},
};

/** 
 * rxPacketArray contains all the data that is expected from the user on the CAN bus depending on the state. 
 **/
j1939_message_t rxPacketList[] = 
{
    // State Not Started
    {.message_id = { .PGN = STATE_0_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } , .length = 8, .isMessageNew = false},
    // State 1
    {.message_id = { .PGN = STATE_1_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } , .length = 8, .isMessageNew = false},
    // State 2
    {.message_id = { .PGN = STATE_2_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } , .length = 8, .isMessageNew = false},
    // State 3
    {.message_id = { .PGN = STATE_3_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } , .length = 8, .isMessageNew = false},
    // State 4
    {.message_id = { .PGN = STATE_4_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } , .length = 8, .isMessageNew = false},
    // State 5
    {.message_id = { .PGN = STATE_5_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } , .length = 8, .isMessageNew = false},
    // State 6
    {.message_id = { .PGN = STATE_6_PGN, .destination_address = 0x00, .source_address = 0x33, .priority = 6 }, 
    .data_buffer = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } , .length = 8, .isMessageNew = false},
};
/******************** ********** ************************/
/******************** Prototypes ************************/
/******************** ********** ************************/



/******************** ******************** ***********************/
/******************** Function Definitions ***********************/
/******************** ******************** ***********************/
void j1939_user_init(void)
{ 
    rxMsgCircBuffer.head = 0; 
    rxMsgCircBuffer.tail = 0; 
}

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

bool j1939_PullMessageFromTable(j1939_message_t *storedMessage)
{
    bool retVal = false; 
    if(j1939_AnyNewMessages())
    {
        if(rxMsgCircBuffer.head != (J1939_MAX_BUFFER_SIZE - 1))
        {
            *storedMessage = rxMsgCircBuffer.message[rxMsgCircBuffer.head]; 
            rxMsgCircBuffer.message[rxMsgCircBuffer.head].isMessageNew = false; 
            rxMsgCircBuffer.head++;  
            
            canspi_CanLoopTest(rxMsgCircBuffer.message[rxMsgCircBuffer.head]); 
        }
        else
        {
            *storedMessage = rxMsgCircBuffer.message[rxMsgCircBuffer.head]; 
            rxMsgCircBuffer.message[rxMsgCircBuffer.head].isMessageNew = false; 
            rxMsgCircBuffer.head = 0;

            canspi_CanLoopTest(rxMsgCircBuffer.message[rxMsgCircBuffer.head]);
        }
        retVal = true;
    }
    return retVal;
}

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

void j1939_RxReceivePacket(void)
{
    canspi_ReceiveMessage(); 
}
bool j1939_PGNCompare(uint16_t pgn1, uint16_t pgn2)
{
    bool retVal = false; 
    if(pgn1 == pgn2)
    {
        retVal = true; 
    }
    return retVal; 
}




void test_circ_buf(void)
{
    char buf[30]; 
    // bool newMessage = j1939_AnyNewMessages(); 
    // sprintf(buf, "newMessage = %s\r\n", newMessage ? "true" : "false" ); 
    // uart_serial_print(buf, sizeof(buf));
    // memset(buf, '\0', sizeof(buf));
    if(j1939_AnyNewMessages())
    {
        sprintf(buf, "*** New Message ***\r\n" ); 
        uart_serial_print(buf, sizeof(buf));
        memset(buf, '\0', sizeof(buf));

        sprintf(buf, "Head = %u\r\n", rxMsgCircBuffer.head); 
        uart_serial_print(buf, sizeof(buf));
        memset(buf, '\0', sizeof(buf));

        sprintf(buf, "Tail = %u\r\n", rxMsgCircBuffer.tail); 
        uart_serial_print(buf, sizeof(buf));
        memset(buf, '\0', sizeof(buf));
        sprintf(buf, "*******************\r\n" ); 
        uart_serial_print(buf, sizeof(buf));
        memset(buf, '\0', sizeof(buf));
    }
}
void canspi_CanLoopTest(j1939_message_t canMsg)
{
    char printStr[30]; 

    sprintf(printStr, "pgn = %x\r\n", canMsg.message_id.PGN); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));

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

    sprintf(printStr, "data7 = %x\r\n\n", canMsg.data_buffer[7]); 
    uart_serial_print(printStr, sizeof(printStr));
    memset(printStr, '\0', sizeof(printStr));
}