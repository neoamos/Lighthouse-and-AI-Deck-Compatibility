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

static pi_task_t led_task;
static int led_val = 0;

static struct pi_device gpio_device;
static struct pi_device nina; 

static void led_handle(void *arg)
{
  pi_gpio_pin_write(&gpio_device, 2, led_val);
  led_val ^= 1;
  pi_task_push_delayed_us(pi_task_callback(&led_task, led_handle, NULL), 500000);
}

static int open_transport(struct pi_device *device)
{
  struct pi_nina_w10_conf nina_conf;

  pi_nina_w10_conf_init(&nina_conf);

  nina_conf.ssid = "";
  nina_conf.passwd = "";
  nina_conf.ip_addr = "0.0.0.0";
  nina_conf.port = 5555;
  pi_open_from_conf(device, &nina_conf);
  if (pi_transport_open(device))
    return -1;

  return 0;
}

// how to give data to the function ?
static void send_char_SPI(void){

    // configure LED
    pi_gpio_pin_configure(&gpio_device, 2, PI_GPIO_OUTPUT);

    // configuration of the SPI parameters
    struct pi_spi_conf spi_config;  

    // Nina-Wifi-Chip as receiving device
    // How does the program know, if it is the nina ? 
    struct pi_device nina; 
    
    // Character that will be sent to NINA as a test
    // PI_L2 means that it will be saved in the L2-buffer of the GAP8 before being sent
    static PI_L2 char letter = 'a';

    // Default SPI-configuration for all parameters
    pi_spi_conf_init(&spi_config);  

    // size_t data_size = length(data); // Data size in bit 

    // Set the SPI-output of the GAP8 as an output explicitely
    pi_gpio_pin_configure(&nina, PI_GPIO_A13_PAD_27_A38, PI_GPIO_OUTPUT);

    // Open device (Nina-Chip) & SPI for communication
    pi_open_from_conf(&nina, &spi_config);
    printf("[SPI] Open\n");
    
    // Open SPI to start transfer
    if (pi_spi_open(&nina))
    {
        printf("[SPI] open failed !\n");
        pmsis_exit(-1);
    }

    pi_spi_open(&nina); 
    
    // Send data via SPI
    while(1)
    {
        // toggle LED when sending information
        pi_gpio_pin_write(&gpio_device, 2, led_val);
        led_val ^= 1;
        pi_task_push_delayed_us(pi_task_callback(&led_task, led_handle, NULL), 500000);

        // send data
        pi_spi_send(&nina, &letter, 8, 0);

    // TODO: confirmation of received data needed ?
    // Use functions from SPI Slave Docu in ESP32 Docu
    }
    printf("SPI transfer completed !\n");

    pi_spi_close(&nina);

    // Stop function & exit platform
    pmsis_exit(0);
}

/* 
 * This uses a transport layer over SPI to send multiple bytes at a time
 * instead of sending one byte at time with bare SPI functions
 * like in the previous appraoch
 */
static void send_char_transport(void){
    // configure LED
    pi_gpio_pin_configure(&gpio_device, 2, PI_GPIO_OUTPUT);
    static PI_L2 char *data = "data from ai deck\n";

    if (open_transport(&nina))
    {
        printf("Failed to open transport to nina\n");
        return;
    }
    printf("Opened transport to nina\n");

    // Send data via transport over SPI to nina
    while(1)
    {
        // toggle LED when sending information
        pi_gpio_pin_write(&gpio_device, 2, led_val);
        led_val ^= 1;
        pi_task_push_delayed_us(pi_task_callback(&led_task, led_handle, NULL), 500000);

        // send data
        pi_transport_send(&nina, data, strlen(data));
        printf("data sent\n");
    }
    printf("SPI transfer completed !\n");

    // Stop function & exit platform
    pmsis_exit(0);
}


int main(void){
    // Start pmsis system on Gap8 with SPI-send function to be executed
    return pmsis_kickoff((void *)send_char_transport);
}