cmake_minimum_required(VERSION 3.13)
project(rfu)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
include_directories(include/ stm32flash/ ${CMAKE_BINARY_DIR}/external/include)
link_directories(${CMAKE_BINARY_DIR}/external/lib)

add_executable(dfu
        src/stm32flash-lib.c
        stm32flash/stm32.c
        stm32flash/port.c
        stm32flash/i2c.c
        stm32flash/utils.c
        stm32flash/dev_table.c
        stm32flash/serial_common.c
        stm32flash/serial_platform.c
        stm32flash/parsers/hex.c
        stm32flash/parsers/binary.c
        main.c)