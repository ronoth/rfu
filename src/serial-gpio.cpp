//
// Created by steven on 7/4/2019.
//

#include <cstdint>
#include <iostream>
#include <stm32.h>
#include "port.h"
#include "Types.h"
#include "CP210xRuntimeDLL.h"


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

void eraseFlash(stm32_t *stm) {
    printf("Erasing Flash\n");
    stm32_readprot_memory(stm);
    Sleep(100);
    stm = stm32_init(stm->port, 1);
    stm32_runprot_memory(stm);
    Sleep(100);
}

void printMore(stm32_t *stm, port_interface *port) {
    printf("Interface %s: %s\n", port->name, port->get_cfg_str(port));

    printf("Version      : 0x%02x\n", stm->bl_version);
    if (port->flags & PORT_GVR_ETX) {
        printf("Option 1     : 0x%02x\n", stm->option1);
        printf("Option 2     : 0x%02x\n", stm->option2);
    }
    printf("Device ID    : 0x%04x (%s)\n", stm->pid, stm->dev->name);
    printf("- RAM        : %dKiB  (%db reserved by bootloader)\n", (stm->dev->ram_end - 0x20000000) / 1024, stm->dev->ram_start - 0x20000000);
    printf("- Flash      : %dKiB (size first sector: %dx%d)\n", (stm->dev->fl_end - stm->dev->fl_start ) / 1024, stm->dev->fl_pps, stm->dev->fl_ps[0]);
    printf("- Option RAM : %db\n", stm->dev->opt_end - stm->dev->opt_start + 1);
    printf("- System RAM : %dKiB\n", (stm->dev->mem_end - stm->dev->mem_start) / 1024);

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
