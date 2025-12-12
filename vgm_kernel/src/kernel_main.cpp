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

#include "ui.h"

  // ... existing code ...

  // Init Subsystems
  FvpTransport::get_instance().init();
  VideoSystem video;
  video.init();

  Desktop desktop;
  Cursor cursor;

  Window win1(10, 10, 60, 40, "My Computer");
  desktop.add_window(&win1);

  printf("FlipperOS Kernel Booting...\n");

  uint32_t frame_count = 0;

  while (true) {
    // ... polling code ...

    // Update
    // Mock mouse movement
    cursor.update((frame_count % 3) - 1, (frame_count % 2) - 1);

    // Draw
    video.clear();
    desktop.draw(video);
    cursor.draw(video);

    // Flush
    video.flush();

    // ... existing heartbeat ...

    // Flush to Flipper
    video.flush();

    // Blink LED
    gpio_put(PICO_DEFAULT_LED_PIN, (frame_count % 20) < 10);

    // Cap framerate slightly to avoid saturating UART if that's what we use
    sleep_ms(33);
  }

  return 0;
}
