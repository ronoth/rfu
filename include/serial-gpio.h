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

#ifndef DFU_SERIAL_GPIO_H
#define DFU_SERIAL_GPIO_H


#include <stm32.h>

#ifdef WIN32
#include "CP210xRuntimeDLL.h"
#include "Types.h"

struct serial {
            HANDLE fd;
            DCB oldtio;
            DCB newtio;
            char setup_str[11];
        };
#else

#include <termios.h>

#define REQTYPE_HOST_TO_DEVICE  0x40
#define REQTYPE_DEVICE_TO_HOST 0xc0
#define CP210X_WRITE_LATCH 0x37E1
#define CP210X_READ_LATCH 0x00c2
#define CP210X_VENDOR_SPECIFIC 0xff

#define NRST    0x0004
#define BOOT0   0x0008


struct serial {
    int fd;
    struct termios oldtio;
    struct termios newtio;
    char setup_str[11];
};
#endif

void jumpToStart(stm32_t *stm);
void eraseFlash(stm32_t *stm);
void writeFlash(stm32_t *stm, std::string file);
void printMore(stm32_t *stm, port_interface *port);
void printDevInfo(serial *device);
void toggleBootStart(serial *device);
void toggleBootFinish(serial *device);



#endif //DFU_SERIAL_GPIO_H
