        if(UserRxBufferFS[1] == 0x01)
        {
          //write reg
          uint8_t tempdata[16];
          uint8_t startreg = UserRxBufferFS[2];
          uint8_t wlen = UserRxBufferFS[3];
          for(uint8_t i = 0; i < wlen; i++)
            tempdata[i] = UserRxBufferFS[4 + i];
          ScWriteBurst(startreg, tempdata, wlen);
          
          //send report back to host and flash led
          UserTxBufferFS[0] = UserRxBufferFS[0];
          UserTxBufferFS[1] = UserRxBufferFS[1];
          UserTxBufferFS[2] = 0xCC;
          HAL_GPIO_TogglePin(GPIOD, LD3_Pin);
          CDC_Transmit_FS(UserTxBufferFS , 3);
          Delay_N1ms(200);
          HAL_GPIO_TogglePin(GPIOD, LD3_Pin);
        }
        else if( UserRxBufferFS[1] == 0x02)
        {
          //RegisterTypeDef reg;
          uint8_t tempdata[16];
          uint8_t startreg = UserRxBufferFS[2];
          uint8_t rlen = UserRxBufferFS[3];
          for(uint8_t i = 0; i < rlen; i++)
            tempdata[i] = 0;

          ScReadBurst(startreg, tempdata, rlen);
          
          HAL_GPIO_TogglePin(GPIOD, LD3_Pin);
          UserTxBufferFS[0] = UserRxBufferFS[0];
          UserTxBufferFS[1] = UserRxBufferFS[1];
          for(uint8_t i = 0; i < rlen; i++)
            UserTxBufferFS[2 + i] = tempdata[i];
          CDC_Transmit_FS(UserTxBufferFS , rlen + 2);
          Delay_N1ms(200);
          HAL_GPIO_TogglePin(GPIOD, LD3_Pin);
        }
        else if( UserRxBufferFS[1] == 0x03)
        {
          uint8_t pilot = UserRxBufferFS[2];

          ScSetPilot(pilot);
          
          HAL_GPIO_TogglePin(GPIOD, LD3_Pin);
          UserTxBufferFS[0] = UserRxBufferFS[0];
          UserTxBufferFS[1] = UserRxBufferFS[1];
          UserTxBufferFS[2] = 0xCC;
          CDC_Transmit_FS(UserTxBufferFS , 3);
          Delay_N1ms(200);
          HAL_GPIO_TogglePin(GPIOD, LD3_Pin);
        }
        
        else if( UserRxBufferFS[1] == 0x04)
        {
          uint8_t devaddr = UserRxBufferFS[2];

          ScSetDevAddr(devaddr);
          
          HAL_GPIO_TogglePin(GPIOD, LD3_Pin);
          UserTxBufferFS[0] = UserRxBufferFS[0];
          UserTxBufferFS[1] = UserRxBufferFS[1];
          UserTxBufferFS[2] = 0xCC;
          CDC_Transmit_FS(UserTxBufferFS , 3);
          Delay_N1ms(200);
          HAL_GPIO_TogglePin(GPIOD, LD3_Pin);
        }