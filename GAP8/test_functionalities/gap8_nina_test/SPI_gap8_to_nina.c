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
#include "SPI_gap8_to_nina.h"
#include "bsp/transport/nina_w10.h"
#include "stdio.h"
#include "cf_interface.h"

static pi_task_t led_task;
static pi_task_t block_task;
static int led_val = 0;

static struct pi_device gpio_device;

static PI_L2 char *data = "aaaaaaaaaa";

static uint32_t hop_count = 0;

static void led_handle(void *arg)
{
  pi_gpio_pin_write(&gpio_device, 2, led_val);
  led_val ^= 1;
  pi_task_push_delayed_us(pi_task_callback(&led_task, led_handle, NULL), 500000);
}

void on_message(uint8_t *msg, int len){
  hop_count = *((uint32_t*)msg);
  printf("Hop count: %i\n", hop_count);
  hop_count++;
  cf_send((uint8_t*)&hop_count, 4);
}

/* 
 * This uses a transport layer over SPI to send multiple bytes at a time
 * instead of sending one byte at time with bare SPI functions
 * like in the previous appraoch
 */
static void send_char_transport(void){
    // configure LED
    pi_gpio_pin_configure(&gpio_device, 2, PI_GPIO_OUTPUT);

    cf_interface_init(on_message);

    pi_time_wait_us(1000000);
    cf_send((uint8_t*)&hop_count, 4);

    // Send data via transport over SPI to nina
    while(1)
    {
        // toggle LED when sending information
        // pi_gpio_pin_write(&gpio_device, 2, led_val);
        // led_val ^= 1;
        // pi_task_push_delayed_us(pi_task_callback(&led_task, led_handle, NULL), 500000);

        // cf_send(data, strlen(data));
        // printf("data sent\n");
        pi_time_wait_us(1000000);
    }
    printf("SPI transfer completed !\n");

    // Stop function & exit platform
    pmsis_exit(0);
}


int main(void){
    // Start pmsis system on Gap8 with SPI-send function to be executed
    return pmsis_kickoff((void *)send_char_transport);
}