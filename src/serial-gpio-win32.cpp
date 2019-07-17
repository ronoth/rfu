//
// Created by steven on 7/4/2019.
//

#include <cstdint>
#include <iostream>
#include <stm32.h>
#include <sys/types.h>
#include <parsers/parser.h>
#include <parsers/binary.h>
#include <sleepms.h>
#include <serial-gpio.h>
#include "port.h"

#if defined(__WIN32__) || defined(__CYGWIN__)
#include "Types.h"
#include "CP210xRuntimeDLL.h"
#endif


void printDevInfo(serial *device) {
    std::cout << "Product: ";
    CP210x_PRODUCT_STRING product[CP210x_MAX_PRODUCT_STRLEN];
    BYTE len = 0;
    auto ret = CP210xRT_GetDeviceProductString(device->fd, &product, &len, TRUE);
    if (ret != CP210x_SUCCESS) {
        std::cerr << "ERROR READING Product String: " << ret << std::endl;
    }
    fwrite(product, sizeof(char), len, stdout);
    std::cout << std::endl;

    std::cout << "Device Serial: ";
    CP210x_SERIAL_STRING serial[CP210x_MAX_SERIAL_STRLEN];
    ret = CP210xRT_GetDeviceSerialNumber(device->fd, &serial, &len, TRUE);
    if (ret != CP210x_SUCCESS) {
        std::cerr << "ERROR READING Product String: " << ret << std::endl;
    }
    fwrite(serial, sizeof(char), len, stdout);
    std::cout << std::endl;
}

void eraseFlash(stm32_t *stm) {
    printf("Erasing Flash\n");
    stm32_readprot_memory(stm);
    sleep_ms(100);
    stm = stm32_init(stm->port, 1);
    stm32_runprot_memory(stm);
    sleep_ms(100);
}

void jumpToStart(stm32_t *stm) {
    sleep_ms(100);
    uint32_t execute = stm->dev->fl_start;
    printf("\nStarting execution at address 0x%08x... ", execute);
    if (stm32_go(stm, execute) == STM32_ERR_OK)
        printf("done.\n");
    else
        printf("failed.\n");
}

void toggleBootFinish(serial *device) {
    // Make BOOT0 Low, releasing bootloader mode
    CP210xRT_WriteLatch(device->fd, BOOT0, 0);
    sleep_ms(100);
}

void toggleBootStart(serial *device) {

    // Make all pins low, enabling reset
    auto ret = CP210xRT_WriteLatch(device->fd, 0xffff, 0);
    if (ret != CP210x_SUCCESS) {
        fprintf(stderr, "ERROR Writing LATCH: %d\n", ret);
    }
    sleep_ms(100);

    // Pull BOOT0 high, signaling bootloader
    ret = CP210xRT_WriteLatch(device->fd, BOOT0, BOOT0);
    if (ret != CP210x_SUCCESS) {
        fprintf(stderr, "ERROR Writing LATCH: %d\n", ret);
    }
    sleep_ms(100);

    // Release reset starting device in bootloader mode
    ret = CP210xRT_WriteLatch(device->fd, NRST, NRST);
    if (ret != CP210x_SUCCESS) {
        fprintf(stderr, "ERROR Writing LATCH: %d\n", ret);
    }
    sleep_ms(100);
}
