#pragma once
#include "../../shared/fvp_protocol.h"
#include <stddef.h>
#include <stdint.h>

// Abstract Transport Class
class FvpTransport {
public:
  virtual void init() = 0;

  // Video Stream
  // Sends a full framebuffer to the Flipper
  virtual void send_video_frame(const uint8_t *buffer, size_t size) = 0;

  // Command/Data Stream
  // Returns true if a packet was read
  virtual bool receive_packet(FvpHeader *header, uint8_t *payload,
                              size_t max_payload) = 0;

  // Send Input/Storage packet
  virtual void send_packet(const FvpHeader &header, const uint8_t *payload) = 0;

  static FvpTransport &get_instance();
};
