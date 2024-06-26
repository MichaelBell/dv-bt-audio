cmake_minimum_required(VERSION 3.12)

set(NAME dvplay)

include(pimoroni_pico_import.cmake)
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico_w)

pico_sdk_init()

include(picovision_import.cmake)
include(bluetooth/bluetooth.cmake)

add_executable(${NAME}
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/a2dp_sink.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/btstack_audio_pico.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/bt_display.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/fixed_fft.cpp
)

include(common/pimoroni_i2c)
include(libraries/pico_graphics/pico_graphics)
include(libraries/jpegdec/jpegdec)

target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${NAME}
    picow_bt_example_common
    pico_audio_i2s
    pico_multicore
    pimoroni_i2c
    pico_graphics
    picovision
    jpegdec
)

target_compile_definitions(${NAME} PRIVATE
    PICO_AUDIO_I2S_DATA_PIN=26
    PICO_AUDIO_I2S_CLOCK_PIN_BASE=27
    BLUETOOTH_DEVICE_NAME="Pico DV"
    PICO_MAX_SHARED_IRQ_HANDLERS=8
)

pico_enable_stdio_usb(${NAME} 1)
pico_add_extra_outputs(${NAME})

# Set up files for the release packages
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.uf2
    ${CMAKE_CURRENT_LIST_DIR}/README.md
    DESTINATION .
)

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_GENERATOR "ZIP" "TGZ")
include(CPack)
