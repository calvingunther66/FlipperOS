#include "video.h"
#include "fvp_transport.h"
#include <string.h>

// Simple 5x7 font data (subset) could go here or be included
// For brevity, we implement a dummy draw_char
static const uint8_t font5x7[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, // Space
    0x7F, 0x09, 0x09, 0x09, 0x06, // A
    0x7F, 0x49, 0x49, 0x49, 0x36, // B
                                  // ... complete font would be needed
};

VideoSystem::VideoSystem() { clear(); }

void VideoSystem::init() { clear(); }

void VideoSystem::clear(bool color) {
  memset(buffer_, color ? 0xFF : 0x00, BUFFER_SIZE);
}

void VideoSystem::set_pixel(int x, int y, bool color) {
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
    return;

  size_t index = (y / 8) * WIDTH + x;
  uint8_t bit = 1 << (y % 8);

  if (color) {
    buffer_[index] |= bit;
  } else {
    buffer_[index] &= ~bit;
  }
}

void VideoSystem::draw_string(int x, int y, const char *str) {
  // Simple placeholder for text drawing
  // In real impl, iterate chars and look up font
  int cx = x;
  while (*str) {
    // draw_char(*str, cx, y);
    cx += 6;
    str++;
  }
}

void VideoSystem::flush() {
  FvpTransport::get_instance().send_video_frame(buffer_, BUFFER_SIZE);
}
