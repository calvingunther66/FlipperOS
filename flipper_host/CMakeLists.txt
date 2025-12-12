cmake_minimum_required(VERSION 3.13)

# This is a standard template for Flipper Zero applications using ufbt
# or built as part of the firmware.

if (NOT DEFINED FLIPPER_APP_NAME)
    set(FLIPPER_APP_NAME "flipper_os_viewer")
endif()

project(${FLIPPER_APP_NAME} C)

add_flipper_app(${FLIPPER_APP_NAME}
    SOURCES
        flipper_os_viewer.c
        # transport layer to be added
        # fvp_transport.c
    
    REQUIRES
        gui
        storage
        dialogs
)

target_include_directories(${FLIPPER_APP_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../shared
)

# Define application metadata
flipper_app_define_manifest(${FLIPPER_APP_NAME}
    stack_size 2048
    icon "icon.png" # Placeholder
    order 10
)
