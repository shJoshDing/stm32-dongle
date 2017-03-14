#ifndef __I2C_H
#define __I2C_H
//==============================================================================
// S E N S I R I O N AG, Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project : SHT2x Sample Code (V1.2)
// File : SHT2x.h
// Author : MST
// Controller: NEC V850/SG3 (uPD70F3740)
// Compiler : IAR compiler for V850 (3.50A)
// Brief : Sensor layer. Definitions of commands and registers,
// functions for sensor access
//==============================================================================
//---------- Includes ----------------------------------------------------------
//#include "hal_iic.h"
#include "common.h"

//---------- Defines -----------------------------------------------------------
// sensor command
#define STM32_VCP_I2C_SIGNLEWRITE 0x01
#define STM32_VCP_I2C_SIGNLEREAD  0x02
#define STM32_VCP_I2C_WRITEBURST  0x03
#define STM32_VCP_I2C_READBURST   0x04
#define STM32_VCP_I2C_SETPILOT    0x05
#define STM32_VCP_I2C_SETDEVADDR  0x06


/* Private functions ---------------------------------------------------------*/
void I2cWriteSingle(RegisterTypeDef reg);
uint8_t I2cReadSingle(uint8_t regaddr);

void I2cWriteBurst(uint8_t startaddr, uint8_t* data, uint8_t nlen);
void I2cReadBurst(uint8_t startaddr, uint8_t* data, uint8_t nlen);

void I2cSetPilot( uint8_t pilot );
void I2cSetDevAddr( uint8_t devaddr );

void stm32_Vcp_I2c_Process(uint8_t* pCommand);

#endif