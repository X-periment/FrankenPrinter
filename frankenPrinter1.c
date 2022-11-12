By Xperiment Nov 2022, https://youtu.be/hN9Ii68lmqY

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "hardware/spi.h"
#include "frankenPrinter1.pio.h"


#define PIO_INPUT_PIN_BASE 0 

#define BUF_SIZE 1000

#define DATA_BUF_SIZE 1000

volatile int head;
volatile int tail;
volatile int size;
volatile int data_buffer[DATA_BUF_SIZE];

void buffer_init(void) {
  head = 0;
  tail = 0;
  size = 0;
}
void buffer_push(int val) {
  if (size < DATA_BUF_SIZE-1) {
    size++;
    data_buffer[head]=val;
    head++;
  } else {
    printf("data buffer overflow\n");
  }
}

int buffer_pop(void) {
  if (size >0) {
    size--;
    tail++;
    return data_buffer[tail-1];
  } else {
    printf("data buffer read while empty\n");
  }
}




volatile uint32_t rxbuf[BUF_SIZE], data_fb;
volatile uint sm;
volatile PIO pio;
volatile int count_wr;
volatile int count_rd_other, count_rd0, count_rd1, count_rd2,  count_wr_fd, count_wr_fe, count_wr_ff, count_wr_fb, count_wr_other;
volatile int new_data;


#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 
  
void pio_irh() {

  if (pio0_hw->irq & 1) {
    pio0_hw->irq =1;
  } else if (pio0_hw->irq & 2) {
    pio0_hw->irq =2;
  }

  while(!pio_sm_is_rx_fifo_empty(pio, sm)) {
    uint32_t raw = pio_sm_get_blocking(pio, sm);

    int iorq_n = (raw>>8)&1;
    int wr_n = (raw>>11)&1;
    int rd_n = (raw>>9)&1;
    int mreq_n = (raw>>12)&1;
    uint32_t address121110 = (raw>>26)&7;
    uint32_t address = (raw>>13)&1023;
    address = address + (address121110<<10); 
    uint32_t data = raw&0xff;
    if (mreq_n ==0) {
      if (wr_n ==0) {
        if (address == 0x1fff) {
          buffer_push(0xff00+data);
        }
        if (address == 0x1ffe) {
          buffer_push(0xfe00+data);
        }
      }

    }

  }

}

int main() {
  
  buffer_init();
 
  stdio_init_all();


  pio = pio0;
  uint offset = pio_add_program(pio, &frankenPrinter1_wr_program);
  sm = pio_claim_unused_sm(pio, true);
  frankenPrinter1_wr_program_init(pio, sm, offset, PIO_INPUT_PIN_BASE);

  irq_set_exclusive_handler(PIO0_IRQ_0, pio_irh);
  irq_set_enabled(PIO0_IRQ_0,true);
  pio0_hw->inte0 = PIO_IRQ0_INTE_SM0_BITS | PIO_IRQ0_INTE_SM1_BITS;
  for (int i =0;i<16;i++) {
    gpio_disable_pulls(i);
  }
    
  count_wr = 0;
  count_rd0 = 0;
  count_rd1 = 0;
  count_rd2 = 0;
  count_rd_other = 0;
  count_wr_fb = 0;
  count_wr_fd = 0;
  count_wr_fe = 0;
  count_wr_ff = 0;
  
  int print_count = 0;

  while (true) {
    
    if (size>0) {
      int val =buffer_pop();
      int mtype = val&0xff00;
      int mdata = val&0xff;
      if (mtype==0xff00) {
        printf("R %d\n",mdata);
      }
      else if (mtype==0xfe00) {
        printf("V %d\n",mdata);
      }
    }
    
  }
}
