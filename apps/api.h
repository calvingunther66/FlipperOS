#pragma once
#include <stddef.h>
#include <stdint.h>

// Syscall Table Entry Point (Convention)
// The Kernel must populate this table at this address
#define SYSCALL_TABLE_ADDR 0x20000080

typedef struct {
  void (*draw_pixel)(int x, int y, int color);
  void (*draw_string)(int x, int y, const char *str);
  void (*draw_clear)(int color);
  void (*flush)();

  // Simple blocking input
  // Returns type of event, fills data
  int (*get_input)(int *data);

  // File I/O
  uint32_t (*fopen)(const char *path, int mode);
  void (*fclose)(uint32_t handle);
  // ...
} SyscallTable;

static inline SyscallTable *get_syscalls() {
  return (SyscallTable *)SYSCALL_TABLE_ADDR;
}

// Wrapper Functions
inline void os_draw_string(int x, int y, const char *str) {
  get_syscalls()->draw_string(x, y, str);
}

inline void os_clear() { get_syscalls()->draw_clear(0); }

inline void os_flush() { get_syscalls()->flush(); }
