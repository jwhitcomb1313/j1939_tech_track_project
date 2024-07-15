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
#include "mcp2515.h"

#define ID_MASK_PRIORITY    0xE0
#define ID_MASK_EDP         0x10
#define ID_MASK_DP          0x08

#define ID_MASK_PF_MSB      0x07
// LSB Upper Byte
#define ID_MASK_PF_LSBUB    0xE0
// LSB Lower Byte
#define ID_MASK_PF_LSBLB    0x03

#define REG_MASK_PF_MSB     0xE0  
#define REG_MASK_PF_LSBUB   0x1C
#define REG_MASK_PF_LSBLB   0x03 

#define REG_MASK_IDE        0x08
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
        unsigned pf             : 8;
        unsigned ps             : 8; // Destination Address
        /** ******************** **/
        unsigned source_address : 8;
    }frame;
    uint32_t id; 
}can_ext_id_t;

typedef union 
{
    struct 
    {
        uint32_t    canId;
        uint8_t     dlc;
        uint8_t     data0;
        uint8_t     data1;
        uint8_t     data2;
        uint8_t     data3;
        uint8_t     data4;
        uint8_t     data5;
        uint8_t     data6;
        uint8_t     data7;
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
void canspi_ConvertRegToID(id_reg_t regId, uint32_t *canId);
void canspi_ConvertIDToReg(uint32_t canId, id_reg_t *regId);

// Test functions
void canspi_CanLoopTest(can_msg_t canMsg); 
void canspi_idCheck(uint32_t canId);
#endif /* INC_CAN_SPI_H_ */
