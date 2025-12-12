#include "fvp_transport.h"
#include "../shared/fvp_protocol.h"
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_serial.h> // Required for Serial definitions

static FuriHalSerialHandle *g_serial_handle = NULL;

// Circular buffer for RX (simplified for singlethreaded poll in this client)
// In a real app we might use a stream buffer or queue
#define RX_BUF_SIZE 4096
static uint8_t g_rx_buffer[RX_BUF_SIZE];
static volatile size_t g_rx_head = 0;
static volatile size_t g_rx_tail = 0;

static void serial_rx_callback(FuriHalSerialHandle *handle,
                               FuriHalSerialRxEvent event, void *context) {
  if (event == FuriHalSerialRxEventData) {
    uint8_t data = furi_hal_serial_read(handle);
    size_t next = (g_rx_head + 1) % RX_BUF_SIZE;
    if (next != g_rx_tail) {
      g_rx_buffer[g_rx_head] = data;
      g_rx_head = next;
    }
  }
}

void fvp_host_init() {
  if (!g_serial_handle) {
    g_serial_handle = furi_hal_serial_control_acquire(FuriHalSerialIdUsart);
    if (g_serial_handle) {
      furi_hal_serial_init(g_serial_handle, 921600);
      furi_hal_serial_async_rx_start(g_serial_handle, serial_rx_callback, NULL,
                                     false);
    }
  }
}

void fvp_host_send_input(FvpInputEvent *event) {
  if (!g_serial_handle)
    return;

  // Construct Input Packet
  FvpHeader header;
  header.magic = FVP_MAGIC;
  header.version = FVP_VERSION;
  header.command = CMD_INPUT_EVENT;
  header.payload_len = sizeof(FvpInputEvent);

  furi_hal_serial_tx(g_serial_handle, (uint8_t *)&header, sizeof(header));
  furi_hal_serial_tx(g_serial_handle, (uint8_t *)event, sizeof(FvpInputEvent));
}

// Helper to read byte from ring buffer
static bool read_byte(uint8_t *out) {
  if (g_rx_head == g_rx_tail)
    return false;
  *out = g_rx_buffer[g_rx_tail];
  g_rx_tail = (g_rx_tail + 1) % RX_BUF_SIZE;
  return true;
}

static bool read_bytes_blocking(uint8_t *out, size_t len) {
  size_t read = 0;
  while (read < len) {
    if (read_byte(&out[read])) {
      read++;
    } else {
      // In a real RTOS app we should yield/wait_event here
      // furi_delay_tick(1);
      // For now simple busy-wait (not ideal but safe-ish if data is flowing)
      // Or better, just return false if not enough data?
      // "Blocking" implies we wait.
      furi_thread_yield();
    }
  }
  return true;
}

// Returns TRUE if a full video frame was decoded into video_buffer
bool fvp_host_receive(uint8_t *video_buffer, size_t video_buffer_len) {
  // Check for header
  // This is a simplified state machine. In production, maintain state across
  // calls. Here we peek/scan since we might be called in a loop.

  // Check if enough data for header at least
  // This logic relies on `read_byte` removing data.
  // Risky change: we need to find MAGIC.

  while (g_rx_head != g_rx_tail) {
    // Peek or just read statefully? Let's try to sync.
    uint8_t byte;
    if (!read_byte(&byte))
      break;

    // Very lazy sync: just look for MAGIC byte 0xAA (assuming from header)
    if (byte == (uint8_t)(FVP_MAGIC & 0xFF)) {
      // Possible header start. Read rest of header!
      // Note: FVP_MAGIC is 0xA55A, little endian -> 5A A5.
      // Actually let's assume byte alignment for now.

      // Wait, if we stripped the first byte, we need to read
      // sizeof(FvpHeader)-1
      uint8_t h_buf[sizeof(FvpHeader)];
      h_buf[0] = byte;
      if (read_bytes_blocking(&h_buf[1], sizeof(FvpHeader) - 1)) {
        FvpHeader *h = (FvpHeader *)h_buf;
        if (h->magic == FVP_MAGIC) {
          // Got valid header
          if (h->command == CMD_VIDEO_FRAME &&
              h->payload_len <= video_buffer_len) {
            read_bytes_blocking(video_buffer, h->payload_len);
            return true;
          }
          // Else skip payload if any
          if (h->payload_len > 0) {
            uint8_t dummy;
            for (int i = 0; i < h->payload_len; i++)
              read_bytes_blocking(&dummy, 1);
          }
        }
      }
    }
  }

  return false;
}
