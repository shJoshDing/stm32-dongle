#ifndef __SC_H
#define __SC_H
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
#define STM32_VCP_SC_SIGNLEWRITE 0x01
#define STM32_VCP_SC_SIGNLEREAD  0x02
#define STM32_VCP_SC_WRITEBURST  0x03
#define STM32_VCP_SC_READBURST   0x04
#define STM32_VCP_SC_SETPILOT    0x05
#define STM32_VCP_SC_SETDEVADDR  0x06


/* Private functions ---------------------------------------------------------*/
void ScWriteSingle(RegisterTypeDef reg);
uint8_t ScReadSingle(uint8_t regaddr);

void ScWriteBurst(uint8_t startaddr, uint8_t* data, uint8_t nlen);
void ScReadBurst(uint8_t startaddr, uint8_t* data, uint8_t nlen);

void ScSetPilot( uint8_t pilot );
void ScSetDevAddr( uint8_t devaddr );

void stm32_Vcp_Sc_Process(uint8_t* pCommand);

#endif