//
// Created by steven on 7/4/2019.
//

#include <cstdint>
#include <iostream>
#include "Types.h"
#include "CP210xRuntimeDLL.h"

const static uint16_t NRST = CP210x_GPIO_2;
const static uint16_t BOOT0 = CP210x_GPIO_3;

void printDevInfo(HANDLE *device) {
    std::cout << "PRODUCT: ";
    CP210x_PRODUCT_STRING product[CP210x_MAX_PRODUCT_STRLEN];
    BYTE len = 0;
    auto ret = CP210xRT_GetDeviceProductString(*device, &product, &len, TRUE);
    if (ret != CP210x_SUCCESS) {
        std::cout << "ERROR READING Product String: " << ret << std::endl;
    }
    fwrite(product, sizeof(char), len, stdout);
    std::cout << std::endl;

    std::cout << "SERIAL: ";
    CP210x_SERIAL_STRING serial[CP210x_MAX_SERIAL_STRLEN];
    ret = CP210xRT_GetDeviceSerialNumber(*device, &serial, &len, TRUE);
    if (ret != CP210x_SUCCESS) {
        std::cout << "ERROR READING Product String: " << ret << std::endl;
    }
    fwrite(serial, sizeof(char), len, stdout);
    std::cout << std::endl;
}

void toggleBootFinish(HANDLE *device) {
    // Make BOOT0 Low, releasing bootloader mode
    CP210xRT_WriteLatch(*device, BOOT0, 0);
    Sleep(100);
}


int setGPIO(HANDLE *device, uint16_t gpio, BOOL level) {
    if (level)
        return CP210xRT_WriteLatch(*device, gpio, gpio);
    else
        return CP210xRT_WriteLatch(*device, gpio, 0);
}


void toggleBootStart(HANDLE *device) {

    // Make all pins low, enabling reset
    auto ret = CP210xRT_WriteLatch(*device, 0xffff, 0);
    Sleep(100);

    // Pull BOOT0 high, signaling bootloader
    ret = CP210xRT_WriteLatch(*device, BOOT0, BOOT0);
    if (ret != CP210x_SUCCESS) {
        std::cout << "ERROR Writing LATCH: " << ret << std::endl;
    }
    Sleep(100);

    // Release reset starting device in bootloader mode
    ret = CP210xRT_WriteLatch(*device, NRST, NRST);
    if (ret != CP210x_SUCCESS) {
        std::cout << "ERROR Writing LATCH: " << ret << std::endl;
    }
    Sleep(100);
}
