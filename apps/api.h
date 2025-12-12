#pragma once
#include <stddef.h>
#include <stdint.h>

// Fixed address for the syscall table (must match Kernel location)
#define SYSCALL_TABLE_ADDR 0x20000080

typedef struct {
  void (*draw_pixel)(int x, int y, bool on);
  void (*draw_string)(int x, int y, const char *str);
  void (*clear)();
  void (*flush)();
  bool (*get_input)(void *event); // Generic event ptr
  int (*fopen)(const char *path, uint8_t mode);
  size_t (*fread)(void *ptr, size_t size, size_t count, int stream);
  size_t (*fwrite)(const void *ptr, size_t size, size_t count, int stream);
  void (*fclose)(int handle);
  // Add more as needed
} SyscallTable;

// Helper to access the table
static inline const SyscallTable *get_syscalls() {
  return (const SyscallTable *)SYSCALL_TABLE_ADDR;
}

// Userland wrappers
static inline void os_draw_pixel(int x, int y, bool on) {
  get_syscalls()->draw_pixel(x, y, on);
}
static inline void os_draw_string(int x, int y, const char *str) {
  get_syscalls()->draw_string(x, y, str);
}
static inline void os_clear() { get_syscalls()->clear(); }
static inline void os_flush() { get_syscalls()->flush(); }
static inline int os_fopen(const char *path, uint8_t mode) {
  return get_syscalls()->fopen(path, mode);
}
// etc...
