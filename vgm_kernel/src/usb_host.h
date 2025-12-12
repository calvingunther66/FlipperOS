#pragma once

// Init USB Host stack
void usb_host_init();

// Poll USB Host task
void usb_host_task();

// Global cursor (temp)
extern int g_cursor_x;
extern int g_cursor_y;

bool pop_key(uint8_t *key);
