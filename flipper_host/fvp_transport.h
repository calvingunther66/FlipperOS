#pragma once
#include "fvp_protocol.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void fvp_host_init();
void fvp_host_send_input(FvpInputEvent *event);
// Returns true if a frame was updated
bool fvp_host_receive(uint8_t *video_buffer, size_t video_buffer_len);

#ifdef __cplusplus
}
#endif
