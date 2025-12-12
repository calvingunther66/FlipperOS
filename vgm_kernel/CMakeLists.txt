cmake_minimum_required(VERSION 3.13)

# Initialize Pico SDK
include(pico_sdk_import.cmake)

project(vgm_kernel C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# --- Kernel Target ---
add_executable(vgm_kernel
    src/main.cpp
    # Transport will be added later
    # src/fvp_transport.cpp
)

# Include shared headers
target_include_directories(vgm_kernel PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../shared
)

# Enable USB Host (TinyUSB)
# Note: We need to configure pico_enable_stdio_usb or similar if we want debug output
# For FlipperOS we might want stdio over UART
pico_enable_stdio_uart(vgm_kernel 1)
pico_enable_stdio_usb(vgm_kernel 0)

# Link libraries
target_link_libraries(vgm_kernel 
    pico_stdlib
    tinyusb_host
    tinyusb_board
)

# Generate map file
pico_add_extra_outputs(vgm_kernel)
