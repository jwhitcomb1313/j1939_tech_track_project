/*
 * mcp2515.h
 *
 *  Created on: 6/21/2024
 *      Author: Justin Whitcomb
 */
#ifndef SRC_MCP2515_H_
#define SRC_MCP2515_H_

#include "main.h"
#include "stm32l432xx.h"
#include <stdbool.h>
/* MCP2515 SPI Instruction Set */
#define MCP2515_RESET           0xC0

#define MCP2515_READ            0x03
#define MCP2515_READ_RXB0SIDH   0x90
#define MCP2515_READ_RXB0D0     0x92
#define MCP2515_READ_RXB1SIDH   0x94
#define MCP2515_READ_RXB1D0     0x96

#define MCP2515_WRITE           0x02
#define MCP2515_LOAD_TXB0SIDH   0x40    /* TX0 ID location */
#define MCP2515_LOAD_TXB0D0     0x41    /* TX0 Data location */
#define MCP2515_LOAD_TXB1SIDH   0x42    /* TX1 ID location */
#define MCP2515_LOAD_TXB1D0     0x43    /* TX1 Data location */
#define MCP2515_LOAD_TXB2SIDH   0x44    /* TX2 ID location */
#define MCP2515_LOAD_TXB2D0     0x45    /* TX2 Data location */

#define MCP2515_RTS_TX0         0x81
#define MCP2515_RTS_TX1         0x82
#define MCP2515_RTS_TX2         0x84
#define MCP2515_RTS_ALL         0x87
#define MCP2515_READ_STATUS     0xA0
#define MCP2515_RX_STATUS       0xB0
#define MCP2515_BIT_MOD         0x05

/* MCP25152515 Register Adresses */
#define MCP2515_RXF0SIDH	0x00
#define MCP2515_RXF0SIDL	0x01
#define MCP2515_RXF0EID8	0x02
#define MCP2515_RXF0EID0	0x03
#define MCP2515_RXF1SIDH	0x04
#define MCP2515_RXF1SIDL	0x05
#define MCP2515_RXF1EID8	0x06
#define MCP2515_RXF1EID0	0x07
#define MCP2515_RXF2SIDH	0x08
#define MCP2515_RXF2SIDL	0x09
#define MCP2515_RXF2EID8	0x0A
#define MCP2515_RXF2EID0	0x0B
#define MCP2515_CANSTAT		0x0E
#define MCP2515_CANCTRL		0x0F

#define MCP2515_RXF3SIDH	0x10
#define MCP2515_RXF3SIDL	0x11
#define MCP2515_RXF3EID8	0x12
#define MCP2515_RXF3EID0	0x13
#define MCP2515_RXF4SIDH	0x14
#define MCP2515_RXF4SIDL	0x15
#define MCP2515_RXF4EID8	0x16
#define MCP2515_RXF4EID0	0x17
#define MCP2515_RXF5SIDH	0x18
#define MCP2515_RXF5SIDL	0x19
#define MCP2515_RXF5EID8	0x1A
#define MCP2515_RXF5EID0	0x1B
#define MCP2515_TEC		    0x1C
#define MCP2515_REC		    0x1D

#define MCP2515_RXM0SIDH	0x20
#define MCP2515_RXM0SIDL	0x21
#define MCP2515_RXM0EID8	0x22
#define MCP2515_RXM0EID0	0x23
#define MCP2515_RXM1SIDH	0x24
#define MCP2515_RXM1SIDL	0x25
#define MCP2515_RXM1EID8	0x26
#define MCP2515_RXM1EID0	0x27
#define MCP2515_CNF3		0x28
#define MCP2515_CNF2		0x29
#define MCP2515_CNF1		0x2A
#define MCP2515_CANINTE		0x2B
#define MCP2515_CANINTF		0x2C
#define MCP2515_EFLG		0x2D

#define MCP2515_TXB0CTRL	0x30
#define MCP2515_TXB1CTRL	0x40
#define MCP2515_TXB2CTRL	0x50
#define MCP2515_RXB0CTRL	0x60
#define MCP2515_RXB0SIDH	0x61
#define MCP2515_RXB1CTRL	0x70
#define MCP2515_RXB1SIDH	0x71

/* Defines for Rx Status */
#define MSG_IN_RXB0             0x01
#define MSG_IN_RXB1             0x02
#define MSG_IN_BOTH_BUFFERS     0x03


#define MCP2515_CS_HIGH()   HAL_GPIO_WritePin(SP1_CS_GPIO_Port, SP1_CS_Pin, GPIO_PIN_SET)
#define MCP2515_CS_LOW()    HAL_GPIO_WritePin(SP1_CS_GPIO_Port, SP1_CS_Pin, GPIO_PIN_RESET)
#define SPI_TIMEOUT 100

#define MCP2515_OPMODE_MASK   0XE0

/******************* ********** ***********************/
/*******************    Enums   ***********************/
/******************* ********** ***********************/
typedef enum 
{
    SPI_MCP2515_OK,
    SPI_MCP2515_ERROR
}spi_mcp2515_t; 

typedef enum 
{
    MODE_NORMAL = 0x00,
    MODE_LOOPBACK = 0x40,
    MODE_CONFIGURATION = 0x80,
}mcp2515_mode_of_operation_t;
 
typedef enum
{
    INSTRUCTION_WRITE       = 0x02,
    INSTRUCTION_READ        = 0x03,
    INSTRUCTION_BITMOD      = 0x05,
    INSTRUCTION_LOAD_TX0    = 0x40,
    INSTRUCTION_LOAD_TX1    = 0x42,
    INSTRUCTION_LOAD_TX2    = 0x44,
    INSTRUCTION_RTS_TX0     = 0x81,
    INSTRUCTION_RTS_TX1     = 0x82,
    INSTRUCTION_RTS_TX2     = 0x84,
    INSTRUCTION_RTS_ALL     = 0x87,
    INSTRUCTION_READ_RX0    = 0x90,
    INSTRUCTION_READ_RX1    = 0x94,
    INSTRUCTION_READ_STATUS = 0xA0,
    INSTRUCTION_RX_STATUS   = 0xB0,
    INSTRUCTION_RESET       = 0xC0
}mcp2515_instruction_t;

typedef enum 
{
    MCP_RXF0SIDH = 0x00,
    MCP_RXF0SIDL = 0x01,
    MCP_RXF0EID8 = 0x02,
    MCP_RXF0EID0 = 0x03,
    MCP_RXF1SIDH = 0x04,
    MCP_RXF1SIDL = 0x05,
    MCP_RXF1EID8 = 0x06,
    MCP_RXF1EID0 = 0x07,
    MCP_RXF2SIDH = 0x08,
    MCP_RXF2SIDL = 0x09,
    MCP_RXF2EID8 = 0x0A,
    MCP_RXF2EID0 = 0x0B,
    MCP_CANSTAT  = 0x0E,
    MCP_CANCTRL  = 0x0F,
    MCP_RXF3SIDH = 0x10,
    MCP_RXF3SIDL = 0x11,
    MCP_RXF3EID8 = 0x12,
    MCP_RXF3EID0 = 0x13,
    MCP_RXF4SIDH = 0x14,
    MCP_RXF4SIDL = 0x15,
    MCP_RXF4EID8 = 0x16,
    MCP_RXF4EID0 = 0x17,
    MCP_RXF5SIDH = 0x18,
    MCP_RXF5SIDL = 0x19,
    MCP_RXF5EID8 = 0x1A,
    MCP_RXF5EID0 = 0x1B,
    MCP_TEC      = 0x1C,
    MCP_REC      = 0x1D,
    MCP_RXM0SIDH = 0x20,
    MCP_RXM0SIDL = 0x21,
    MCP_RXM0EID8 = 0x22,
    MCP_RXM0EID0 = 0x23,
    MCP_RXM1SIDH = 0x24,
    MCP_RXM1SIDL = 0x25,
    MCP_RXM1EID8 = 0x26,
    MCP_RXM1EID0 = 0x27,
    MCP_CNF3     = 0x28,
    MCP_CNF2     = 0x29,
    MCP_CNF1     = 0x2A,
    MCP_CANINTE  = 0x2B,
    MCP_CANINTF  = 0x2C,
    MCP_EFLG     = 0x2D,
    MCP_TXB0CTRL = 0x30,
    MCP_TXB0SIDH = 0x31,
    MCP_TXB0SIDL = 0x32,
    MCP_TXB0EID8 = 0x33,
    MCP_TXB0EID0 = 0x34,
    MCP_TXB0DLC  = 0x35,
    MCP_TXB0DATA = 0x36,
    MCP_TXB1CTRL = 0x40,
    MCP_TXB1SIDH = 0x41,
    MCP_TXB1SIDL = 0x42,
    MCP_TXB1EID8 = 0x43,
    MCP_TXB1EID0 = 0x44,
    MCP_TXB1DLC  = 0x45,
    MCP_TXB1DATA = 0x46,
    MCP_TXB2CTRL = 0x50,
    MCP_TXB2SIDH = 0x51,
    MCP_TXB2SIDL = 0x52,
    MCP_TXB2EID8 = 0x53,
    MCP_TXB2EID0 = 0x54,
    MCP_TXB2DLC  = 0x55,
    MCP_TXB2DATA = 0x56,
    MCP_RXB0CTRL = 0x60,
    MCP_RXB0SIDH = 0x61,
    MCP_RXB0SIDL = 0x62,
    MCP_RXB0EID8 = 0x63,
    MCP_RXB0EID0 = 0x64,
    MCP_RXB0DLC  = 0x65,
    MCP_RXB0DATA = 0x66,
    MCP_RXB1CTRL = 0x70,
    MCP_RXB1SIDH = 0x71,
    MCP_RXB1SIDL = 0x72,
    MCP_RXB1EID8 = 0x73,
    MCP_RXB1EID0 = 0x74,
    MCP_RXB1DLC  = 0x75,
    MCP_RXB1DATA = 0x76
}mcp2515_register_t;

typedef enum 
{
    TX_BUF_TXB0SIDH = 0x31,
    TX_BUF_TXB0DATA = 0x36,
    TX_BUF_TXB1SIDH = 0x41,
    TX_BUF_TXB1DATA = 0x46,
    TX_BUF_TXB2SIDH = 0x51,
    TX_BUF_TXB2DATA = 0x56,
}load_tx_buf_instr_t; 

typedef enum
{
    RX_BUF_RXB0SIDH     = 0x61,
    RX_BUF_RXB0DATA     = 0x66,
    RX_BUF_RXB1SIDH     = 0x71,
    RX_BUF_MCP_RXB1DATA = 0x76
}read_rx_buf_instr_t; 

typedef union
{
  struct
  {
    unsigned RX0IF      : 1; // 0: CANINTF[0]
    unsigned RX1IF      : 1; // 1: CANINTF[1]
    unsigned TXB0REQ    : 1; // 2: TXB0CNTRL[3]
    unsigned TX0IF      : 1; // 3: CANINTF[2]
    unsigned TXB1REQ    : 1; // 4: TXB1CNTRL[3]
    unsigned TX1IF      : 1; // 5: CANINTF[3]
    unsigned TXB2REQ    : 1; // 6: TXB2CNTRL[3]
    unsigned TX2IF      : 1; // 7: CANINTF[4]
  };
  uint8_t ctrl_status;  
}ctrl_status_t;

typedef union
{
  struct
  {
    unsigned filter     : 3;
    unsigned msgType    : 2;
    unsigned unusedBit  : 1;
    unsigned rxBuffer   : 2;
  };
  uint8_t ctrl_rx_status;
}rx_status_t;

typedef union
{
  struct
  {
    uint8_t RXBnSIDH;
    uint8_t RXBnSIDL;
    uint8_t RXBnEID8;
    uint8_t RXBnEID0;
    uint8_t RXBnDLC;
    uint8_t RXBnD0;
    uint8_t RXBnD1;
    uint8_t RXBnD2;
    uint8_t RXBnD3;
    uint8_t RXBnD4;
    uint8_t RXBnD5;
    uint8_t RXBnD6;
    uint8_t RXBnD7;
  };
  uint8_t rx_reg_array[13];
}rx_reg_t;

typedef struct
{
  uint8_t SIDH;
  uint8_t SIDL;
  uint8_t EID8;
  uint8_t EID0;
}id_reg_t;

/******************* ********** ***********************/
/******************* Prototypes ***********************/
/******************* ********** ***********************/
/**  Init/Mode functions **/
bool MCP2515_Init(void); 
bool MCP2515_SetConfigurationMode(void);
bool MCP2515_SetNormalMode(void); 
bool MCP2515_SetLoopbackMode(void);
/**  Write functions **/
void MCP2515_WriteByte(uint8_t address, uint8_t data);
void MCP2515_WriteMultipleBytes(uint8_t address, uint8_t* data, uint8_t length); 
void MCP2515_WriteTxBuffer(load_tx_buf_instr_t instruction, id_reg_t *idReg, uint8_t* data, uint8_t length); 
/**  Read functions **/
uint8_t MCP2515_ReadByte(uint8_t address); 
void MCP2515_ReadMultipleBytes(uint8_t address, uint8_t* data, uint8_t length); 
void MCP2515_ReadRxBuffer(read_rx_buf_instr_t instruction, uint8_t* data, uint8_t length);
/**  Status functions **/
rx_status_t MCP2515_GetRxStatus(void); 
ctrl_status_t MCP2515_GetControlStatus(void); 

 
void MCP_test_function(void);

#endif /* SRC_MCP2515_H_ */
