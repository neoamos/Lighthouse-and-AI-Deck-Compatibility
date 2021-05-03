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

#define RX_BUF_SIZE 1024    //Buffer Size?
#define TX_BUF_SIZE 1024    //Buffer Size?

#define TXD_PIN (GPIO_NUM_22)            //Correct GPIO?
#define RXD_PIN (GPIO_NUM_23)            //Correct GPIO?

#define uart_num UART_NUM_0

/* Pointer to data transferred via SPI */
static uint8_t *spi_buffer;

/* The LED is connected on GPIO */
#define BLINK_GPIO 4

/* The period of the LED blinking */
static uint32_t blink_period_ms = 500;

// static char value = 0xBC;

static const uint8_t SOH = 0x81;

/* GAP8 communication struct */
typedef struct
{
  uint32_t type; /* Is 0x81 for JPEG related data */
  uint32_t size; /* Size of data to request */
} __attribute__((packed)) nina_req_t;

/* GAP8 streamer packet type for JPEG related data */
#define NINA_W10_CMD_SEND_PACKET  0x81

void uart_init(void) {

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

    ESP_ERROR_CHECK(uart_set_pin(uart_num, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    // uart_set_sw_flow_ctrl(uart_num, true, 8, UART_FIFO_LEN - 8);

    ESP_ERROR_CHECK(uart_driver_install(uart_num, RX_BUF_SIZE * 2, TX_BUF_SIZE * 2, 0, NULL, 0));
}

static void send_uart_packet(uint8_t* buffer, int len){
    uart_write_bytes(uart_num, (const char*) &SOH, 1); //Start of header
    uart_write_bytes(uart_num, (const char*) &len, 4); //Size
    uart_write_bytes(uart_num, (const char*) buffer, len); //Data
}

/* Handle packet received from the GAP8 */
static void handle_gap8_package(uint8_t *buffer) {
  nina_req_t *req = (nina_req_t *)buffer;

  switch (req->type)
  {
    case NINA_W10_CMD_SEND_PACKET:
        {
            // uint32_t type = req->type;
            uint32_t size = req->size;
            
            spi_read_data(&buffer, size*8); // Make sure transfer is word aligned

            send_uart_packet(buffer, size);
            break;
        }
  }
}


static void ai_to_cf_task(void* args) {
    while (1) {
        int32_t datalength = spi_read_data(&spi_buffer, CMD_PACKET_SIZE);
        if (datalength > 0) {
            handle_gap8_package(spi_buffer);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

//Main application
void app_main() {
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(BLINK_GPIO, 1);

    vTaskDelay(pdMS_TO_TICKS(1000));
    uart_init();
    spi_init();

    xTaskCreate(ai_to_cf_task, "ai_to_cf_task", 4096, NULL, 5, NULL);

    while(1) {
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(blink_period_ms/2));
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(blink_period_ms/2));
    }
}
