/*
 * application.c
 *
 *  Created on: Aug 28, 2024
 *      Author: jwhitcomb
 */

#include "application.h"
#include "j1939.h"

/******************** ********** ************************/
/******************** Variables  ************************/
/******************** ********** ************************/
app_state_machine_t applicationState = APP_STATE_NOT_STARTED; 
static uint32_t txPeriodCounter = 0;
static uint32_t serialPrintCounter = 0;  

/******************** ********** ************************/
/******************** Prototypes ************************/
/******************** ********** ************************/
void application_StateMachine(void); 

/******************** ******************** ***********************/
/******************** Function Definitions ***********************/
/******************** ******************** ***********************/
void application_MainLoopFunction(void)
{
    // Always ready to receive data 
    j1939_RxReceivePacket(); 

    // Manage state machine
    application_StateMachine();         

    if(txPeriodCounter == TX_TRANSMIT_PERIOD)
    {
        // Send the packet for the current state every 100 ms
        j1939_TxSendPacket(); 
        txPeriodCounter = 0; 
    } 
    if(serialPrintCounter == SERIAL_PRINT_PERIOD)
    txPeriodCounter ++;   
}

void application_StateMachine(void)
{
    j1939_message_t newMessage; 
    j1939_PullMessageFromTable(&newMessage); 

    switch(applicationState)
    {
        case APP_STATE_NOT_STARTED:
            // Determine if the message has the correct pgn for the state
            if(j1939_PGNCompare(newMessage.message_id.PGN, STATE_0_PGN))
            {
                applicationState = APP_STATE_1; 
            }
            break; 
        case APP_STATE_1: 
            // Determine if the message has the correct pgn for the state
            if(j1939_PGNCompare(newMessage.message_id.PGN, STATE_1_PGN))
            {
                applicationState = APP_STATE_2; 
            }
            break;
        case APP_STATE_2:   
            // Determine if the message has the correct pgn for the state
            if(j1939_PGNCompare(newMessage.message_id.PGN, STATE_2_PGN))
            {
                applicationState = APP_STATE_3; 
            }
            break;
        case APP_STATE_3: 
            // Determine if the message has the correct pgn for the state
            if(j1939_PGNCompare(newMessage.message_id.PGN, STATE_3_PGN))
            {
                applicationState = APP_STATE_4; 
            }
            break;
        case APP_STATE_4: 
            // Determine if the message has the correct pgn for the state
            if(j1939_PGNCompare(newMessage.message_id.PGN, STATE_4_PGN))
            {
                applicationState = APP_STATE_5; 
            }
            break;
        case APP_STATE_5: 
            // Determine if the message has the correct pgn for the state
            if(j1939_PGNCompare(newMessage.message_id.PGN, STATE_5_PGN))
            {
                applicationState = APP_STATE_6; 
            }
            break;
        case APP_STATE_6: 
            // Determine if the message has the correct pgn for the state
            if(j1939_PGNCompare(newMessage.message_id.PGN, STATE_6_PGN))
            {
                applicationState = APP_STATE_SUCCESS; 
            }
            break;
        case APP_STATE_SUCCESS:
            break;

    }
}

app_state_machine_t GetApplicationState(void)
{
    return applicationState; 
}

void SetApplicationState(app_state_machine_t state)
{
    applicationState = state; 
}
