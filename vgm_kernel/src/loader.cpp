#include "fs_proxy.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Defined in linker script or config
#define USERLAND_BASE 0x20010000

typedef int (*AppEntryPtr)();

class AppLoader {
public:
  static bool load_and_run(const char *filename) {
    printf("Loading app: %s\n", filename);

    // 1 -> Read Mode
    FvpFileHandle file = fvp_fopen(filename, 1);
    if (!file) { // 0 is invalid
      printf("Failed to open app file\n");
      return false;
    }

    void *dest = (void *)USERLAND_BASE;
    uint8_t *byte_dest = (uint8_t *)dest;
    size_t total_read = 0;

    // Read loop
    while (true) {
      size_t read = fvp_fread(byte_dest + total_read, 1, 1024, file);
      if (read == 0)
        break; // EOF or Error
      total_read += read;

      // Safety check
      if (total_read > (192 * 1024)) {
        printf("App too large!\n");
        fvp_fclose(file);
        return false;
      }
    }

    fvp_fclose(file);
    printf("Loaded %d bytes. Jumping to 0x%08x...\n", total_read,
           USERLAND_BASE);

    // Disable interrupts or resource cleanup before jump might be needed in
    // real OS For now we just go.

    // Pointer to entry function
    AppEntryPtr entry = (AppEntryPtr)USERLAND_BASE;

    // Execute
    int result = entry();

    printf("App finished with code %d\n", result);
    return true;
  }
};
