#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/param.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"

#include "driver/uart.h"
#include "sdkconfig.h"

#include "spi.h"

static const int RX_BUF_SIZE = 1024;    //Buffer Size?
static const int TX_BUF_SIZE = 1024;    //Buffer Size?

#define TXD_PIN (GPIO_NUM_22)            //Correct GPIO?
#define RXD_PIN (GPIO_NUM_23)            //Correct GPIO?

#define uart_num UART_NUM_0

/* Pointer to data transferred via SPI */
static uint8_t *spi_buffer;

/* The LED is connected on GPIO */
#define BLINK_GPIO 4

/* The period of the LED blinking */
static uint32_t blink_period_ms = 500;


static int value = 0xBC;

void uart_init(void) {

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

    uart_set_pin(uart_num, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(uart_num, RX_BUF_SIZE * 2, TX_BUF_SIZE * 2, 0, NULL, 0);
}

/* Handle packet received from the GAP8 and send over UART2 */
static void uart_sendData(uint8_t *buffer) {
    // Send the value 0xBC to enable the UART and reset the bootloader state
    uart_write_bytes(uart_num, value, sizeof(value));
	//int32_t datalength = spi_read_data(&buffer, CMD_PACKET_SIZE);
	uart_write_bytes(UART_NUM_0, (const char *)buffer, sizeof(buffer));
}

static void data_sending_task(void) {
    while (1) {
        int32_t datalength = spi_read_data(&spi_buffer, CMD_PACKET_SIZE);
        if (datalength > 0) {
            // Write data to UART.
            uart_sendData(spi_buffer);
        }
    }
}

//Main application
void app_main() {
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(BLINK_GPIO, 1);

    uart_init();
    spi_init();
    xTaskCreate(data_sending_task, "data_sending_task", 4096, NULL, 5, NULL);

    while(1) {
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(blink_period_ms/2));
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(blink_period_ms/2));
    }
}