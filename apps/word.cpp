#include "api.h"

extern "C" int main() {
  os_clear();
  os_draw_string(0, 0, "Word Editor v1.0");
  os_draw_string(0, 10, "Type something...");
  os_flush();

  // In a real loop we would read keyboard events
  // and process text buffer

  while (1)
    ;
}
