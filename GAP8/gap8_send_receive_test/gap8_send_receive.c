/**
 * Function, that sends data from the AI-Decks's gap8 microprocessor to the NINA-Wifi-Chip via the SPI communication protocol.
 * The goal is to send data from the AI-Deck to the Crazyflie drone via SPI and UART 2 instead of UART1, so that UART1 can be used to 
 * communicate and exchange data with the lighthouse deck. This way, distortions between the lighthouse and AI-Deck resulting from both
 * using UART1 won't happen.
 * 
 * Gap8 SPI-Output Pin: B1 (GAP8_GPIO_NINA_IO)      SPI-Input: A13 (NINA_GPIO_GAP8_IO)
 * NINA SPI-Input Pin:  5  (GAP8_GPIO_NINA_IO)      SPI-Output: 25 (NINA_GPIO_GAP8_IO)   
 */

#include "pmsis.h"
#include "gap8_send_receive.h"
#include "bsp/transport/nina_w10.h"
#include "stdio.h"
#include "cf_interface.h"

static uint32_t hop_count = 0;

void on_message(uint8_t *msg, int len){
  hop_count = *((uint32_t*)msg);
  printf("Hop count: %i\n", hop_count);
  hop_count++;
  cf_send((uint8_t*)&hop_count, 4);
}

/* 
 * This initiates a ping-pong communication.
 * The message is a 4 byte integer that is increamented with each ping
 */
static void send_receive_test(void){

    cf_interface_init(on_message);

    pi_time_wait_us(1000000);
    cf_send((uint8_t*)&hop_count, 4);

    while(1)
    {
        pi_time_wait_us(1000000);
    }

    // Stop function & exit platform
    pmsis_exit(0);
}


int main(void){
    // Start pmsis system on Gap8 with SPI-send function to be executed
    return pmsis_kickoff((void *)send_receive_test);
}