#include "pico/stdlib.h"
#include "tusb.h"
#include <stdio.h>

// Forward declaration or include for VideoSystem to update cursor
// #include "video.h"

// Global cursor state (temporary, should be in a proper InputManager)
int g_cursor_x = 64;
int g_cursor_y = 32;

//--------------------------------------------------------------------+
// TinyUSB Host Callbacks
//--------------------------------------------------------------------+

// Invoked when device with HID interface is mounted
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance,
                      uint8_t const *desc_report, uint16_t desc_len) {
  printf("HID device mounted, address = %d, instance = %d\r\n", dev_addr,
         instance);

  // Request to receive report
  // tuh_hid_receive_report(dev_addr, instance);
  // Note: real implementation needs to parse report descriptor or assume boot
  // protocol For simplicity we often use boot protocol or generic parsing
}

// Invoked when device with HID interface is unmounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
  printf("HID device unmounted, address = %d, instance = %d\r\n", dev_addr,
         instance);
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance,
                                uint8_t const *report, uint16_t len) {
  // Process report (Movement, Keys)
  printf("HID Report: %d bytes\r\n", len);

  // If mouse:
  // Update g_cursor_x, g_cursor_y

  // Continue to request to receive report
  tuh_hid_receive_report(dev_addr, instance);
}

void usb_host_init() { tusb_init(); }

void usb_host_task() { tuh_task(); }
