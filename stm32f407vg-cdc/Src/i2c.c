
//---------- Includes ----------------------------------------------------------
#include "sc.h"
//#include "stm32f4_discovery.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "common.h"
#include "usbd_cdc_if.h"

//typedefine
#define SCL    GPIO_PIN_12
#define SDA   GPIO_PIN_14

//functions 
//void Set_Pin_Read(void);
//void Set_Pin_Write(void);

//variables
uint8_t i2c_pilot = 5;              //2us, freq = 1000 / (pilot *2) = 250KHz 
uint8_t i2c_DevAddr = 0x65;

extern uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];
uint8_t i2ctempdata[APP_TX_DATA_SIZE];

//==============================================================================
void stm32_Vcp_Sc_Process(uint8_t* pCommand)
{
  RegisterTypeDef reg;
  uint8_t startAddr = 0;
  uint8_t wxLen = 0;
  uint8_t rxLen = 0;
  uint8_t* pdata;
  
  switch(pCommand[2])
  {
    case STM32_VCP_I2C_SIGNLEWRITE:
      wxLen = pCommand[3];
      for(uint8_t i = 0; i < wxLen; i++)
      {
        reg.Addr  = pCommand[4 + i * 2];
        reg.Value = pCommand[5 + i * 2];
        I2cWriteSingle(reg);
      }
      
      UserTxBufferFS[0] = 0xA5;
      UserTxBufferFS[1] = pCommand[1];
      UserTxBufferFS[2] = pCommand[2];
      UserTxBufferFS[3] = 0xCC;
      CDC_Transmit_FS(UserTxBufferFS , 4);
      break;
    
    case STM32_VCP_I2C_SIGNLEREAD:
      rxLen = pCommand[3];
      for(uint8_t i = 0; i < rxLen; i++)
      {
        reg.Addr = pCommand[4 + i];
        i2ctempdata[i] = I2cReadSingle(reg.Addr);
      }
      UserTxBufferFS[0] = 0xA5;
      UserTxBufferFS[1] = pCommand[1];
      UserTxBufferFS[2] = pCommand[2];
      UserTxBufferFS[3] = pCommand[3];
      for(uint8_t i = 0; i < rxLen; i++)
        UserTxBufferFS[4 + i] = i2ctempdata[i];
      CDC_Transmit_FS(UserTxBufferFS , 4 + rxLen);
      break;
      
    case STM32_VCP_I2C_WRITEBURST:
      startAddr = pCommand[3];
      wxLen = pCommand[4];
      pdata = &pCommand[5];
      I2cWriteBurst(startAddr, pdata, wxLen);
      
      UserTxBufferFS[0] = 0xA5;
      UserTxBufferFS[1] = pCommand[1];
      UserTxBufferFS[2] = pCommand[2];
      UserTxBufferFS[3] = 0xCC;
      CDC_Transmit_FS(UserTxBufferFS , 4);
      break;
      
    case STM32_VCP_I2C_READBURST:
      startAddr = pCommand[3];
      rxLen = pCommand[4];
      //pdata = &pCommand[5];
      I2cReadBurst(startAddr, i2ctempdata, rxLen);
      
      UserTxBufferFS[0] = 0xA5;
      UserTxBufferFS[1] = pCommand[1];
      UserTxBufferFS[2] = pCommand[2];
      for(uint8_t i = 0; i < rxLen; i++)
        UserTxBufferFS[3 + i] = i2ctempdata[i];
      CDC_Transmit_FS(UserTxBufferFS , rxLen+3);
      break;
      
    case STM32_VCP_I2C_SETPILOT:
      I2cSetPilot(pCommand[3]);
      
      UserTxBufferFS[0] = 0xA5;
      UserTxBufferFS[1] = pCommand[1];
      UserTxBufferFS[2] = pCommand[2];
      UserTxBufferFS[3] = 0xCC;
      CDC_Transmit_FS(UserTxBufferFS , 4);
      break;
      
    case STM32_VCP_I2C_SETDEVADDR:
      I2cSetDevAddr(pCommand[3]);
      
      UserTxBufferFS[0] = 0xA5;
      UserTxBufferFS[1] = pCommand[1];
      UserTxBufferFS[2] = pCommand[2];
      UserTxBufferFS[3] = 0xCC;
      CDC_Transmit_FS(UserTxBufferFS , 4);
      break;
         
    default:
      break;
 
  }
}


//==============================================================================
void I2cSetPinWrite(void)
{
    //HAL_GPIO_WritePin(GPIOB, SDA, GPIO_PIN_RESET);
    //HAL_GPIO_WritePin(GPIOB, SDA, GPIO_PIN_RESET);
    GPIO_InitTypeDef   GPIO_InitStructure;

    GPIO_InitStructure.Pin = SDA;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//==============================================================================
void I2cSetPinRead(void)
{
    //HAL_GPIO_WritePin(GPIOB, SDA, GPIO_PIN_RESET);

    GPIO_InitTypeDef   GPIO_InitStructure;

    GPIO_InitStructure.Pin = SDA;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);  
}

//==============================================================================
void I2cStart(void)
{
    HAL_GPIO_WritePin(GPIOB, SDA | SCL, GPIO_PIN_SET);
    Delay_N1us(i2c_pilot * 3);
    
    HAL_GPIO_WritePin(GPIOB, SDA, GPIO_PIN_RESET);
    Delay_N1us(i2c_pilot);
    
    HAL_GPIO_WritePin(GPIOB, SCL, GPIO_PIN_RESET);
    Delay_N1us(i2c_pilot);
     
}


//==============================================================================
void I2cWriteByte(uint8_t byte)
{
   uint8_t mask = 0;
    
    for(uint8_t i = 0; i < 8; i++)
    {
        mask = 0x01 << (7-i);
        if(byte & mask)
            //GPIO_SetBits(GPIOB, SCL | SDA);
          HAL_GPIO_WritePin(GPIOB, SDA | SCL, GPIO_PIN_SET);
        else
        {
            //GPIO_SetBits(GPIOB, SCL);
          HAL_GPIO_WritePin(GPIOB, SCL, GPIO_PIN_SET);
          HAL_GPIO_WritePin(GPIOB, SDA, GPIO_PIN_RESET);
        }
        Delay_N1us(i2c_pilot);
        //GPIO_ResetBits(GPIOB, SCL);
        HAL_GPIO_WritePin(GPIOB, SCL, GPIO_PIN_RESET);
        Delay_N1us(i2c_pilot);
    } 
}

//==============================================================================
uint8_t I2cReadByteLastByte(void)
{
    uint8_t byte = 0; 
    
    HAL_GPIO_WritePin(GPIOB, SDA, GPIO_PIN_SET);
    I2cSetPinRead();
    
    for(uint8_t i = 0; i < 8; i++)
    {      
        //GPIO_SetBits(GPIOB, SCL);
        HAL_GPIO_WritePin(GPIOB, SCL, GPIO_PIN_SET);
        Delay_N1us(i2c_pilot);
        //GPIO_ResetBits(GPIOB, SCL);
        //HAL_GPIO_WritePin(GPIOB, SCL, GPIO_PIN_RESET);
        //Delay_N1us(i2c_pilot);
        //if(GPIO_ReadInputDataBit(GPIOB, SDA))
        if(HAL_GPIO_ReadPin(GPIOB, SDA))
            byte += 1<<(7-i);
 
        //GPIO_ResetBits(GPIOB, SCL);
        HAL_GPIO_WritePin(GPIOB, SCL, GPIO_PIN_RESET);
        Delay_N1us(i2c_pilot);
    } 
    HAL_GPIO_WritePin(GPIOB, SDA, GPIO_PIN_RESET);
    I2cSetPinWrite();
    
    return byte;
}

uint8_t I2cReadByte(void)
{
    uint8_t byte = 0; 
    
    HAL_GPIO_WritePin(GPIOB, SDA, GPIO_PIN_SET);
    I2cSetPinRead();
    
    for(uint8_t i = 0; i < 8; i++)
    {      
        //GPIO_SetBits(GPIOB, SCL);
        HAL_GPIO_WritePin(GPIOB, SCL, GPIO_PIN_SET);
        Delay_N1us(i2c_pilot);
        //GPIO_ResetBits(GPIOB, SCL);
        //HAL_GPIO_WritePin(GPIOB, SCL, GPIO_PIN_RESET);
        //Delay_N1us(i2c_pilot);
        //if(GPIO_ReadInputDataBit(GPIOB, SDA))
        if(HAL_GPIO_ReadPin(GPIOB, SDA))
            byte += 1<<(7-i);
 
        //GPIO_ResetBits(GPIOB, SCL);
        HAL_GPIO_WritePin(GPIOB, SCL, GPIO_PIN_RESET);
        Delay_N1us(i2c_pilot);
    } 
    HAL_GPIO_WritePin(GPIOB, SDA, GPIO_PIN_SET);
    I2cSetPinWrite();
    
    return byte;
}

//==============================================================================
//void WriteSyncBit(bool indicator)
//{
//    if(indicator)
//        //GPIO_SetBits(GPIOB, SCL | SDA);
//      HAL_GPIO_WritePin(GPIOB, SCL | SDA, GPIO_PIN_SET);
//    else
//    {
//        //GPIO_SetBits(GPIOB, SCL);
//        HAL_GPIO_WritePin(GPIOB, SCL, GPIO_PIN_SET);
//        HAL_GPIO_WritePin(GPIOB, SDA, GPIO_PIN_RESET);
//    }
//    Delay_N1us(i2c_pilot);
//    //GPIO_ResetBits(GPIOB, SCL);
//    HAL_GPIO_WritePin(GPIOB, SCL, GPIO_PIN_RESET);
//    Delay_N1us(i2c_pilot);
//}

//==============================================================================
void I2cWriteDevAddr(uint8_t byte, bool read)
{
   uint8_t mask = 0;
    
    for(uint8_t i = 0; i < 7; i++)
    {
        mask = 0x01 << (6-i);
        if(byte & mask)
            //GPIO_SetBits(GPIOB, SCL | SDA);
            HAL_GPIO_WritePin(GPIOB, SCL | SDA, GPIO_PIN_SET);
        else
        {
            //GPIO_SetBits(GPIOB, SCL);
            HAL_GPIO_WritePin(GPIOB, SCL, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, SDA, GPIO_PIN_RESET);
        }
        Delay_N1us(i2c_pilot);
        //GPIO_ResetBits(GPIOB, SCL);
        HAL_GPIO_WritePin(GPIOB, SCL, GPIO_PIN_RESET);
        Delay_N1us(i2c_pilot);
    } 
    if(read)
        //GPIO_SetBits(GPIOB, SCL | SDA);
        HAL_GPIO_WritePin(GPIOB, SCL | SDA, GPIO_PIN_SET);
    else
    {
        //GPIO_SetBits(GPIOB, SCL);
        HAL_GPIO_WritePin(GPIOB, SCL, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, SDA, GPIO_PIN_RESET);
    }
    Delay_N1us(i2c_pilot);
    //GPIO_ResetBits(GPIOB, SCL);
    HAL_GPIO_WritePin(GPIOB, SCL, GPIO_PIN_RESET);
    Delay_N1us(i2c_pilot);
    
}


/*==============================================================================
// Name : Set Pilot
// Brief : Sensor layer. Definitions of commands and registers,
// functions for sensor access
==============================================================================*/
void I2cSetPilot( uint8_t pilot )
{
    i2c_pilot = pilot;
}

void I2cSetDevAddr( uint8_t devaddr )
{
    i2c_DevAddr = devaddr;
}

/*==============================================================================
// Name : Owci_Write
// Brief : Sensor layer. Definitions of commands and registers,
// functions for sensor access
==============================================================================*/
void I2cWriteSingle(RegisterTypeDef reg)
{
    //I2cSetPilot(10);
    I2cWriteByte(0x00);
    WriteSyncBit(false);
    I2cWriteByte(0x00);
    WriteSyncBit(true);
    I2cWriteDevAddr(i2c_DevAddr, false);
    WriteSyncBit(true);
    
    I2cWriteByte(reg.Addr);
    WriteSyncBit(true);
    
    I2cWriteByte(reg.Value);
    WriteSyncBit(false);
    
    I2cWriteByte(0x00);
    WriteSyncBit(false);
}

/*==============================================================================
// Name : Owci_Read
// Brief : Sensor layer. Definitions of commands and registers,
// functions for sensor access
==============================================================================*/
uint8_t I2cReadSingle(uint8_t regaddr)
{
    uint8_t regValue = 0;
    //I2cSetPilot(10);
    I2cWriteByte(0x00);
    WriteSyncBit(false);
    I2cWriteByte(0x00);
    WriteSyncBit(true);
    I2cWriteDevAddr(i2c_DevAddr, true);
    WriteSyncBit(true);
    
    I2cWriteByte(regaddr);
    WriteSyncBit(true);
    
    regValue = I2cReadByte();
    WriteSyncBit(true);
    
    I2cWriteByte(0x00);
    WriteSyncBit(false);
    return regValue;
}

void I2cWriteBurst(uint8_t startaddr, uint8_t* data, uint8_t nlen)
{
    I2cWriteByte(0x00);
    WriteSyncBit(false);
    I2cWriteByte(0x00);
    WriteSyncBit(true);
    I2cWriteDevAddr(i2c_DevAddr, false);
    WriteSyncBit(true);
    
    I2cWriteByte(startaddr);
    WriteSyncBit(true);
    
    for(uint8_t i = 0; i < nlen - 1; i++)
    {
      I2cWriteByte(data[i]);
      WriteSyncBit(true);
    }
    I2cWriteByte(data[nlen - 1]);
    WriteSyncBit(false);
    
    I2cWriteByte(0x00);
    WriteSyncBit(false);
}
void I2cReadBurst(uint8_t startaddr, uint8_t* data, uint8_t nlen)
{
    //uint8_t regValue = 0;
    //I2cSetPilot(10);
    I2cWriteByte(0x00);
    WriteSyncBit(false);
    I2cWriteByte(0x00);
    WriteSyncBit(true);
    I2cWriteDevAddr(i2c_DevAddr, true);
    WriteSyncBit(true);
    
    I2cWriteByte(startaddr);
    WriteSyncBit(true);
    
    for(uint8_t i = 0; i < nlen - 1; i++)
    {
      data[i] = I2cReadByte();
      WriteSyncBit(true);
    }
    
    data[nlen - 1] = I2cReadByteLastByte();
    WriteSyncBit(false);
    
    I2cWriteByte(0x00);
    WriteSyncBit(false);
    //return regValue;
}


