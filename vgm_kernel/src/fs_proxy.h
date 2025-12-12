#pragma once
#include <stddef.h>
#include <stdint.h>

// Handle type
typedef uint32_t FvpFileHandle;

#define FVP_FILE_INVALID 0

// FS Modes
#define FVP_MODE_READ 1
#define FVP_MODE_WRITE 2

FvpFileHandle fvp_fopen(const char *path, uint8_t mode);
size_t fvp_fread(void *ptr, size_t size, size_t count, FvpFileHandle stream);
size_t fvp_fwrite(const void *ptr, size_t size, size_t count,
                  FvpFileHandle stream);
int fvp_fclose(FvpFileHandle stream);
