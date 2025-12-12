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

  // 2. Wait for Response
  int timeout = 100000;
  while (timeout-- > 0) {
    FvpHeader resp_header;
    uint8_t resp_payload[256];
    if (transport.receive_packet(&resp_header, resp_payload,
                                 sizeof(resp_payload))) {
      if (resp_header.command == CMD_FILE_OK && resp_header.payload_len == 4) {
        uint32_t handle;
        memcpy(&handle, resp_payload, 4);
        return (FvpFileHandle)handle;
      } else if (resp_header.command == CMD_FILE_ERROR) {
        return 0;
      }
    }
  }
  return 0; // Timeout
}

size_t fvp_fread(void *ptr, size_t size, size_t count, FvpFileHandle stream) {
  FvpTransport &transport = FvpTransport::get_instance();
  size_t total_bytes = size * count;
  size_t bytes_read = 0;
  uint8_t *out_ptr = (uint8_t *)ptr;

  // We can only read small chunks due to packet size limits (256 bytes payload)
  // Let's read in 200 byte chunks
  const size_t CHUNK_SIZE = 200;

  while (bytes_read < total_bytes) {
    size_t to_read = total_bytes - bytes_read;
    if (to_read > CHUNK_SIZE)
      to_read = CHUNK_SIZE;

    // Send CMD_FILE_READ
    // Payload: [Handle (4)] [Len (4)]
    uint8_t payload[8];
    uint32_t handle_le = (uint32_t)stream;
    uint32_t len_le = (uint32_t)to_read;
    memcpy(payload, &handle_le, 4);
    memcpy(payload + 4, &len_le, 4);

    FvpHeader req_header;
    req_header.magic = FVP_MAGIC;
    req_header.version = FVP_VERSION;
    req_header.command = CMD_FILE_READ;
    req_header.payload_len = 8;

    transport.send_packet(req_header, payload);

    // Wait for CMD_FILE_DATA or ERROR
    // Busy wait with timeout
    bool success = false;
    int timeout = 100000; // Loops
    while (timeout-- > 0) {
      FvpHeader resp_header;
      uint8_t resp_payload[256];
      if (transport.receive_packet(&resp_header, resp_payload,
                                   sizeof(resp_payload))) {
        if (resp_header.command == CMD_FILE_DATA) {
          // Got data
          memcpy(out_ptr + bytes_read, resp_payload, resp_header.payload_len);
          bytes_read += resp_header.payload_len;
          success = true;
          break;
        } else if (resp_header.command == CMD_FILE_ERROR) {
          return bytes_read / size; // Stop and return what we got
        }
      }
    }

    if (!success) {
      // Timeout
      break;
    }
  }

  return bytes_read / size;
}

size_t fvp_fwrite(const void *ptr, size_t size, size_t count,
                  FvpFileHandle stream) {
  FvpTransport &transport = FvpTransport::get_instance();
  size_t total_bytes = size * count;
  size_t bytes_written = 0;
  const uint8_t *in_ptr = (const uint8_t *)ptr;

  const size_t CHUNK_SIZE = 200; // Payload limit

  while (bytes_written < total_bytes) {
    size_t to_write = total_bytes - bytes_written;
    if (to_write > CHUNK_SIZE)
      to_write = CHUNK_SIZE;

    // Send CMD_FILE_WRITE
    // Payload: [Handle (4)] [Len (4)] [Data (N)]
    // Total payload overhead = 8 bytes

    uint8_t payload[256];
    uint32_t handle_le = (uint32_t)stream;
    uint32_t len_le = (uint32_t)to_write;
    memcpy(payload, &handle_le, 4);
    memcpy(payload + 4, &len_le, 4);
    memcpy(payload + 8, in_ptr + bytes_written, to_write);

    FvpHeader req_header;
    req_header.magic = FVP_MAGIC;
    req_header.version = FVP_VERSION;
    req_header.command = CMD_FILE_WRITE;
    req_header.payload_len = 8 + to_write;

    transport.send_packet(req_header, payload);

    // Wait for OK
    bool success = false;
    int timeout = 100000;
    while (timeout-- > 0) {
      FvpHeader resp_header;
      uint8_t resp_payload[256];
      if (transport.receive_packet(&resp_header, resp_payload,
                                   sizeof(resp_payload))) {
        if (resp_header.command == CMD_FILE_OK) {
          // Confirmed
          bytes_written += to_write;
          success = true;
          break;
        } else if (resp_header.command == CMD_FILE_ERROR) {
          return bytes_written / size;
        }
      }
    }
    if (!success)
      break;
  }
  return bytes_written / size;
}

int fvp_fclose(FvpFileHandle stream) {
  FvpTransport &transport = FvpTransport::get_instance();
  uint32_t handle_le = (uint32_t)stream;

  FvpHeader req_header;
  req_header.magic = FVP_MAGIC;
  req_header.version = FVP_VERSION;
  req_header.command = CMD_FILE_CLOSE;
  req_header.payload_len = 4;

  transport.send_packet(req_header, (uint8_t *)&handle_le);

  // Check OK if strictly needed, or just assumption
  return 0;
}
