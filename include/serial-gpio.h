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

#include <string>
#include <stm32.h>

#ifdef WIN32
#include "Types.h"
#endif

void jumpToStart(stm32_t *stm);
void eraseFlash(stm32_t *stm);
void writeFlash(stm32_t *stm, std::string file);
void printDevInfo(stm32_t *stm, port_interface *port);
void toggleBootStart(port_interface *port);
void toggleBootFinish(port_interface *port);



#endif //DFU_SERIAL_GPIO_H
