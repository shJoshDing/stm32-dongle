
//---------- Includes ----------------------------------------------------------
#include "sc.h"
//#include "stm32f4_discovery.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "common.h"
#include "usbd_cdc_if.h"

//typedefine
#define SCK    GPIO_PIN_3
#define DATA   GPIO_PIN_5

//functions 
//void Set_Pin_Read(void);
//void Set_Pin_Write(void);

//variables
uint8_t sc_pilot = 10;              //2us, freq = 1000 / (pilot *2) = 250KHz 
uint8_t sc_DevAddr = 0x65;

extern uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];
uint8_t tempdata[APP_TX_DATA_SIZE];

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
    case STM32_VCP_SC_SIGNLEWRITE:
      wxLen = pCommand[3];
      for(uint8_t i = 0; i < wxLen; i++)
      {
        reg.Addr  = pCommand[4 + i * 2];
        reg.Value = pCommand[5 + i * 2];
        ScWriteSingle(reg);
      }
      
      UserTxBufferFS[0] = 0xA5;
      UserTxBufferFS[1] = pCommand[1];
      UserTxBufferFS[2] = pCommand[2];
      UserTxBufferFS[3] = 0xCC;
      CDC_Transmit_FS(UserTxBufferFS , 4);
      break;
    
    case STM32_VCP_SC_SIGNLEREAD:
      rxLen = pCommand[3];
      for(uint8_t i = 0; i < rxLen; i++)
      {
        reg.Addr = pCommand[4 + i];
        tempdata[i] = ScReadSingle(reg.Addr);
      }
      UserTxBufferFS[0] = 0xA5;
      UserTxBufferFS[1] = pCommand[1];
      UserTxBufferFS[2] = pCommand[2];
      UserTxBufferFS[3] = pCommand[3];
      for(uint8_t i = 0; i < rxLen; i++)
        UserTxBufferFS[4 + i] = tempdata[i];
      CDC_Transmit_FS(UserTxBufferFS , 4 + rxLen);
      break;
      
    case STM32_VCP_SC_WRITEBURST:
      startAddr = pCommand[3];
      wxLen = pCommand[4];
      pdata = &pCommand[5];
      ScWriteBurst(startAddr, pdata, wxLen);
      
      UserTxBufferFS[0] = 0xA5;
      UserTxBufferFS[1] = pCommand[1];
      UserTxBufferFS[2] = pCommand[2];
      UserTxBufferFS[3] = 0xCC;
      CDC_Transmit_FS(UserTxBufferFS , 4);
      break;
      
    case STM32_VCP_SC_READBURST:
      startAddr = pCommand[3];
      rxLen = pCommand[4];
      //pdata = &pCommand[5];
      ScReadBurst(startAddr, tempdata, rxLen);
      
      UserTxBufferFS[0] = 0xA5;
      UserTxBufferFS[1] = pCommand[1];
      UserTxBufferFS[2] = pCommand[2];
      for(uint8_t i = 0; i < rxLen; i++)
        UserTxBufferFS[3 + i] = tempdata[i];
      CDC_Transmit_FS(UserTxBufferFS , rxLen+3);
      break;
      
    case STM32_VCP_SC_SETPILOT:
      ScSetPilot(pCommand[3]);
      
      UserTxBufferFS[0] = 0xA5;
      UserTxBufferFS[1] = pCommand[1];
      UserTxBufferFS[2] = pCommand[2];
      UserTxBufferFS[3] = 0xCC;
      CDC_Transmit_FS(UserTxBufferFS , 4);
      break;
      
    case STM32_VCP_SC_SETDEVADDR:
      ScSetDevAddr(pCommand[3]);
      
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
void SetPinWrite(void)
{
    //HAL_GPIO_WritePin(GPIOB, DATA, GPIO_PIN_RESET);
    //HAL_GPIO_WritePin(GPIOB, DATA, GPIO_PIN_RESET);
    GPIO_InitTypeDef   GPIO_InitStructure;

    GPIO_InitStructure.Pin = DATA;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//==============================================================================
void SetPinRead(void)
{
    //HAL_GPIO_WritePin(GPIOB, DATA, GPIO_PIN_RESET);

    GPIO_InitTypeDef   GPIO_InitStructure;

    GPIO_InitStructure.Pin = DATA;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);  
}

//==============================================================================
void WriteByte(uint8_t byte)
{
   uint8_t mask = 0;
    
    for(uint8_t i = 0; i < 8; i++)
    {
        mask = 0x01 << (7-i);
        if(byte & mask)
            //GPIO_SetBits(GPIOB, SCK | DATA);
          HAL_GPIO_WritePin(GPIOB, DATA | SCK, GPIO_PIN_SET);
        else
        {
            //GPIO_SetBits(GPIOB, SCK);
          HAL_GPIO_WritePin(GPIOB, SCK, GPIO_PIN_SET);
          HAL_GPIO_WritePin(GPIOB, DATA, GPIO_PIN_RESET);
        }
        Delay_N1us(sc_pilot);
        //GPIO_ResetBits(GPIOB, SCK);
        HAL_GPIO_WritePin(GPIOB, SCK, GPIO_PIN_RESET);
        Delay_N1us(sc_pilot);
    } 
}

//==============================================================================
uint8_t ReadByteLastByte(void)
{
    uint8_t byte = 0; 
    
    HAL_GPIO_WritePin(GPIOB, DATA, GPIO_PIN_SET);
    SetPinRead();
    
    for(uint8_t i = 0; i < 8; i++)
    {      
        //GPIO_SetBits(GPIOB, SCK);
        HAL_GPIO_WritePin(GPIOB, SCK, GPIO_PIN_SET);
        Delay_N1us(sc_pilot);
        //GPIO_ResetBits(GPIOB, SCK);
        //HAL_GPIO_WritePin(GPIOB, SCK, GPIO_PIN_RESET);
        //Delay_N1us(sc_pilot);
        //if(GPIO_ReadInputDataBit(GPIOB, DATA))
        if(HAL_GPIO_ReadPin(GPIOB, DATA))
            byte += 1<<(7-i);
 
        //GPIO_ResetBits(GPIOB, SCK);
        HAL_GPIO_WritePin(GPIOB, SCK, GPIO_PIN_RESET);
        Delay_N1us(sc_pilot);
    } 
    HAL_GPIO_WritePin(GPIOB, DATA, GPIO_PIN_RESET);
    SetPinWrite();
    
    return byte;
}

uint8_t ReadByte(void)
{
    uint8_t byte = 0; 
    
    HAL_GPIO_WritePin(GPIOB, DATA, GPIO_PIN_SET);
    SetPinRead();
    
    for(uint8_t i = 0; i < 8; i++)
    {      
        //GPIO_SetBits(GPIOB, SCK);
        HAL_GPIO_WritePin(GPIOB, SCK, GPIO_PIN_SET);
        Delay_N1us(sc_pilot);
        //GPIO_ResetBits(GPIOB, SCK);
        //HAL_GPIO_WritePin(GPIOB, SCK, GPIO_PIN_RESET);
        //Delay_N1us(sc_pilot);
        //if(GPIO_ReadInputDataBit(GPIOB, DATA))
        if(HAL_GPIO_ReadPin(GPIOB, DATA))
            byte += 1<<(7-i);
 
        //GPIO_ResetBits(GPIOB, SCK);
        HAL_GPIO_WritePin(GPIOB, SCK, GPIO_PIN_RESET);
        Delay_N1us(sc_pilot);
    } 
    HAL_GPIO_WritePin(GPIOB, DATA, GPIO_PIN_SET);
    SetPinWrite();
    
    return byte;
}

//==============================================================================
void WriteSyncBit(bool indicator)
{
    if(indicator)
        //GPIO_SetBits(GPIOB, SCK | DATA);
      HAL_GPIO_WritePin(GPIOB, SCK | DATA, GPIO_PIN_SET);
    else
    {
        //GPIO_SetBits(GPIOB, SCK);
        HAL_GPIO_WritePin(GPIOB, SCK, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, DATA, GPIO_PIN_RESET);
    }
    Delay_N1us(sc_pilot);
    //GPIO_ResetBits(GPIOB, SCK);
    HAL_GPIO_WritePin(GPIOB, SCK, GPIO_PIN_RESET);
    Delay_N1us(sc_pilot);
}

//==============================================================================
void WriteDevAddr(uint8_t byte, bool read)
{
   uint8_t mask = 0;
    
    for(uint8_t i = 0; i < 7; i++)
    {
        mask = 0x01 << (6-i);
        if(byte & mask)
            //GPIO_SetBits(GPIOB, SCK | DATA);
            HAL_GPIO_WritePin(GPIOB, SCK | DATA, GPIO_PIN_SET);
        else
        {
            //GPIO_SetBits(GPIOB, SCK);
            HAL_GPIO_WritePin(GPIOB, SCK, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, DATA, GPIO_PIN_RESET);
        }
        Delay_N1us(sc_pilot);
        //GPIO_ResetBits(GPIOB, SCK);
        HAL_GPIO_WritePin(GPIOB, SCK, GPIO_PIN_RESET);
        Delay_N1us(sc_pilot);
    } 
    if(read)
        //GPIO_SetBits(GPIOB, SCK | DATA);
        HAL_GPIO_WritePin(GPIOB, SCK | DATA, GPIO_PIN_SET);
    else
    {
        //GPIO_SetBits(GPIOB, SCK);
        HAL_GPIO_WritePin(GPIOB, SCK, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, DATA, GPIO_PIN_RESET);
    }
    Delay_N1us(sc_pilot);
    //GPIO_ResetBits(GPIOB, SCK);
    HAL_GPIO_WritePin(GPIOB, SCK, GPIO_PIN_RESET);
    Delay_N1us(sc_pilot);
    
}


/*==============================================================================
// Name : Set Pilot
// Brief : Sensor layer. Definitions of commands and registers,
// functions for sensor access
==============================================================================*/
void ScSetPilot( uint8_t pilot )
{
    sc_pilot = pilot;
}

void ScSetDevAddr( uint8_t devaddr )
{
    sc_DevAddr = devaddr;
}

/*==============================================================================
// Name : Owci_Write
// Brief : Sensor layer. Definitions of commands and registers,
// functions for sensor access
==============================================================================*/
void ScWriteSingle(RegisterTypeDef reg)
{
    //ScSetPilot(10);
    WriteByte(0x00);
    WriteSyncBit(false);
    WriteByte(0x00);
    WriteSyncBit(true);
    WriteDevAddr(sc_DevAddr, false);
    WriteSyncBit(true);
    
    WriteByte(reg.Addr);
    WriteSyncBit(true);
    
    WriteByte(reg.Value);
    WriteSyncBit(false);
    
    WriteByte(0x00);
    WriteSyncBit(false);
}

/*==============================================================================
// Name : Owci_Read
// Brief : Sensor layer. Definitions of commands and registers,
// functions for sensor access
==============================================================================*/
uint8_t ScReadSingle(uint8_t regaddr)
{
    uint8_t regValue = 0;
    //ScSetPilot(10);
    WriteByte(0x00);
    WriteSyncBit(false);
    WriteByte(0x00);
    WriteSyncBit(true);
    WriteDevAddr(sc_DevAddr, true);
    WriteSyncBit(true);
    
    WriteByte(regaddr);
    WriteSyncBit(true);
    
    regValue = ReadByte();
    WriteSyncBit(true);
    
    WriteByte(0x00);
    WriteSyncBit(false);
    return regValue;
}

void ScWriteBurst(uint8_t startaddr, uint8_t* data, uint8_t nlen)
{
    WriteByte(0x00);
    WriteSyncBit(false);
    WriteByte(0x00);
    WriteSyncBit(true);
    WriteDevAddr(sc_DevAddr, false);
    WriteSyncBit(true);
    
    WriteByte(startaddr);
    WriteSyncBit(true);
    
    for(uint8_t i = 0; i < nlen - 1; i++)
    {
      WriteByte(data[i]);
      WriteSyncBit(true);
    }
    WriteByte(data[nlen - 1]);
    WriteSyncBit(false);
    
    WriteByte(0x00);
    WriteSyncBit(false);
}
void ScReadBurst(uint8_t startaddr, uint8_t* data, uint8_t nlen)
{
    //uint8_t regValue = 0;
    //ScSetPilot(10);
    WriteByte(0x00);
    WriteSyncBit(false);
    WriteByte(0x00);
    WriteSyncBit(true);
    WriteDevAddr(sc_DevAddr, true);
    WriteSyncBit(true);
    
    WriteByte(startaddr);
    WriteSyncBit(true);
    
    for(uint8_t i = 0; i < nlen - 1; i++)
    {
      data[i] = ReadByte();
      WriteSyncBit(true);
    }
    
    data[nlen - 1] = ReadByteLastByte();
    WriteSyncBit(false);
    
    WriteByte(0x00);
    WriteSyncBit(false);
    //return regValue;
}


