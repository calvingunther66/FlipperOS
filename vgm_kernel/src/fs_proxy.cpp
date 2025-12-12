#include "fs_proxy.h"
#include "../../shared/fvp_protocol.h"
#include "fvp_transport.h"
#include <stdio.h>
#include <string.h>

// Blocking calls using Transport
// Note: This is a synchronous implementation which might block the UI if not
// threaded But for single-tasking OS it's fine.

FvpFileHandle fvp_fopen(const char *path, uint8_t mode) {
  FvpTransport &transport = FvpTransport::get_instance();

  // 1. Send Open Request
  FvpFileOpenReq req;
  req.flags = mode;
  strncpy(req.path, path, sizeof(req.path) - 1);

  FvpHeader header;
  header.magic = FVP_MAGIC;
  header.version = FVP_VERSION;
  header.command = CMD_FILE_OPEN;
  header.payload_len = sizeof(req);

  transport.send_packet(header, (const uint8_t *)&req);

  // 2. Wait for Response (Spinlock for now)
  // In real code: transport.waitForResponse();
  // Returning dummy handle 1 for skeleton correctness
  return 1;
}

size_t fvp_fread(void *ptr, size_t size, size_t count, FvpFileHandle stream) {
  // Send Read Request
  return 0; // Stub
}

size_t fvp_fwrite(const void *ptr, size_t size, size_t count,
                  FvpFileHandle stream) {
  return 0; // Stub
}

int fvp_fclose(FvpFileHandle stream) { return 0; }
