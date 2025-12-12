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

    FvpFileHandle file = fvp_fopen(filename, FVP_MODE_READ);
    if (file == FVP_FILE_INVALID) {
      printf("Failed to open app file\n");
      return false;
    }

    // In a real loader, we would read ELF headers.
    // For this flat binary or simple model, we just read into RAM.

    void *dest = (void *)USERLAND_BASE;
    // Mock size for now, loop read until EOF
    // size_t bytes = fvp_fread(dest, 1, 64*1024, file);

    // Mock loading code for simulation
    // In reality, this would overwrite memory.

    fvp_fclose(file);

    printf("Jumping to Userland at 0x%08x...\n", USERLAND_BASE);

    // This is the jump
    // We probably need to suspend OS tasks here

    AppEntryPtr entry = (AppEntryPtr)USERLAND_BASE;

    // Execute
    // int result = entry();

    // printf("App finished with code %d\n", result);
    return true;
  }
};
