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
#include "deck.h"
#include "system.h"
#include "ai_deck_interface.h"

#define DEBUG_MODULE "UART_TEST"

static int messages_recieved = 0;
static int bytes_received = 0;
static uint32_t hop_count = 0;
long tick_start, tick_end;

void on_message(uint8_t* msg, int len){
  tick_end = xTaskGetTickCount();
  messages_recieved += 1;
  bytes_received += len;
  hop_count = *((uint32_t*)msg);
  DEBUG_PRINT("Hop count: %u, RTT: %ims\n", (unsigned int)hop_count, (int)T2M(tick_end-tick_start));
  vTaskDelay(pdMS_TO_TICKS(500));
  hop_count++;
  tick_start = xTaskGetTickCount();
  ai_deck_send((uint8_t*)&hop_count, 4);
}

void appMain()
{
  DEBUG_PRINT("Starting UART test app\n");

  ai_deck_interface_init(on_message);

  while(1){
    vTaskDelay(pdMS_TO_TICKS(1000));
    // DEBUG_PRINT("RX bytes: %i, RX msgs: %i from AI deck\n", bytes_received, messages_recieved);
  }
}
