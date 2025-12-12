#include "api.h"

// Entry point
extern "C" int main() {
  int selected = 0;

  while (1) {
    os_clear();
    os_draw_string(40, 5, "FlipperOS Shell");
    os_draw_string(10, 20, "Select App:");

    if (selected == 0)
      os_draw_string(10, 35, "> Word Editor");
    else
      os_draw_string(10, 35, "  Word Editor");

    if (selected == 1)
      os_draw_string(10, 45, "> Terminal");
    else
      os_draw_string(10, 45, "  Terminal");

    os_flush();

    // Blocking wait for simple toggle
    // In real app we'd check input
    // dummy delay
    for (volatile int i = 0; i < 100000; i++)
      ;
  }
}
