/*
  Copyright (C) 2019 Steven Osborn <steven@lolsborn.com>

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
#include <termios.h>

#include "Types.h"
#include "CP210xRuntimeDLL.h"

#include "cxxopts.hpp"
#include "serial-gpio.h"
#include "stm32flash-lib.h"
#include "port.h"
#include "stm32.h"
#include "parsers/parser.h"

#include "parsers/binary.h"
#include "parsers/hex.h"

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
        if (opts.count("device") != 1) {
            std::cout << "Must specify serial device" << std::endl;
            std::cout << options.help();
            exit(1);
        }

        auto device = opts["device"].as<std::string>();

        std::string file = "";
        if(opts.count("file") == 1) {
            file = opts["file"].as<std::string>();
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
        writeFlash(stmout, port, file);
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