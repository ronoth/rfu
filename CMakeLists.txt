cmake_minimum_required(VERSION 3.13)
project(dfu)

set(CMAKE_CXX_STANDARD 11)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


include(ExternalProject)

ExternalProject_Add(libusb
    GIT_REPOSITORY      https://github.com/ronoth/libusb
    GIT_TAG             origin/v1.0.22-fd
    PATCH_COMMAND       "${CMAKE_BINARY_DIR}/libusb-src/bootstrap.sh"
    SOURCE_DIR          "${CMAKE_BINARY_DIR}/libusb-src"
    CONFIGURE_COMMAND   ${CMAKE_BINARY_DIR}/libusb-src/configure --disable-udev --prefix=${CMAKE_BINARY_DIR}/external
    BUILD_COMMAND       make
    BUILD_IN_SOURCE     1
)


LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

include_directories(include/ stm32flash/ ${CMAKE_BINARY_DIR}/external/include)
link_directories(${CMAKE_BINARY_DIR}/external/lib)

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


if (WIN32)
    configure_file(lib/CP210xRuntime.dll
        ${CMAKE_CURRENT_BINARY_DIR}/CP210xRuntime.dll COPYONLY)
    target_link_libraries(dfu ${CMAKE_CURRENT_SOURCE_DIR}/lib/CP210xRuntime.lib)
else()
    target_link_libraries(dfu ${CMAKE_BINARY_DIR}/external/lib/libusb-1.0.a pthread)
    add_dependencies(dfu libusb)
endif (WIN32)

add_executable(dfu-gui gui.cpp)
target_link_libraries(dfu-gui Qt5::Widgets)