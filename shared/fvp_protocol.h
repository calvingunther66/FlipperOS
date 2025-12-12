#pragma once
#include <stdbool.h>
#include <stdint.h>

// FVP - Flipper Video Protocol Definitions

#define FVP_MAGIC 0xFE
#define FVP_VERSION 2

// Command Codes (Strict Match)
enum FvpCommand {
  CMD_PING = 0x01,
  CMD_PONG = 0x02,

  // CMD_VIDEO_FRAME: Chunk of pixel data from VGM -> Flipper.
  CMD_VIDEO_FRAME = 0x10,

  // CMD_INPUT_EVENT: Button press from Flipper -> VGM.
  CMD_INPUT_EVENT = 0x20,

  // CMD_FILE_*: RPC commands from VGM -> Flipper
  CMD_FILE_OPEN = 0x30,
  CMD_FILE_READ = 0x31,
  CMD_FILE_WRITE = 0x32,
  CMD_FILE_CLOSE = 0x33, // Added for completeness, though not explicitly
                         // requested in list it's implied by "RPC commands"
  CMD_FILE_DIR = 0x34,

  // Storage Responses
  CMD_FILE_OK = 0x40,
  CMD_FILE_DATA = 0x41,
  CMD_FILE_ERROR = 0x42,

  // CMD_USB_HID: Mouse/Keyboard data (internal OS usage)
  CMD_USB_HID = 0x50,
};

// Structures
#pragma pack(push, 1)

typedef struct {
  uint8_t magic;
  uint8_t version;
  uint8_t command;
  uint16_t payload_len;
  // Payload follows
} FvpHeader;

typedef struct {
  uint8_t type;  // 0=Button, 1=Mouse, 2=Key
  uint16_t data; // Keycode or button mask
  int8_t x;      // Mouse dx
  int8_t y;      // Mouse dy
} FvpInputEvent;

typedef struct {
  uint8_t flags; // Read/Write
  char path[128];
} FvpFileOpenReq;

#pragma pack(pop)
