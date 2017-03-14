#ifndef __COMMON_H
#define __COMMON_H
//==============================================================================
// S E N S I R I O N AG, Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project : SHT2x Sample Code (V1.2)
// File : I2C_HAL.h
// Author : MST
// Controller: NEC V850/SG3 (uPD70F3740)
// Compiler : IAR compiler for V850 (3.50A)
// Brief : I2C Hardware abstraction layer
//==============================================================================
//---------- Includes ----------------------------------------------------------
//#include "system.h"
//#include "stm32f4_discovery.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "stm32f4xx.h"

//---------- Defines -----------------------------------------------------------
//#define uint8_t u8t
//I2C ports
//The communication on SDA and SCL is done by switching pad direction
//For a low level on SCL or SDA, direction is set to output. For a high level on
//SCL or SDA, direction is set to input. (pull up resistor active)

//PD5 => scl; PD6 => sda
#define N100US 3334
#define N200US 6667
#define N300US 10001
#define N600US 20002
#define N900US 30003
#define N1MS 33340
#define N10MS 333400
#define N100MS 3334000


typedef enum{
  false = 0,
  true = 1,
}bool;

typedef struct
{
  uint8_t Addr;
  uint8_t Value;
}RegisterTypeDef;

//==============================================================================
void Delay(__IO uint32_t nCount);

void Delay_N1us(__IO uint32_t nCount);

void Delay_N1ms(__IO uint32_t nCount);

void FlashLED(void);
//==============================================================================



#endif