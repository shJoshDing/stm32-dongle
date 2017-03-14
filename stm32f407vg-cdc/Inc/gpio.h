#ifndef __GPIO_H
#define __GPIO_H
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
#include "main.h"
#include "common.h"

//---------- Defines -----------------------------------------------------------
// sensor command
#define STM32_VCP_GPIO_SET	        0x01
#define STM32_VCP_GPIO_RESET	        0x02
#define STM32_VCP_GPIO_TOGGLE           0x03
#define STM32_VCP_GPIO_READ             0x04

/* Private functions ---------------------------------------------------------*/
void GpioSet(uint8_t group, uint16_t pin);
void GpioReset(uint8_t group, uint16_t pin);
void GpioToggle(uint8_t group, uint16_t pin);
uint8_t GpioRead(uint8_t group, uint16_t pin);

void stm32_Vcp_Gpio_Process(uint8_t* pCommand);

#endif