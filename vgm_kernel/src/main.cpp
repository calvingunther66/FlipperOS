#include "../../shared/fvp_protocol.h"
#include "fvp_transport.h"
#include "pico/stdlib.h"
#include "video.h"
#include <stdio.h>

// Define LED pin if not defined by board header
#ifndef PICO_DEFAULT_LED_PIN
#define PICO_DEFAULT_LED_PIN 25
#endif

int main() {
  stdio_init_all();

  // Init HW
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  // Init Subsystems
  FvpTransport::get_instance().init();
  VideoSystem video;
  video.init();

  printf("FlipperOS Kernel Booting...\n");
  printf("FVP Version: %d\n", FVP_VERSION);

  uint32_t frame_count = 0;

  while (true) {
    // 1. Poll Transport (Receive Input/Storage/Ping)
    // In a real implementation this might be IRQ driven, but we can poll for
    // now
    FvpHeader header;
    uint8_t payload[256];
    if (FvpTransport::get_instance().receive_packet(&header, payload,
                                                    sizeof(payload))) {
      // Handle packet
    }

    // 2. Poll TinyUSB (TODO)
    // tuh_task();

    // 3. Update Graphics
    video.clear();
    video.draw_string(2, 2, "FlipperOS v0.1");

    char buf[32];
    snprintf(buf, sizeof(buf), "Frame: %lu", (unsigned long)frame_count++);
    video.draw_string(2, 12, buf);

    // Draw a generic moving box
    int box_x = (frame_count / 2) % (VideoSystem::WIDTH - 10);
    int box_y = 30 + (frame_count / 10) % 10;

    for (int y = 0; y < 10; y++) {
      for (int x = 0; x < 10; x++) {
        if (x == 0 || x == 9 || y == 0 || y == 9)
          video.set_pixel(box_x + x, box_y + y, true);
      }
    }

    // Flush to Flipper
    video.flush();

    // Blink LED
    gpio_put(PICO_DEFAULT_LED_PIN, (frame_count % 20) < 10);

    // Cap framerate slightly to avoid saturating UART if that's what we use
    sleep_ms(33);
  }

  return 0;
}
