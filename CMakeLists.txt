cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(Pico_USB_Keyboard_Sample C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

#set(FAMILY rp2040)
#set(BOARD pico_sdk)
#set(TINYUSB_FAMILY_PROJECT_NAME_PREFIX "tinyusb_dev_")
#add_subdirectory(${PICO_TINYUSB_PATH}/examples/device tinyusb_device_examples)

add_executable(Pico_USB_Keyboard_Sample)

target_sources(Pico_USB_Keyboard_Sample PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${CMAKE_CURRENT_LIST_DIR}/input.c
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
        )

# Make sure TinyUSB can find tusb_config.h
target_include_directories(Pico_USB_Keyboard_Sample PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(Pico_USB_Keyboard_Sample PUBLIC pico_stdlib pico_unique_id tinyusb_device tinyusb_board pico_multicore)

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
#target_compile_definitions(Pico_USB_Keyboard_Sample PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

pico_add_extra_outputs(Pico_USB_Keyboard_Sample)

# add url via pico_set_program_url
#example_auto_set_url(Pico_USB_Keyboard_Sample)
