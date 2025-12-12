#include "fvp_transport.h"
#include "pico/stdlib.h"
#include <stdio.h>

// Concrete implementation using UART (Mock for now, replacing with PIO later)
class UartTransport : public FvpTransport {
public:
  void init() override {
    // Assume stdio is already init for now, or init specific UART
    // uart_init(uart0, 115200);
    // ... pins setup ...
  }

  void send_video_frame(const uint8_t *buffer, size_t size) override {
    // Send Magic for Video Frame
    FvpHeader header;
    header.magic = FVP_MAGIC;
    header.version = FVP_VERSION;
    header.command = CMD_VIDEO_FRAME;
    header.payload_len = (uint16_t)size;

    // In a real high-speed impl, this would use DMA
    // For now, we just print a log to simulate
    // fwrite(&header, sizeof(header), 1, stdout);
    // fwrite(buffer, size, 1, stdout);
  }

  bool receive_packet(FvpHeader *header, uint8_t *payload,
                      size_t max_payload) override {
    // Check if data available
    // if (!uart_is_readable(uart0)) return false;

    // This is a blocking read simulation for the flow
    // In real loop, we would parse byte by byte state machine
    return false;
  }

  void send_packet(const FvpHeader &header, const uint8_t *payload) override {
    // fwrite(&header, sizeof(header), 1, stdout);
    // if (payload && header.payload_len > 0) {
    //     fwrite(payload, header.payload_len, 1, stdout);
    // }
  }
};

static UartTransport s_transport;

FvpTransport &FvpTransport::get_instance() { return s_transport; }
