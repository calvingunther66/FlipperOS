#pragma once
#include <stddef.h>
#include <stdint.h>

class VideoSystem {
public:
  static const int WIDTH = 128;
  static const int HEIGHT = 64;
  static const size_t BUFFER_SIZE = (WIDTH * HEIGHT) / 8;

  VideoSystem();
  void init();

  // Clear buffer (0 = black, 1 = white usually, but Flipper is monochrome)
  void clear(bool color = false);

  // Set pixel
  void set_pixel(int x, int y, bool color);

  // Draw string (simple built-in font)
  void draw_string(int x, int y, const char *str);

  // Send current buffer to Flipper
  void flush();

  uint8_t *get_buffer() { return buffer_; }

private:
  uint8_t buffer_[BUFFER_SIZE];
};
