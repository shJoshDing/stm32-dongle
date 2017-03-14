
//---------- Includes ----------------------------------------------------------
#include "gpio.h"
//#include "stm32f4_discovery.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "common.h"
#include "usbd_cdc_if.h"

//typedefine
//#define SCK    GPIO_PIN_13
//#define DATA   GPIO_PIN_15

//functions 
//void Set_Pin_Read(void);
//void Set_Pin_Write(void);

//variables
extern uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

//==============================================================================
void stm32_Vcp_Gpio_Process(uint8_t* pCommand)
{
  uint8_t ioStatus =  0x00;
  uint8_t group = pCommand[3];
  uint16_t pin = pCommand[4];
        
  switch(pCommand[2])
  {
    case STM32_VCP_GPIO_SET:
      //FlashLED();
      GpioSet(group, pin);
      
      UserTxBufferFS[0] = 0xA5;
      UserTxBufferFS[1] = pCommand[1];
      UserTxBufferFS[2] = pCommand[2];
      UserTxBufferFS[3] = 0xCC;
      CDC_Transmit_FS(UserTxBufferFS , 4);
      break;
    
    case STM32_VCP_GPIO_RESET:
      //FlashLED();
      GpioReset(group, pin);
      
      UserTxBufferFS[0] = 0xA5;
      UserTxBufferFS[1] = pCommand[1];
      UserTxBufferFS[2] = pCommand[2];
      UserTxBufferFS[3] = 0xCC;
      CDC_Transmit_FS(UserTxBufferFS , 4);
      break;
      
    case STM32_VCP_GPIO_TOGGLE:
      //FlashLED();
      GpioToggle(group, pin);
      
      UserTxBufferFS[0] = 0xA5;
      UserTxBufferFS[1] = pCommand[1];
      UserTxBufferFS[2] = pCommand[2];
      UserTxBufferFS[3] = 0xCC;
      CDC_Transmit_FS(UserTxBufferFS , 4);
      break;
      
    case STM32_VCP_GPIO_READ:
      ioStatus = GpioRead(group, pin);
      
      UserTxBufferFS[0] = 0xA5;
      UserTxBufferFS[1] = pCommand[1];
      UserTxBufferFS[2] = pCommand[2];
      UserTxBufferFS[3] = ioStatus;
      CDC_Transmit_FS(UserTxBufferFS , 4);
      break;
         
    default:
      break;
 
  }
}


//==============================================================================
void GpioSet(uint8_t group, uint16_t pin)
{
  if(pin == 0x01)
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
  else if( pin == 0x02)
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
  else
    ;
/*
  else if(group == 0x01)
    HAL_GPIO_WritePin(USR_IO_B_Port, pin, GPIO_PIN_SET);
  else if(group == 0x02)
    HAL_GPIO_WritePin(USR_IO_C_Port, pin, GPIO_PIN_SET);
  else if(group == 0x03)
    HAL_GPIO_WritePin(USR_IO_D_Port, pin, GPIO_PIN_SET);
  else if(group == 0x04)
    HAL_GPIO_WritePin(USR_IO_E_Port, pin, GPIO_PIN_SET);
*/
}

//==============================================================================
void GpioReset(uint8_t group, uint16_t pin)
{
    //HAL_GPIO_WritePin(GPIOB, pin, GPIO_PIN_RESET);
  if(pin == 0x01)
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
  else if( pin == 0x02)
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
  else
    ;
/*
  else if(group == 0x01)
    HAL_GPIO_WritePin(USR_IO_B_Port, pin, GPIO_PIN_RESET);
  else if(group == 0x02)
    HAL_GPIO_WritePin(USR_IO_C_Port, pin, GPIO_PIN_RESET);
  else if(group == 0x03)
    HAL_GPIO_WritePin(USR_IO_D_Port, pin, GPIO_PIN_RESET);
  else if(group == 0x04)
    HAL_GPIO_WritePin(USR_IO_E_Port, pin, GPIO_PIN_RESET);
*/
}

//==============================================================================
void GpioToggle(uint8_t group, uint16_t pin)
{ 
  if(pin == 0x01)
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
  else if( pin == 0x02)
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
  else
    ;
  /*
  else if(group == 0x01)
    HAL_GPIO_TogglePin(USR_IO_B_Port, pin);
  else if(group == 0x02)
    HAL_GPIO_TogglePin(USR_IO_C_Port, pin);
  else if(group == 0x03)
    HAL_GPIO_TogglePin(USR_IO_D_Port, pin);
  else if(group == 0x04)
    HAL_GPIO_TogglePin(USR_IO_E_Port, pin);
  */
}


//==============================================================================
uint8_t GpioRead(uint8_t group, uint16_t pin)
{ 
  uint8_t result = 0;

  if(pin == 0x01)
    result = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7);
  else if( pin == 0x02)
    result = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9);
  else
    result = 0xFF;
    
  return result;
  /*
  else if(group == 0x01)
    HAL_GPIO_TogglePin(USR_IO_B_Port, pin);
  else if(group == 0x02)
    HAL_GPIO_TogglePin(USR_IO_C_Port, pin);
  else if(group == 0x03)
    HAL_GPIO_TogglePin(USR_IO_D_Port, pin);
  else if(group == 0x04)
    HAL_GPIO_TogglePin(USR_IO_E_Port, pin);
  */
}



