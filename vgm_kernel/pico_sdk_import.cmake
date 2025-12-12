# This is a stub file for the AI agent context.
# In a real build environment, the PICO_SDK_PATH would be set or this file would be copied from the SDK.
if (DEFINED ENV{PICO_SDK_PATH} AND (NOT PICO_SDK_PATH))
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
    message("Using PICO_SDK_PATH from environment ('${PICO_SDK_PATH}')")
endif ()

if (NOT PICO_SDK_PATH)
    # Default path for many installs
    set(PICO_SDK_PATH "/usr/share/pico-sdk") 
    message("PICO_SDK_PATH not set, trying default: ${PICO_SDK_PATH}")
endif ()

if (NOT EXISTS ${PICO_SDK_PATH})
    message(WARNING "PICO_SDK_PATH not found at ${PICO_SDK_PATH}. Build may fail.")
else()
    include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake OPTIONAL)
endif()
