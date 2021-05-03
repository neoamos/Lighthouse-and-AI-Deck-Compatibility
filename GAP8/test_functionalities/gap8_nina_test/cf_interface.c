
#include "pmsis.h"
#include "bsp/transport/nina_w10.h"
#include "stdio.h"

#include "cf_interface.h"

static struct pi_device nina; 
static struct pi_device uart;

#define RX_BUFFER_SIZE 1024
static uint8_t buffer[RX_BUFFER_SIZE];
static int data_len;
static pi_task_t rcv_task;
static void (*msg_callback) (uint8_t*, int);

void cf_receive_async();

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

static int open_uart(struct pi_device *device){
  // set configurations in uart
  struct pi_uart_conf conf;
  pi_uart_conf_init(&conf);
  conf.baudrate_bps =230400;
  conf.enable_tx = 0;
  conf.enable_rx = 1;

  // Open uart
  pi_open_from_conf(device, &conf);
  if (pi_uart_open(device))
  {
    return -1;
  }
  return 0;
}

void cf_send(uint8_t *buffer, int len){
  if(len > 1024){
    printf("Don't send messages over 1024 bytes\n");
    return;
  }
  pi_transport_send(&nina, buffer, len);
}

void cf_receive_handle(void *args){
  // printf("receive handle\n");
  msg_callback(buffer, data_len);
  cf_receive_async();
}

void cf_receive_header_handle(void *args){
  data_len = *((int*)buffer);
  // printf("receive header handle\n");
  pi_uart_read_async(&uart, buffer, data_len, pi_task_callback(&rcv_task, cf_receive_handle, NULL));
}

void cf_receive_soh_handle(void *args){
  // printf("receive soh handle\n");
  if(buffer[0] == 0x81){
    pi_uart_read_async(&uart, buffer, 4, pi_task_callback(&rcv_task, cf_receive_header_handle, NULL));
  }else{
    cf_receive_async();
  }
}

void cf_receive_async(){
  buffer[0] = 0;
  pi_uart_read_async(&uart, buffer, 1, pi_task_callback(&rcv_task, cf_receive_soh_handle, NULL));
}

void cf_interface_init( void (*callback) (uint8_t*, int) ){
  if (open_transport(&nina))
  {
      printf("Failed to open transport to nina\n");
      return;
  }
  printf("Opened transport to nina\n");

  if(callback != NULL){
    if(open_uart(&uart)){
      printf("Failed to open uart\n");
      return;
    }
    printf("Opened uart\n");

    msg_callback = callback;
    cf_receive_async();
  }

}