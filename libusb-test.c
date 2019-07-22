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
#include <assert.h>
#include <libusb.h>
#include <stdio.h>

#include "sleepms.h"

#define PID 0xea60
#define VID 0x10c4

#define REQTYPE_HOST_TO_DEVICE  0x40
#define REQTYPE_DEVICE_TO_HOST 0xc0
#define CP210X_WRITE_LATCH 0x37E1
#define CP210X_READ_LATCH 0x00c2
#define CP210X_VENDOR_SPECIFIC 0xff

#define     NRST                        0x0004
#define     BOOT0                       0x0008


struct libusb_context *usb_ctx;
static libusb_device_handle *cp210x_handle;

// Functions borrowed from libv/flashrom
// Copyright (C) 2018 Linaro Limited
static int cp210x_gpio_get(void)
{
    int res;
    uint8_t gpio;

    res = libusb_control_transfer(cp210x_handle, REQTYPE_DEVICE_TO_HOST,
                                  CP210X_VENDOR_SPECIFIC, CP210X_READ_LATCH,
                                  0, &gpio, 1, 0);
    if (res < 0) {
        printf("Failed to read GPIO pins (%s)\n", libusb_error_name(res));
        return 0;
    }

    return gpio;
}

static void cp210x_gpio_set(uint8_t val, uint8_t mask)
{
    int res;
    uint16_t gpio;

    gpio = ((val & 0xf) << 8) | (mask & 0xf);

    /* Set relay state on the card */
    res = libusb_control_transfer(cp210x_handle, REQTYPE_HOST_TO_DEVICE,
                                  CP210X_VENDOR_SPECIFIC, CP210X_WRITE_LATCH,
                                  gpio, NULL, 0, 0);
    if (res < 0)
        printf("Failed to read GPIO pins (%s)\n", libusb_error_name(res));
}

int main(int argc, char** argv) {


    int r = libusb_init(&usb_ctx); //initialize the library for the session we just declared
    libusb_set_option(usb_ctx, LIBUSB_OPTION_LOG_LEVEL, 3);

    // discover devices
    libusb_device **list;
    ssize_t cnt = libusb_get_device_list(NULL, &list);
    ssize_t i = 0;
    unsigned char serial[256];
    int err = 0;
    if (cnt < 0)
        printf("ERROR HERE\n");
    printf("iterating %zd devices\n", cnt);
    for (i = 0; i < cnt; i++) {
        libusb_device *device = list[i];
        struct libusb_device_descriptor desc;
        libusb_get_device_descriptor(device, &desc);
        printf("pid %d, vid %d\n", desc.idProduct, desc.idVendor);
        uint8_t serialidx = desc.iSerialNumber;
        r = libusb_open(device, &cp210x_handle);
        if (r) {
            printf("Error failed to open dev\n");
        }
        r = libusb_get_string_descriptor_ascii(cp210x_handle, desc.iSerialNumber, serial, sizeof(serial));
        printf("Serial: %s", serial);
        if(r == LIBUSB_ERROR_IO) {
            printf("Error fetching serial descriptor\n");
            return 1;
        }
        if (cp210x_handle)
            libusb_close(cp210x_handle);
    }
    libusb_free_device_list(list, 1);

//    printf("opening 0x%04x:0x%04x\n", VID, PID);
//    cp210x_handle  = libusb_open_device_with_vid_pid(usb_ctx, VID, PID);


//    if(cp210x_handle == NULL) {
//        printf("device not found\n");
//        return 1;
//    }
    printf("device open\n");

    r = cp210x_gpio_get();
    printf("gpio 0x%04x\n", r);

    cp210x_gpio_set(0xff, 0x00ff);
    sleep_ms(100);
    cp210x_gpio_set(BOOT0, BOOT0);
    sleep_ms(100);
    cp210x_gpio_set(BOOT0, NRST);
    sleep_ms(100);

    r = cp210x_gpio_get();
    printf("gpio 0x%04x\n", r);

    libusb_close(cp210x_handle);
}