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
#include <algorithm>
#include <iostream>
#include <bitset>
#include <iomanip>
#include <sys/types.h>


#if defined(__WIN32__) || defined(__CYGWIN__)
#include "Types.h"
#include "CP210xRuntimeDLL.h"
#else
#include <termios.h>
#endif

#include "cxxopts.hpp"
#include "serial-gpio.h"
#include "stm32flash-lib.h"
#include "port.h"
#include "stm32.h"
#include "parsers/parser.h"
#include "parsers/binary.h"
#include "parsers/hex.h"

#include "libusb-1.0/libusb.h"

struct libusb_context *usb_ctx;
static libusb_device_handle *cp210x_handle;


int main(int argc, char** argv) {


    cxxopts::Options options("Ronoth DFU", "Device Firmware Upgrade for devices by Ronoth, LLC");
    options.add_options()
            ("d,device", "Device Path / COM Port", cxxopts::value<std::string>())
            ("f,file", "Firmware File Path", cxxopts::value<std::string>())
            ("h,help", "Display this message");

    try {
        auto opts = options.parse(argc, argv);


        if (opts.count("help")) {
            std::cout << options.help();
            exit(1);
        }

        std::string file;
        if(opts.count("file") == 1) {
            file = opts["file"].as<std::string>();
        }

        std::string device;

        if (opts.count("device") != 1) {
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

            // List all of the available devices.
            for (i = 0; i < cnt; i++) {
                libusb_device *device = list[i];
                struct libusb_device_descriptor desc;
                libusb_get_device_descriptor(device, &desc);
                if(desc.idProduct != LOSTIK2_PID && desc.idVendor != LOSTIK2_VID) {
                    continue;
                }

                uint8_t serialidx = desc.iSerialNumber;
                r = libusb_open(device, &cp210x_handle);
                if (r) {
                    continue;
                }
                r = libusb_get_string_descriptor_ascii(cp210x_handle, desc.iSerialNumber, serial, sizeof(serial));
                printf("Serial: %s\n", serial);
                if(r == LIBUSB_ERROR_IO) {
                    printf("Error fetching serial descriptor\n");
                    return 1;
                }
                if (cp210x_handle)
                    libusb_close(cp210x_handle);
            }

            libusb_free_device_list(list, 1);

         //   std::cout << "Must specify serial device" << std::endl;
//            std::cout << options.help();
//            exit(1);
        } else {
            // Use device path from command line
            device = opts["device"].as<std::string>();
        }



        stm32_t *stmout;
        struct port_options port_opts;
        struct port_interface *port = NULL;

        port_opts.device = device.c_str();
        port_opts.baudRate = SERIAL_BAUD_57600;
        port_opts.serial_mode = "8e1";
        port_opts.bus_addr = 0;
        port_opts.rx_frame_max = STM32_MAX_RX_FRAME;
        port_opts.tx_frame_max = STM32_MAX_TX_FRAME;

        if (port_open(&port_opts, &port) != PORT_ERR_OK) {
            fprintf(stderr, "Failed to open port: %s\n", port_opts.device);
            return PORT_INIT_FAILED;
        }


        serial *h = (serial *)port->_private;
        printDevInfo(h);
        toggleBootStart(h);


        char init_flag = 1;
        stmout = stm32_init(port, init_flag);
        if (!stmout) {
            printf("Unable to init STM32\n");
            return STM_INIT_FAILED;
        }


        printMore(stmout, port);
        eraseFlash(stmout);
        writeFlash(stmout, file);
        toggleBootFinish(h);
        jumpToStart(stmout);

        if (stmout) stm32_close(stmout);
        if (port)
            port->close(port);

    } catch(cxxopts::OptionException& exception) {
        std::cout << exception.what() << std::endl;
        std::cout << options.help();
        exit(1);
    }



    return 0;
}