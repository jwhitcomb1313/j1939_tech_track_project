/*
 * j1939.h
 *
 *  Created on: Jul 31, 2024
 *      Author: jwhitcomb
 */

#ifndef INC_J1939_H_
#define INC_J1939_H_

/******************** ********** ************************/
/********************* Includes *************************/
/******************** ********** ************************/
#include "main.h"
#include "stm32l432xx.h"
#include <stdbool.h>
#include "can_spi.h"

/******************** ********** ************************/
/********************** Defines *************************/
/******************** ********** ************************/

#define J1939_RX_MSG_DATA_SIZE          (8)            // J1939 Rx message data size, in bytes
#define J1939_TX_MSG_DATA_SIZE          (8)            // J1939 Tx message data size, in bytes
#define J1939_MAX_BUFFER_SIZE           30

/******************** ********** ************************/
/********************** Structs *************************/
/******************** ********** ************************/
/*!
    \brief      This is J1939 Message ID structure.\n
                This structure is used to define each J1939 Message ID with the following parameters:\n
                    - PGN: The PGN of the message.\n
                    - destination_address: The destination address of the message.\n
                    - source_address: The source address of the message.\n
                    - priority: The J1939 priority of the message.
    \ingroup    can
    @{
 */
typedef struct
{
    uint16_t               PGN;
    uint8_t                destination_address;
    uint8_t                source_address;
    uint8_t                priority;
} j1939_message_id_t;
/*! @} */

/*!
    \brief      This is J1939 Message structure.\n
                This structure is used to define each J1939 Tx Message with the following parameters:\n
                    - data_buffer[]: This buffer stores the data that needs to be transmitted.\n
                    - message_id: The J1939_message_id_t message ID.\n
                    - length: The length of the data.
                    - period: The period that this message is transmitted, in ms.
    \ingroup    can
    @{
 */
typedef struct
{
    j1939_message_id_t     message_id;
    uint8_t                data_buffer[ J1939_TX_MSG_DATA_SIZE ];
    uint8_t                length;
} j1939_message_t;
/*! @} */


/*!
    \brief      This is J1939 Tx Message structure.\n
                This structure is used to define each J1939 Tx Message with the following parameters:\n
                    -
    \ingroup    can
    @{
 */
typedef struct 
{
    j1939_message_t message[30]; 
    uint8_t count; 
    uint8_t head;  
    bool newMessage; 
} j1939_buffer_t;





/******************** ********** ************************/
/******************** Prototypes ************************/
/******************** ********** ************************/
void j1939_AddJ139MessageToTable(can_msg_t canMsg, can_ext_id_t canId);
bool j1939_GetNewMessage(j1939_message_t *newMessage)

















































// /*!
//     \brief      This is J1939 Tx Message Table structure.\n
//                 This structure is used to define each message in the J1939 Tx Message table with the following parameters:\n
//                     - tx_message: The Tx message object. See structure J1939_tx_message_t.
//                     - tx_callback: The function to call before sending the J1939 data This allows the application to update the buffer.
//     \ingroup    can
//     @{
//  */
// typedef void (* J1939_tx_callback_ptr) ( J1939_stack_t j1939_stack, J1939_tx_message_t * );
// typedef struct
// {
//     J1939_tx_message_t     tx_message;
//     J1939_tx_callback_ptr  tx_update_handler;
// } J1939_tx_msg_table_t;
// /*! @} */

// /*!
//     \brief      This is J1939 Rx Message Table structure.\n
//                 This structure is used to define each message in the J1939 Rx Message table with the following parameters:\n
//                     - message_id: The J1939 message ID of the received message. See structure J1939_message_id_t.\n
//                     - rx_callback: The function to call to send the data to the application.
//     \ingroup    can
//     @{
//  */
// typedef void (* J1939_rx_callback_ptr) ( J1939_stack_t j1939_stack, J1939_rx_message_t * );
// typedef void (* J1939_rx_timeout_callback_ptr) ( J1939_stack_t j1939_stack );
// typedef struct
// {
//     J1939_message_id_t             message_id;
//     uint32_t                       expected_trr;
//     J1939_rx_callback_ptr          rx_callback;
//     J1939_rx_timeout_callback_ptr  rx_timeout_callback;
// } J1939_rx_msg_table_t;
// /*! @} */














#endif /* INC_J1939_H_ */
