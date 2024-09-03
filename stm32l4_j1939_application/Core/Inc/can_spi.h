/*
 * can_spi.h
 *
 *  Created on: Jun 26, 2024
 *      Author: Justin Whitcomb
 */

#ifndef INC_CAN_SPI_H_
#define INC_CAN_SPI_H_
#include "j1939.h"
#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include "mcp2515.h"


// ID Bit Masks
#define ID_MASK_PRIORITY    0xE0
#define ID_MASK_EDP         0x10
#define ID_MASK_DP          0x08

#define ID_MASK_PF_MSB      0x07
// LSB Upper Byte
#define ID_MASK_PF_LSBUB    0xE0
// LSB Lower Byte
#define ID_MASK_PF_LSBLB    0x03

#define REG_MASK_PF_MSB     0xE0  
// LSB Upper Byte
#define REG_MASK_PF_LSBUB   0x1C
// LSB Lower Byte
#define REG_MASK_PF_LSBLB   0x03 
#define REG_MASK_IDE        0x08



//Click board has a 10MHz crystal
#define CAN_OSC_FREQUENCY               10000000 
#define MCP2515_CANCTRL_REQOP_MASK      0xE0
#define MCP2515_CANCTRL_REQOP_CONFIG    0x80
#define MCP2515_CNF2_PRSEG_MASK         0x07
#define MCP2515_CNF2_PHSEG_MASK         0x38
#define MCP2515_CNF2_BTLMODE            0x80
#define MCP2515_CNF3_PHSEG_MASK         0x07
/******************* ********** ***********************/
/*******************    Enums   ***********************/
/******************* ********** ***********************/

/** CAN 29 bit extendeded identifier */
typedef union 
{
    struct 
    {
        unsigned priority       : 3;
        unsigned edp            : 1; // Reserved bit always 0
        unsigned dp             : 1; 
        /********** PGN ***********/
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
uint8_t canspi_ReceiveMessage(void); 
void canspi_ConvertRegToID(id_reg_t regId, uint32_t *canId);
void canspi_ConvertIDToReg(uint32_t canId, id_reg_t *regId);
void canspi_readRxBuffer(rx_reg_t *rxData, uint8_t regPosition);


// Test functions
 
void canspi_idCheck(uint32_t canId);
void canspi_ReadTxRegisterPrint(void);
void canspi_ReadRx0RegisterPrint(void);
void canspi_ReadRx1RegisterPrint(void); 
void canspi_ReadRegIdPrint(id_reg_t regId);
void printRegister(id_reg_t regId); 
void testRegisterWrite(void);
void canspi_printTxRxErrorReg(void);
void test_PrintTXnCtrl(uint8_t value);
void MCP_test_loopback_function(void);
#endif /* INC_CAN_SPI_H_ */
