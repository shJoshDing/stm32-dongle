#include "common.h"
//#include "stm32f4_discovery.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>

/**
  * @brief  Delay Function.
  * @param  nCount:specifies the Delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}

void Delay_N1us(__IO uint32_t one_us)
{
  __IO uint32_t nCount = one_us*0x18;
  while(nCount--)
  {
  }
}

void Delay_N1ms(__IO uint32_t one_ms)
{
  __IO uint32_t nCount = one_ms;
  while(nCount--)
  {
      Delay_N1us(1000);
  }
}

void FlashLED(void)
{
  for(uint8_t i = 0; i < 4; i ++)
  {
    HAL_GPIO_TogglePin(GPIOD, LD3_Pin);
    Delay_N1ms(100);
  }
}