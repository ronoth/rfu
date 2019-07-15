cmake_minimum_required(VERSION 3.13)
project(dfu)

set(CMAKE_CXX_STANDARD 11)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include/ stm32flash/)
find_package(Qt5Widgets REQUIRED)

add_executable(dfu
        src/serial-gpio-platform.cpp
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
        main.cpp
        include/sleepms.h)

configure_file(lib/CP210xRuntime.dll
        ${CMAKE_CURRENT_BINARY_DIR}/CP210xRuntime.dll COPYONLY)

target_link_libraries(dfu ${CMAKE_CURRENT_SOURCE_DIR}/lib/CP210xRuntime.lib)



add_executable(dfu-gui gui.cpp)

target_link_libraries(dfu-gui Qt5::Widgets)