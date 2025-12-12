#include "pico/stdlib.h"
#include "tusb.h"
#include <stdint.h>
#include <stdio.h>

// Forward declaration or include for VideoSystem to update cursor
// #include "video.h"

// Global cursor state (temporary, should be in a proper InputManager)
int g_cursor_x = 64;
int g_cursor_y = 32;

// Queue definitions
extern uint8_t g_key_queue[];
extern volatile int g_key_head;
extern volatile int g_key_tail;
#define KEY_QUEUE_SIZE 16

bool pop_key(uint8_t *key) {
  if (g_key_head == g_key_tail)
    return false;
  *key = g_key_queue[g_key_tail];
  g_key_tail = (g_key_tail + 1) % KEY_QUEUE_SIZE;
  return true;
}

//--------------------------------------------------------------------+
// TinyUSB Host Callbacks
//--------------------------------------------------------------------+

// Invoked when device with HID interface is mounted
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance,
                      uint8_t const *desc_report, uint16_t desc_len) {
  printf("HID device mounted, address = %d, instance = %d\r\n", dev_addr,
         instance);

  // Request to receive report
  // If we wanted to check protocol (Mouse vs Keyboard), we'd check interface
  // protocol For now, blindly request report
  tuh_hid_receive_report(dev_addr, instance);
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance,
                                uint8_t const *report, uint16_t len) {
  // Simple Mouse Report Assumption (Boot Protocol)
  // Byte 0: Buttons
  // Byte 1: X
  // Byte 2: Y

  // TODO: Check if it's actually a mouse (protocol = HID_ITF_PROTOCOL_MOUSE)
  // For prototype, assuming it is if len >= 3

  if (len >= 3) {
    int8_t dx = (int8_t)report[1];
    int8_t dy = (int8_t)report[2];

    // Update globals
    g_cursor_x += dx;
    g_cursor_y += dy;

    // Clamp (VideoSystem::WIDTH = 128, HEIGHT = 64)
    if (g_cursor_x < 0)
      g_cursor_x = 0;
    if (g_cursor_x > 127)
      g_cursor_x = 127;
    if (g_cursor_y < 0)
      g_cursor_y = 0;
    if (g_cursor_y > 63)
      g_cursor_y = 63;
  }

  // Continue to request to receive report
  tuh_hid_receive_report(dev_addr, instance);
}

void usb_host_init() { tusb_init(); }

void usb_host_task() { tuh_task(); }
