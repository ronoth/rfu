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
#include "Types.h"
#include "CP210xRuntimeDLL.h"


int setGPIO(HANDLE *device, uint16_t gpio, BOOL level);


void eraseFlash(stm32_t *stm);
void printMore(stm32_t *stm, port_interface *port);
void printDevInfo(HANDLE *device);
void toggleBootStart(HANDLE *device);
void toggleBootFinish(HANDLE *device);



#endif //DFU_SERIAL_GPIO_H
