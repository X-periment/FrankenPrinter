// By Xperiment  November 2022, video available at: https://youtu.be/tLPAEYJCyFs

//THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION



#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "hardware/spi.h"
#include "frankenPrinter2.pio.h"

//#define SPI_SCK_PIN 2
//#define SPI_TX_PIN 3
// GPIO 4 for PIO data input, GPIO 5 for clock input:
#define PIO_INPUT_PIN_BASE 0 //IORQ 

#define BUF_SIZE 1000
volatile uint32_t rxbuf[BUF_SIZE], data_fb;
volatile uint sm, sm2;
volatile PIO pio;
volatile int count_wr;
volatile int count_rd_other, count_rd0, count_rd1, count_rd2,  count_wr_fd, count_wr_fe, count_wr_ff, count_wr_fb, count_wr_other;
volatile int new_data;
volatile int pins_val;

volatile int picture_count;

int arr[] = {31, 12, 30, 13, 13, 13, 13, 13, 13, 13, 33, 32, 15, 18, 11, 11, 18, 16, 18, 23, 13, 13, 13, 28, 31, 1, 12, 30, 33, 28, 13, 28, 13, 28, 1, 20, 16, 16, 16, 11, 11, 18, 11, 29, 13, 13, 13, 20, 31, 1, 12, 30, 28, 28, 33, 1, 33, 1, 23, 29, 23, 29, 15, 18, 18, 18, 18, 23, 12, 13, 30, 16, 32, 31, 1, 30, 33, 33, 12, 13, 13, 48, 23, 32, 12, 23, 23, 16, 16, 16, 18, 16, 15, 32, 15, 16, 31, 12, 30, 33, 33, 12, 3, 13, 3, 3, 13, 13, 31, 20, 15, 20, 18, 11, 18, 16, 16, 16, 16, 16, 31, 12, 12, 30, 30, 13, 3, 3, 3, 13, 13, 12, 32, 48, 1, 29, 18, 14, 26, 18, 16, 18, 18, 16, 1, 12, 28, 13, 30, 13, 3, 3, 141, 3, 3, 13, 29, 20, 23, 15, 26, 14, 14, 11, 18, 18, 18, 18, 1, 28, 13, 13, 13, 3, 3, 13, 3, 3, 3, 3, 140, 176, 161, 141, 3, 13, 18, 14, 11, 18, 18, 18, 31, 33, 13, 3, 3, 3, 141, 3, 161, 7, 141, 141, 157, 148, 157, 7, 141, 158, 141, 20, 14, 18, 18, 11, 30, 13, 13, 3, 3, 158, 160, 3, 159, 141, 141, 141, 151, 157, 148, 7, 141, 7, 151, 7, 33, 26, 18, 29, 31, 28, 13, 3, 141, 157, 143, 159, 176, 141, 3, 161, 140, 141, 3, 160, 141, 3, 3, 3, 141, 23, 26, 13, 32, 30, 3, 141, 161, 151, 144, 143, 143, 156, 141, 3, 141, 151, 3, 160, 141, 3, 7, 160, 3, 3, 15, 141, 23, 30, 3, 156, 7, 157, 143, 148, 143, 176, 141, 7, 7, 143, 158, 159, 141, 141, 161, 161, 140, 3, 3, 28, 15, 13, 3, 141, 140, 151, 143, 148, 144, 143, 176, 7, 156, 7, 158, 176, 156, 141, 141, 141, 141, 176, 141, 13, 28, 3, 141, 161, 159, 151, 143, 157, 143, 151, 7, 159, 159, 161, 7, 160, 161, 141, 141, 141, 159, 143, 141, 15, 3, 141, 141, 161, 176, 148, 144, 159, 143, 160, 141, 7, 148, 148, 159, 160, 161, 3, 3, 140, 143, 144, 13, 18, 3, 141, 141, 161, 176, 143, 144, 141, 143, 141, 141, 161, 143, 144, 151, 159, 140, 3, 160, 151, 144, 159, 16, 18, 3, 141, 141, 158, 160, 148, 143, 161, 143, 141, 13, 161, 143, 144, 144, 159, 160, 158, 160, 143, 144, 13, 16, 18, 3, 3, 141, 156, 159, 157, 157, 7, 148, 158, 141, 161, 151, 144, 143, 157, 176, 176, 160, 157, 141, 18, 18, 18, 3, 141, 156, 141, 158, 159, 160, 176, 157, 157, 159, 161, 157, 143, 148, 157, 176, 176, 148, 159, 29, 16, 16, 16, 3, 3, 156, 141, 156, 140, 7, 159, 140, 161, 7, 160, 159, 176, 176, 176, 7, 160, 159, 12, 18, 18, 18, 16, 13, 3, 141, 141, 141, 161, 140, 7, 160, 160, 159, 7, 161, 141, 141, 141, 141, 156, 3, 18, 16, 16, 15, 15, 13, 13, 141, 141, 141, 141, 161, 140, 7, 160, 140, 158, 156, 141, 141, 141, 156, 3, 16, 16, 16, 16, 20, 23, 1, 13, 3, 141, 156, 141, 141, 156, 158, 160, 159, 140, 141, 3, 141, 156, 3, 15, 11, 16, 16, 15, 23, 20, 31, 32, 13, 3, 141, 161, 161, 159, 176, 157, 176, 158, 141, 160, 157, 157, 13, 11, 18, 16, 15, 23, 20, 29, 32, 32, 29, 13, 3, 141, 140, 158, 161, 157, 7, 141, 7, 143, 159, 141, 15, 18, 16, 15, 15, 20, 15, 3, 1, 29, 16, 16, 15, 3, 3, 141, 141, 140, 156, 141, 141, 159, 157, 141, 1, 18, 18, 16, 15, 15, 13, 3, 32, 15, 11, 26, 14, 18, 32, 28, 3, 3, 141, 141, 3, 140, 157, 7, 13, 18, 18, 16, 15, 13, 3, 3, 23, 16, 26, 14, 14, 14, 15, 23, 30, 3, 141, 141, 161, 159, 148, 176, 31, 16, 16, 16, 33, 3, 3, 3, 16, 11, 14, 14, 14, 14, 14, 15, 16, 13, 3, 3, 156, 157, 148, 140, 1, 16, 16, 48, 3, 3, 3, 3, 16, 26, 26, 11, 26, 11, 11, 14, 11, 11, 48, 33, 3, 158, 141, 3, 16, 16, 15, 3, 3, 3, 13, 33, 18, 11, 18, 18, 18, 18, 18, 11, 26, 11, 18, 15, 29, 13, 3, 48, 16, 15, 13, 3, 3, 13, 32, 12};

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
  //irq_clear(PIO0_IRQ_0);
  if (pio0_hw->irq & 1) {
    pio0_hw->irq =1;
  } else if (pio0_hw->irq & 2) {
    pio0_hw->irq =2;
  }

  while(!pio_sm_is_rx_fifo_empty(pio, sm)) {
    uint32_t raw = pio_sm_get_blocking(pio, sm);
    data_fb = raw;
    new_data = 1;
    if (pins_val == 0) {
        pins_val = 255;
        pio_sm_put(pio, sm, 0xffffff00 + arr[picture_count]);
        picture_count++;
      
    } else {
        pins_val = 0;
        pio_sm_put(pio, sm, 0xffffff00 + arr[picture_count]);
        picture_count++;
    } 
    if (picture_count > 32*16) picture_count = 0;
 
  }

}

int main() {
 
  stdio_init_all();

  
  picture_count = 0;
  
  pins_val = 0;

  pio = pio0;
  uint offset = pio_add_program(pio, &frankenPrinter2_io_program);
  sm = pio_claim_unused_sm(pio, true);
  frankenPrinter2_io_program_init(pio, sm, offset, PIO_INPUT_PIN_BASE);  // base is 0 here
  

  irq_set_exclusive_handler(PIO0_IRQ_0, pio_irh);
  irq_set_enabled(PIO0_IRQ_0,true);
  pio0_hw->inte0 = PIO_IRQ0_INTE_SM0_BITS | PIO_IRQ0_INTE_SM1_BITS;
  for (int i =0;i<16;i++) { // why not 32??
    gpio_disable_pulls(i);
  }
    
  ;int count = 0;
  while (true) {
    if (new_data ==1) {
      printf("%x\n",data_fb);
 
      
      ;count++;
      new_data = 0;
     }

  }
}
