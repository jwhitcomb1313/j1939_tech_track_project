/*
 * application.h
 *
 *  Created on: Aug 28, 2024
 *      Author: jwhitcomb
 */

#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

#include "can_spi.h"
#include "j1939.h"

#define TX_TRANSMIT_PERIOD 350
#define SERIAL_PRINT_PERIOD 1000
#define STATE_0_PGN 0xFF0F
#define STATE_1_PGN 0xFF01
#define STATE_2_PGN 0xFF02
#define STATE_3_PGN 0xFF03
#define STATE_4_PGN 0xFF04
#define STATE_5_PGN 0xFF05
#define STATE_6_PGN 0xFF06 

typedef enum 
{
    APP_STATE_NOT_STARTED,
    APP_STATE_1,
    APP_STATE_2,
    APP_STATE_3,
    APP_STATE_4,
    APP_STATE_5,
    APP_STATE_6,
    APP_STATE_SUCCESS,
}app_state_machine_t; 





void application_MainLoopFunction(void);
app_state_machine_t GetApplicationState(void);
void SetApplicationState(app_state_machine_t state);

#endif /* INC_APPLICATION_H_ */
