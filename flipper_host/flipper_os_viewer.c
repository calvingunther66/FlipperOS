#include "fvp_protocol.h"
#include "fvp_transport.h"
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>

// 128x64 / 8 bits per byte = 1024 bytes
#define FRAME_BUFFER_SIZE (128 * 64 / 8)

// Define the application state
typedef struct {
  uint8_t frame_buffer[FRAME_BUFFER_SIZE];
  FuriMutex *mutex;
} FlipperOSApp;

// Screen draw callback
static void app_draw_callback(Canvas *canvas, void *ctx) {
  FlipperOSApp *app = ctx;

  // Lock mutex to prevent tearing if writing happens during draw
  if (furi_mutex_acquire(app->mutex, 0) == FuriStatusOk) {
    canvas_draw_xbm(canvas, 0, 0, 128, 64, app->frame_buffer);
    furi_mutex_release(app->mutex);
  }
}

// Input callback
static void app_input_callback(InputEvent *input_event, void *ctx) {
  FuriMessageQueue *event_queue = ctx;
  furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t flipper_os_viewer_app(void *p) {
  UNUSED(p);

  FlipperOSApp *app = malloc(sizeof(FlipperOSApp));
  memset(app->frame_buffer, 0, FRAME_BUFFER_SIZE);
  app->mutex = furi_mutex_alloc(FuriMutexTypeNormal);

  // Initial placeholder pattern
  app->frame_buffer[0] = 0xFF;

  // Initialize FVP
  fvp_host_init();

  FuriMessageQueue *event_queue =
      furi_message_queue_alloc(8, sizeof(InputEvent));

  // ViewPort setup
  ViewPort *view_port = view_port_alloc();
  view_port_draw_callback_set(view_port, app_draw_callback, app);
  view_port_input_callback_set(view_port, app_input_callback, event_queue);

  // GUI setup
  Gui *gui = furi_record_open(RECORD_GUI);
  gui_add_view_port(gui, view_port, GuiLayerFullscreen);

  InputEvent event;
  bool running = true;

  // Main loop
  while (running) {
    // Poll input queue
    if (furi_message_queue_get(event_queue, &event, 0) == FuriStatusOk) {
      if (event.type == InputTypePress && event.key == InputKeyBack) {
        // Long press back to exit? Or just exit on single click for now
        // For a viewer, generally long back is exit
        // But let's stick to simple
        running = false;
      }

      // Forward Input to VGM
      FvpInputEvent fvp_input;
      fvp_input.type = FVP_INPUT_BUTTON;
      fvp_input.data = 0; // TODO: Map key to bitmask or code

      // Simple mapping for now
      // In reality we'd map Up/Down/Left/Right/Ok/Back
      fvp_input.data = (event.key << 8) | event.type;

      fvp_host_send_input(&fvp_input);
    }

    // Poll for video frame
    // In real app, transport might have its own thread or callback
    // Here we poll
    if (furi_mutex_acquire(app->mutex, 100) == FuriStatusOk) {
      if (fvp_host_receive(app->frame_buffer, FRAME_BUFFER_SIZE)) {
        view_port_update(view_port);
      }
      furi_mutex_release(app->mutex);
    }

    // Yield a bit if no message
    // furi_delay_ms(1);
  }

  // Cleanup
  view_port_enabled_set(view_port, false);
  gui_remove_view_port(gui, view_port);
  view_port_free(view_port);
  furi_record_close(RECORD_GUI);
  furi_message_queue_free(event_queue);

  furi_mutex_free(app->mutex);
  free(app);

  return 0;
}
