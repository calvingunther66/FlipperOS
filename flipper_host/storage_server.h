#pragma once
#include "fvp_protocol.h"
#include <furi.h>
#include <storage/storage.h>

void storage_server_init();
void storage_server_handle_command(FvpHeader *header, uint8_t *payload,
                                   FvpHeader *response_header,
                                   uint8_t *response_payload);
