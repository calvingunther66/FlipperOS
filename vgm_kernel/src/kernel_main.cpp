#include "../../shared/fvp_protocol.h"
#include "fvp_transport.h"
#include "pico/stdlib.h"
#include "usb_host.h"
#include "video.h"
#include <stdio.h>

#include "ui.h"

// Define LED pin if not defined by board header
#ifndef PICO_DEFAULT_LED_PIN
// Userland API
bool sys_get_input(void *event) {
  // Treat event as char* for now for simplicity
  return pop_key((uint8_t *)event);
}

// Syscall Table Implementation
SyscallTable g_syscalls = {
    .draw_pixel =
        [](int x, int y, bool on) {
          VideoSystem::get_instance().set_pixel(x, y, on);
        },
    .draw_string =
        [](int x, int y, const char *str) {
          VideoSystem::get_instance().draw_string(x, y, str);
        },
    .clear = []() { VideoSystem::get_instance().clear(); },
    .flush = []() { VideoSystem::get_instance().flush(); },
    .get_input = sys_get_input,
    .fopen = fvp_fopen,
    .fread = fvp_fread,
    .fwrite = fvp_fwrite,
    .fclose = fvp_fclose,
};

int main() {
  stdio_init_all();

  // Placed at fixed address by linker script usually, or we just copy it there
  // For this prototype, let's assume we copy it to 0x20000080
  memcpy((void *)0x20000080, &g_syscalls, sizeof(g_syscalls));

  // Init HW
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

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
    // 2. Poll TinyUSB
    usb_host_task();

    // Update
    // 3. Update Graphics
    // Set cursor from USB state
    cursor.x_ = g_cursor_x;
    cursor.y_ = g_cursor_y;

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
