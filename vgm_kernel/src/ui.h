#pragma once
#include "video.h"
#include <string>
#include <vector>

class Window {
public:
  Window(int x, int y, int w, int h, const char *title)
      : x_(x), y_(y), w_(w), h_(h), title_(title) {}

  virtual void draw(VideoSystem &video) {
    // Draw border
    for (int i = x_; i < x_ + w_; i++) {
      video.set_pixel(i, y_, true);
      video.set_pixel(i, y_ + h_ - 1, true);
    }
    for (int i = y_; i < y_ + h_; i++) {
      video.set_pixel(x_, i, true);
      video.set_pixel(x_ + w_ - 1, i, true);
    }
    // Title
    video.draw_string(x_ + 2, y_ + 2, title_.c_str());

    // Content area clear
    // ...
  }

  int x_, y_, w_, h_;
  std::string title_;
};

class Desktop {
public:
  void add_window(Window *win) { windows_.push_back(win); }

  void draw(VideoSystem &video) {
    // Draw background
    // ...

    // Draw windows
    for (auto *win : windows_) {
      win->draw(video);
    }

    // Draw Taskbar
    video.draw_string(0, 56, "[Start]  12:00");
  }

private:
  std::vector<Window *> windows_;
};

class Cursor {
public:
  void update(int dx, int dy) {
    x_ += dx;
    y_ += dy;
    if (x_ < 0)
      x_ = 0;
    if (x_ > 127)
      x_ = 127;
    if (y_ < 0)
      y_ = 0;
    if (y_ > 63)
      y_ = 63;
  }

  void draw(VideoSystem &video) {
    video.set_pixel(x_, y_, true);
    video.set_pixel(x_ + 1, y_ + 1, true);
    video.set_pixel(x_, y_ + 1, true);
    video.set_pixel(x_ + 1, y_, true);
  }

  int x_ = 64;
  int y_ = 32;
};
