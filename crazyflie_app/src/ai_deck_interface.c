#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "ai_deck_interface.h"

#include "FreeRTOS.h"
#include "task.h"

#include "debug.h"
#include "uart1.h"
#include "uart2.h"
#include "deck.h"
#include "system.h"


#define RX_BUFFER_SIZE 1024
static uint8_t byte;
static uint8_t buffer[RX_BUFFER_SIZE];
static int data_len;
static const uint8_t SOH = 0x81;
static void (*msg_callback) (uint8_t*, int);

void print_buffer(uint8_t *buffer, int len){
  DEBUG_PRINT("Received message: \"");
  for(int i = 0; i < len; i++){
    DEBUG_PRINT("%c", buffer[i]);
  }
  DEBUG_PRINT("\"\n");
}

void uart_read_bytes(uint8_t *buffer, int len){
  for(int i = 0; i < len; i++){
    uart2GetDataWithDefaultTimeout(buffer + i);
  }
}

void receiving_task(void *pvParameters)
{
    DEBUG_PRINT("Receive task started\n");
    while(1){
      byte = 0;
      if (uart2GetDataWithDefaultTimeout(&byte)){ 
        // Whait for an SOH character
        if(byte == 0x81){
          // First four bytes is the data length
          uart_read_bytes((uint8_t*) (&data_len), 4);

          // Read the data
          data_len = data_len < RX_BUFFER_SIZE ? data_len : RX_BUFFER_SIZE;
          uart_read_bytes(buffer, data_len);
          // print_buffer(buffer, data_len);
          msg_callback(buffer, data_len);
        }
      }
    }

    vTaskDelete(NULL);
}

void send_bytes(int len, uint8_t *buffer){
  for(int i = 0; i< len; i++){
    uart1Putchar(buffer[i]);
    // vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void ai_deck_send(uint8_t *buffer, int len){
  if(len > 1024){
    DEBUG_PRINT("Don't send messages over 1024 bytes\n");
    return;
  }

  uart1SendData(1, (uint8_t*) &SOH);
  uart1SendData(4, (uint8_t*) &len);
  uart1SendData(len, buffer);
}


void ai_deck_interface_init( void (*callback) (uint8_t*, int) ){
  uart2Init(115200);

  if(!uart1Test()){
    uart1Init(230400);
  }

  systemWaitStart();
  vTaskDelay(M2T(10));

  // Pull the reset button to get a clean read out of the data
  pinMode(DECK_GPIO_IO4, OUTPUT);
  digitalWrite(DECK_GPIO_IO4, LOW);
  vTaskDelay(10);
  digitalWrite(DECK_GPIO_IO4, HIGH);
  pinMode(DECK_GPIO_IO4, INPUT_PULLUP);


  if(callback != NULL){
    DEBUG_PRINT("Starting receive task\n");
    msg_callback = callback;
    xTaskCreate(receiving_task, "receive_task", 4096, NULL, 2, NULL);
  }
}