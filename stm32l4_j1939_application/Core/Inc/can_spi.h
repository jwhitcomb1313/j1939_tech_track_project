/*
 * can_spi.h
 *
 *  Created on: Jun 26, 2024
 *      Author: Justin Whitcomb
 */

#ifndef INC_CAN_SPI_H_
#define INC_CAN_SPI_H_

#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#define STANDARD_CAN_MSG_ID_2_0B    1
#define EXTENDED_CAN_MSG_ID_2_0B    2

/******************* ********** ***********************/
/*******************    Enums   ***********************/
/******************* ********** ***********************/

/** CAN 29 bit extendeded identifier */
typedef union 
{
    struct 
    {
        unsigned priority       : 3;
        /********** PGN ***********/
        unsigned edp            : 1; // Reserved bit always 0
        unsigned dp             : 1; 
        unsigned pdu_format     : 8;
        unsigned pdu_specific   : 8; // Destination Address
        /** ******************** **/
        unsigned source_address : 8;
    };
    unsigned int id; 
}can_ext_id_t;

typedef union 
{
    struct 
    {
        can_ext_id_t    canId;
        uint8_t         dlc;
        uint8_t         data0;
        uint8_t         data1;
        uint8_t         data2;
        uint8_t         data3;
        uint8_t         data4;
        uint8_t         data5;
        uint8_t         data6;
        uint8_t         data7;
    }frame;
    uint8_t array[13];
} can_msg_t;



/******************* ********** ***********************/
/******************* Prototypes ***********************/
/******************* ********** ***********************/
bool canspi_Init(void); 
uint8_t canspi_MessagesInBuffer(void); 
uint8_t canspi_TransmitMessage(can_msg_t *can_message); 
uint8_t canspi_ReceiveMessage(can_msg_t *can_message); 
#endif /* INC_CAN_SPI_H_ */
