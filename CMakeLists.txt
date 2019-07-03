cmake_minimum_required(VERSION 3.13)
project(dfu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt5Widgets REQUIRED)

add_executable(dfu main.cpp)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/CP210xRuntime.dll
        ${CMAKE_CURRENT_BINARY_DIR}/CP210xRuntime.dll COPYONLY)
target_link_libraries(dfu ${CMAKE_CURRENT_SOURCE_DIR}/CP210xRuntime.lib)


add_executable(dfu-siusb siusb-main.cpp)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/SiUSBXp.dll
        ${CMAKE_CURRENT_BINARY_DIR}/SiUSBXp.dll COPYONLY)
target_link_libraries(dfu-siusb ${CMAKE_CURRENT_SOURCE_DIR}/SiUSBXp.lib)

add_executable(dfu-gui gui.cpp)

target_link_libraries(dfu-gui Qt5::Widgets)