/*
 * j1939.c
 *
 *  Created on: Jul 31, 2024
 *      Author: jwhitcomb
 */

#include "main.h"
#include "j1939.h"

/******************** ********** ************************/
/******************** Variables  ************************/
/******************** ********** ************************/
j1939_buffer_t messageBuffer;

/******************** ********** ************************/
/******************** Prototypes ************************/
/******************** ********** ************************/



/******************** ******************** ***********************/
/******************** Function Definitions ***********************/
/******************** ******************** ***********************/
void j1939_user_init(void)
{
    messageBuffer.count = 0;  
    messageBuffer.head = 0; 
    messageBuffer.newMessage = false; 
}
void j1939_AddJ139MessageToTable(can_msg_t canMsg, can_ext_id_t canId)
{
    j1939_message_id_t message_id; 
    j1939_message_t message; 
    uint16_t pgn = canId.frame.pf; 
    pgn = (pgn << 8) | canId.frame.ps; 

    message.message_id.PGN = pgn; 
    message.message_id.destination_address = canId.frame.ps; 
    message.message_id.source_address = canId.frame.source_address;
    message.message_id.priority = canId.frame.priority; 

    message.length = canMsg.frame.dlc;
    message.data_buffer[0] = canMsg.frame.data0; 
    message.data_buffer[1] = canMsg.frame.data1;
    message.data_buffer[2] = canMsg.frame.data2;
    message.data_buffer[3] = canMsg.frame.data3;
    message.data_buffer[4] = canMsg.frame.data4;
    message.data_buffer[5] = canMsg.frame.data5;
    message.data_buffer[6] = canMsg.frame.data6;
    message.data_buffer[7] = canMsg.frame.data7;

    if(messageBuffer.count != J1939_MAX_BUFFER_SIZE)
    {   
        messageBuffer.message[messageBuffer.head] = message;
        messageBuffer.count++;
        messageBuffer.head++; 
    }
    else
    {
        // Count doesn't increase here, we just overwrite the oldest data
        messageBuffer.head = 0; 
        messageBuffer.message[messageBuffer.head] = message;
    }
}

bool j1939_GetNewMessage(j1939_message_t *newMessage)
{
    bool retVal = false; 
    if(messageBuffer.newMessage == true)
    {
        *newMessage = messageBuffer.message[messageBuffer.head]; 
        messageBuffer.head++;
        retVal = true; 
    }
    return retVal;
}