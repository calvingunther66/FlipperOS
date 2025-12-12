#include "api.h"

extern "C" int main() {
  os_clear();
  os_draw_string(0, 0, "Word Editor v1.0");
  os_draw_string(0, 10, "_");
  os_flush();

  char buffer[128] = {0};
  int pos = 0;

  while (1) {
    char key = 0;
    if (get_syscalls()->get_input(&key)) {
      if (key == '\b') {
        if (pos > 0) {
          pos--;
          buffer[pos] = 0;
        }
      } else if (key == '\n') {
        // Newline not supported in this simple view
      } else {
        if (pos < 127) {
          buffer[pos++] = key;
          buffer[pos] = 0;
        }
      }

      os_clear();
      os_draw_string(0, 0, "Word Editor v1.0");
      os_draw_string(0, 10, buffer);
      os_flush();
    }

    // small delay to prevent cpu hogging if we had sleep
  }
}
