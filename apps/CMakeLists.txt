cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(flipper_apps C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# Shared settings
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${CMAKE_CURRENT_SOURCE_DIR}/app.ld -nostartfiles")

# --- Shell App ---
add_executable(shell_app shell.cpp)
# Link strict to avoid bloat
target_link_options(shell_app PRIVATE -Wl,--gc-sections)
# Rename output
set_target_properties(shell_app PROPERTIES OUTPUT_NAME "shell.fos")

# --- Word App ---
add_executable(word_app word.cpp)
target_link_options(word_app PRIVATE -Wl,--gc-sections)
set_target_properties(word_app PROPERTIES OUTPUT_NAME "word.fos")

# Common includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
