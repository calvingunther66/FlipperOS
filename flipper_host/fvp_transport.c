#include "fvp_transport.h"
#include <furi_hal.h>

void fvp_host_init() {
  // Init GPIO/SPI/UART
  // furi_hal_uart_init(...)
}

void fvp_host_send_input(FvpInputEvent *event) {
  // Send input packet to VGM
  // furi_hal_uart_tx(...)
}

bool fvp_host_receive(uint8_t *video_buffer, size_t video_buffer_len) {
  // Poll for data, if video frame received, fill buffer
  return false;
}
