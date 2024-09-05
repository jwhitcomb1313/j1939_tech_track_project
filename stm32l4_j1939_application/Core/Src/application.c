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
static app_state_machine_t applicationState = APP_STATE_NOT_STARTED; 
static unsigned int txPeriodCounter = 0;
static unsigned int serialPrintCounter = 0;  

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
    {
        serialPrintCounter = 0; 
    }
    txPeriodCounter ++;   
}

void application_StateMachine(void)
{
    char buf[30]; 
    memset(buf, '\0', sizeof(buf));
    j1939_message_t newMessage; 
    if(j1939_PullMessageFromTable(&newMessage))
    { 
        switch(applicationState)
        {
            case APP_STATE_NOT_STARTED:
                // Determine if the message has the correct pgn for the state
                if(j1939_PGNCompare(newMessage.message_id.PGN, STATE_0_PGN))
                {
                    // Determine if the data is valid and in the correct position
                    if(newMessage.data_buffer[STATE_0_RX_POS] == STATE_0_RX_DATA)
                    {
                        applicationState = APP_STATE_1;
                        sprintf(buf, "*******************\r\n" ); 
                        uart_serial_print(buf, sizeof(buf));
                        memset(buf, '\0', sizeof(buf));
                        sprintf(buf, "App State 1\r\n"); 
                        uart_serial_print(buf, sizeof(buf));
                        memset(buf, '\0', sizeof(buf));
                        sprintf(buf, "*******************\r\n" ); 
                        uart_serial_print(buf, sizeof(buf));
                        memset(buf, '\0', sizeof(buf));
                    }
                }
                break; 
            case APP_STATE_1: 
                // Determine if the message has the correct pgn for the state
                if(j1939_PGNCompare(newMessage.message_id.PGN, STATE_1_PGN))
                {
                    // Determine if the data is valid and in the correct position
                    if(newMessage.data_buffer[STATE_1_RX_POS] == STATE_1_RX_DATA)
                    {
                        applicationState = APP_STATE_2;

                        sprintf(buf, "*******************\r\n" ); 
                        uart_serial_print(buf, sizeof(buf));
                        memset(buf, '\0', sizeof(buf));
                        sprintf(buf, "App State 2\r\n"); 
                        uart_serial_print(buf, sizeof(buf));
                        memset(buf, '\0', sizeof(buf));
                        sprintf(buf, "*******************\r\n" ); 
                        uart_serial_print(buf, sizeof(buf));
                        memset(buf, '\0', sizeof(buf));
                    } 
                }
                break;
            case APP_STATE_2:   
                // Determine if the message has the correct pgn for the state
                if(j1939_PGNCompare(newMessage.message_id.PGN, STATE_2_PGN))
                {
                    // Determine if the data is valid and in the correct position
                    if(((newMessage.data_buffer[STATE_2_RX_POS] & (0x0F)) >> 2) == STATE_2_RX_DATA)
                    {
                        applicationState = APP_STATE_3;

                        sprintf(buf, "*******************\r\n" ); 
                        uart_serial_print(buf, sizeof(buf));
                        memset(buf, '\0', sizeof(buf));
                        sprintf(buf, "App State 3\r\n"); 
                        uart_serial_print(buf, sizeof(buf));
                        memset(buf, '\0', sizeof(buf));
                        sprintf(buf, "*******************\r\n" ); 
                        uart_serial_print(buf, sizeof(buf));
                        memset(buf, '\0', sizeof(buf));
                    } 
                }
                break;
            case APP_STATE_3: 
                // Determine if the message has the correct pgn for the state
                if(j1939_PGNCompare(newMessage.message_id.PGN, STATE_3_PGN))
                {
                    uint16_t data = newMessage.data_buffer[6] << 8; 
                    data |= newMessage.data_buffer[5]; 
                    // Determine if the data is valid and in the correct position
                    if(data == STATE_3_RX_DATA)
                    {
                        applicationState = APP_STATE_4;

                        sprintf(buf, "*******************\r\n" ); 
                        uart_serial_print(buf, sizeof(buf));
                        memset(buf, '\0', sizeof(buf));
                        sprintf(buf, "App State 4\r\n"); 
                        uart_serial_print(buf, sizeof(buf));
                        memset(buf, '\0', sizeof(buf));
                        sprintf(buf, "*******************\r\n" ); 
                        uart_serial_print(buf, sizeof(buf));
                        memset(buf, '\0', sizeof(buf));
                    } 
                }
                break;
            case APP_STATE_4: 
                // Determine if the message has the correct pgn for the state
                if(j1939_PGNCompare(newMessage.message_id.PGN, STATE_4_PGN))
                {
                    uint32_t data = newMessage.data_buffer[7] << 24; 
                    data |= (newMessage.data_buffer[6] << 16); 
                    data |= (newMessage.data_buffer[5] << 8);
                    data |= (newMessage.data_buffer[4] << 0);
                    // Determine if the data is valid and in the correct position
                    if(data == STATE_4_RX_DATA)
                    {
                        applicationState = APP_STATE_NOT_STARTED;

                        sprintf(buf, "*******************\r\n" ); 
                        uart_serial_print(buf, sizeof(buf));
                        memset(buf, '\0', sizeof(buf));
                        sprintf(buf, "App State Success\r\n"); 
                        uart_serial_print(buf, sizeof(buf));
                        memset(buf, '\0', sizeof(buf));
                        sprintf(buf, "*******************\r\n" ); 
                        uart_serial_print(buf, sizeof(buf));
                        memset(buf, '\0', sizeof(buf));
                    } 
                }
                break;
            case APP_STATE_SUCCESS:
                applicationState = APP_STATE_NOT_STARTED; 
                break;
        }
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
