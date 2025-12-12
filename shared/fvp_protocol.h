#pragma once
#include <stdint.h>
#include <stdbool.h>

// FVP - Flipper Video Protocol Definitions

#define FVP_MAGIC 0xFE
#define FVP_VERSION 1

// Command Codes
enum FvpCommand {
    FVP_CMD_PING = 0x01,
    FVP_CMD_PONG = 0x02,
    
    // Video Stream
    FVP_CMD_FRAME_DATA = 0x10, // Payload: Raw pixel data
    
    // Input Stream
    FVP_CMD_INPUT_EVENT = 0x20, // Payload: FvpInputEvent
    
    // Storage Stream
    FVP_CMD_FS_OPEN  = 0x30, // Open file
    FVP_CMD_FS_READ  = 0x31, // Read file
    FVP_CMD_FS_WRITE = 0x32, // Write file
    FVP_CMD_FS_CLOSE = 0x33, // Close file
    FVP_CMD_FS_DIR   = 0x34, // List directory
    
    // Storage Responses
    FVP_CMD_FS_OK    = 0x40,
    FVP_CMD_FS_DATA  = 0x41,
    FVP_CMD_FS_ERROR = 0x42,
};

// Input Types
enum FvpInputType {
    FVP_INPUT_BUTTON = 0,
    FVP_INPUT_MOUSE  = 1,
    FVP_INPUT_KEY    = 2,
};

// Structures (Packed to ensure consistent alignment)
#pragma pack(push, 1)

typedef struct {
    uint8_t magic;
    uint8_t version;
    uint8_t command;
    uint16_t payload_len;
    // Payload follows
} FvpHeader;

typedef struct {
    uint8_t type; // FvpInputType
    uint16_t data; // Keycode, or button mask
    int8_t x;     // Mouse dx
    int8_t y;     // Mouse dy
} FvpInputEvent;

// File System Request Structures
typedef struct {
    uint8_t flags; // Read/Write/Append
    char path[128];
} FvpFsOpenReq;

#pragma pack(pop)
