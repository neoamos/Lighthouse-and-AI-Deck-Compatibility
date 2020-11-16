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
#include "SPI_send_data.h"
#include "stdio.h"

static void send_data_SPI(PI_L2 void data)
// how to give data to the function ?
{
    // configuration of the SPI parameters
    struct pi_spi_conf spi_config;  

    // Define buffer size of data to be sent
    // What Data Type ? L2_MEM ?
    static const int BUFFER_SIZE = 2048;

    // Nina-Wifi-Chip as receiving device
    // How does the program know, if it is the nina ? 
    struct pi_device nina; 

    // Default SPI-configuration for all parameters
    pi_spi_conf_init(&spi_config);  

    // Set the SPI-output of the GAP8 as an output explicitely
    pi_gpio_pin_configure(&nina, PI_GPIO_A13_PAD_27_A38, PI_GPIO_OUTPUT);

    // Open device (Nina-Chip) & SPI for communication
    pi_open_from_conf(&nina, &config);
    printf("[SPI] Open\n");
    
    // Open SPI to start transfer
    if (pi_spi_open(&nina_chip))
    {
        printf("[SPI] open failed !\n");
        pmsis_exit(-1);
    }

    pi_spi_open(&nina_chip); 
    
    // Send data via SPI
    while(1)
    {
        pi_spi_send(&nina, &letter, BUFFER_SIZE, 0);
    }
    printf("SPI transfer completed !\n");

    // Close SPI-Device (NINA) to free allocated resources
    if (pi_spi_close(&nina_chip))
    {
        printf("[SPI] close failed !\n");
        pmsis_exit(-1);
    }
    
    pi_spi_close(&nina_chip)

    // Stop function & exit platform
    pmsis_exit(0);
}

int main(void)
{
    // Start pmsis system on Gap8 with SPI-send function to be executed
    return pmsis_kickoff((void *)send_data_SPI);
}