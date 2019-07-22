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
#include <stdio.h>
#include <serial-gpio.h>
#include "sleepms.h"
#include "libusb-1.0/libusb.h"


// Functions borrowed from libv/flashrom
// Copyright (C) 2018 Linaro Limited
static int cp210x_gpio_get(libusb_device_handle *handle)
{
    int res;
    uint8_t gpio;

    res = libusb_control_transfer(handle, REQTYPE_DEVICE_TO_HOST,
                                  CP210X_VENDOR_SPECIFIC, CP210X_READ_LATCH,
                                  0, &gpio, 1, 0);
    if (res < 0) {
        printf("Failed to read GPIO pins (%s)\n", libusb_error_name(res));
        return 0;
    }

    return gpio;
}

static void cp210x_gpio_set(uint8_t val, uint8_t mask, libusb_device_handle *handle)
{
    int res;
    uint16_t gpio;

    gpio = ((val & 0xf) << 8) | (mask & 0xf);

    /* Set relay state on the card */
    res = libusb_control_transfer(handle, REQTYPE_HOST_TO_DEVICE,
                                  CP210X_VENDOR_SPECIFIC, CP210X_WRITE_LATCH,
                                  gpio, NULL, 0, 0);
    if (res < 0)
        printf("Failed to set GPIO pins (%s)\n", libusb_error_name(res));
}


void printDevInfo(serial* device) {
}


void toggleBootFinish(serial *device) {
    sleep_ms(100);
}

void toggleBootStart(serial *device) {
    // Initialize libusb

    struct libusb_context *usb_ctx;
    libusb_device_handle *cp210x_handle;
    libusb_device *dev;

    int r = libusb_init(&usb_ctx); //initialize the library for the session we just declared
    libusb_set_option(usb_ctx, LIBUSB_OPTION_LOG_LEVEL, 3);
    if(r < 0) {
        printf("LibUSB: error with init\n");
    }
    printf("opening 0x%04x:0x%04x\n", LOSTIK2_VID, LOSTIK2_PID);

    cp210x_handle  = libusb_open_device_with_vid_pid(usb_ctx, LOSTIK2_VID, LOSTIK2_PID);


    //libusb_open_fd(dev, device->fd, &cp210x_handle);

    cp210x_gpio_set(0xff, 0x00ff, cp210x_handle);
    sleep_ms(100);
    cp210x_gpio_set(BOOT0, BOOT0, cp210x_handle);
    sleep_ms(100);
    cp210x_gpio_set(BOOT0, NRST, cp210x_handle);
    sleep_ms(100);
}
