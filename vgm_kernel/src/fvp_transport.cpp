#include "fvp_transport.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "pico/stdlib.h"
#include <stdio.h>

// Concrete implementation using Pico UART
class UartTransport : public FvpTransport {
public:
  void init() override {
    // Init UART0 at 921600 baud for high speed frame transfer
    uart_init(uart0, 921600);

    // Set TX/RX pins (GP0, GP1 are default for UART0)
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);

    // Disable flow control for simplicity, enable FIFO
    uart_set_hw_flow(uart0, false, false);
    uart_set_format(uart0, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(uart0, true);
  }

  void send_video_frame(const uint8_t *buffer, size_t size) override {
    FvpHeader header;
    header.magic = FVP_MAGIC;
    header.version = FVP_VERSION;
    header.command = CMD_VIDEO_FRAME;
    header.payload_len = (uint16_t)size;

    uart_write_blocking(uart0, (const uint8_t *)&header, sizeof(header));
    uart_write_blocking(uart0, buffer, size);
  }

  bool receive_packet(FvpHeader *header, uint8_t *payload,
                      size_t max_payload) override {
    // Non-blocking check for header
    if (!uart_is_readable(uart0))
      return false;

    // If we have data, try to read header (peek or state machine in real impl)
    // For simplicity here, if we see magic byte, we block-read the rest
    // REAL IMPL should use a state machine buffer

    // Simple blocking read for now to ensure sync:
    uart_read_blocking(uart0, (uint8_t *)header, sizeof(FvpHeader));

    if (header->magic != FVP_MAGIC) {
      // Sync lost or garbage
      return false;
    }

    if (header->payload_len > 0 && header->payload_len <= max_payload) {
      uart_read_blocking(uart0, payload, header->payload_len);
    }

    return true;
  }

  void send_packet(const FvpHeader &header, const uint8_t *payload) override {
    uart_write_blocking(uart0, (const uint8_t *)&header, sizeof(header));
    if (payload && header.payload_len > 0) {
      uart_write_blocking(uart0, payload, header.payload_len);
    }
  }
};

static UartTransport s_transport;

FvpTransport &FvpTransport::get_instance() { return s_transport; }
