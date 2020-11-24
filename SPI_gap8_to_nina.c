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
#include "stdio.h"

static void send_char_SPI(void)
// how to give data to the function ?
{
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
    pi_open_from_conf(&nina, &config);
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
        pi_spi_send(&nina, &letter, 8, 0);

    // TODO: confirmation of received data needed ?
    // Use functions from SPI Slave Docu in ESP32 Docu
    }
    printf("SPI transfer completed !\n");

    // Close SPI-Device (NINA) to free allocated resources
    if (pi_spi_close(&nina))
    {
        printf("[SPI] close failed !\n");
        pmsis_exit(-1);
    }

    pi_spi_close(&nina)

    // Stop function & exit platform
    pmsis_exit(0);
}

int main(void)
{
    // Start pmsis system on Gap8 with SPI-send function to be executed
    return pmsis_kickoff((void *)send_char_SPI);
}