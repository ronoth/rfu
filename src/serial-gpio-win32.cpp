/*
  Copyright (C) 2019 Ronoth, LLC <steven@ronoth.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <iostream>
#include <sleepms.h>
#include <serial-gpio.h>

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
