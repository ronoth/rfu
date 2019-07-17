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
#include <sys/ioctl.h>
#include "port.h"
#include "Types.h"
#include "CP210xRuntimeDLL.h"


void printDevInfo(serial* device) {
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
    //CP210xRT_WriteLatch(*device, BOOT0, 0);
    sleep_ms(100);
}


void toggleBootStart(serial *device) {

    // Make all pins low, enabling reset

    int bit, lines;


    // Make all pins low, enabling reset
    lines = 0x00ff;
    if (ioctl(device->fd, TIOCMSET, &lines)) {
        fprintf(stderr, "Error toggling boot pin clear\n");
    }
    sleep_ms(100);

    // Pull BOOT0 high, signaling bootloader
    lines = (0xff << 8) + BOOT0;
    if (ioctl(device->fd, TIOCMSET, &lines)) {
        fprintf(stderr, "Error toggling boot pin BOOT0\n");
    }
    sleep_ms(100);

    // Release reset starting device in bootloader mode
    lines = (0xff << 8) + NRST;
    if (ioctl(device->fd, TIOCMSET, &lines)) {
        fprintf(stderr, "Error toggling boot NRST\n");
    }
    sleep_ms(100);
}
