#include "storage_server.h"
#include <string.h>

static Storage *g_storage = NULL;
// Simple handle map or just passing pointers (unsafe but simple for now)
// We'll trust the ID passed back is a pointer for this PoC
// In prod, use a handle table validation.

void storage_server_init() { g_storage = furi_record_open(RECORD_STORAGE); }

void storage_server_handle_command(FvpHeader *header, uint8_t *payload,
                                   FvpHeader *response_header,
                                   uint8_t *response_payload) {
  // Default response is ERROR
  response_header->magic = FVP_MAGIC;
  response_header->version = FVP_VERSION;
  response_header->command = FVP_CMD_FS_ERROR;
  response_header->payload_len = 0;

  switch (header->command) {
  case FVP_CMD_FS_OPEN: {
    FvpFsOpenReq *req = (FvpFsOpenReq *)payload;
    File *file = storage_file_alloc(g_storage);

    const char *flags_str = "r";
    if (req->flags & 1)
      flags_str = "r"; // Simplify for now
    if (req->flags & 2)
      flags_str = "w";

    if (storage_file_open(file, req->path, FSAM_READ_WRITE, FSOM_OPEN_ALWAYS)) {
      // Success
      response_header->command = FVP_CMD_FS_OK;
      // Send back file handle as 32-bit ID
      uint32_t handle = (uint32_t)file;
      memcpy(response_payload, &handle, sizeof(handle));
      response_header->payload_len = sizeof(handle);
    } else {
      storage_file_free(file);
      // Error handled by default
    }
    break;
  }
  case FVP_CMD_FS_READ: {
    // Payload: [Handle (4)] [Len (4)]
    uint32_t handle;
    uint32_t len;
    memcpy(&handle, payload, 4);
    memcpy(&len, payload + 4, 4);

    File *file = (File *)handle;
    // Cap len at buffer size
    if (len > 200)
      len = 200; // Small buffer for packet

    uint16_t read = storage_file_read(file, response_payload, len);
    response_header->command = FVP_CMD_FS_DATA;
    response_header->payload_len = read;
    break;
  }
  case FVP_CMD_FS_CLOSE: {
    uint32_t handle;
    memcpy(&handle, payload, 4);
    File *file = (File *)handle;
    storage_file_close(file);
    storage_file_free(file);
    response_header->command = FVP_CMD_FS_OK;
    break;
  }
  }
}
