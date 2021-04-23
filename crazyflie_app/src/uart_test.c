/**
 * ,---------,       ____  _ __
 * |  ,-^-,  |      / __ )(_) /_______________ _____  ___
 * | (  O  ) |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * | / ,--´  |    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *    +------`   /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie control firmware
 *
 * Copyright (C) 2019 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * hello_world.c - App layer application of a simple hello world debug print every
 *   2 seconds.  
 */


#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "app.h"

#include "FreeRTOS.h"
#include "task.h"

#include "debug.h"
#include "uart1.h"
#include "uart2.h"
#include "deck.h"
#include "system.h"

#define DEBUG_MODULE "UART_TEST"

static uint8_t byte;

void appMain()
{
  DEBUG_PRINT("Starting UART test app\n");

  uart2Init(115200);

  systemWaitStart();
  vTaskDelay(M2T(1000));

  // Pull the reset button to get a clean read out of the data
  pinMode(DECK_GPIO_IO4, OUTPUT);
  digitalWrite(DECK_GPIO_IO4, LOW);
  vTaskDelay(10);
  digitalWrite(DECK_GPIO_IO4, HIGH);
  pinMode(DECK_GPIO_IO4, INPUT_PULLUP);

  // Read out the byte the NINA sends and immediately send it to the console.
  while (1)
  {
      if (uart2GetDataWithDefaultTimeout(&byte))
      {   
          DEBUG_PRINT("%c", (unsigned int)byte);
          vTaskDelay(pdMS_TO_TICKS(10));
      }
  }

  /* Sample code for sending data to AI deck over UART1
   * Need to initialize UART1 instead of UART2
   */
  // char* data = "d";
  // while(1){
  //   DEBUG_PRINT("Sending character\n");
  //   uart1SendData(1, (uint8_t*)data);
  //   vTaskDelay(pdMS_TO_TICKS(1000));
  // }
}
