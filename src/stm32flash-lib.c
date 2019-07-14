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
#include <stdint.h>
#include <stdio.h>
#include "stm32flash-lib.h"

#include "port.h"

int initPort(const char* device, stm32_t* stmout) {

    struct port_options port_opts;
    port_opts.device = device;
    port_opts.baudRate = SERIAL_BAUD_57600;
    port_opts.serial_mode = "8e1";
    port_opts.bus_addr = 0;
    port_opts.rx_frame_max = STM32_MAX_RX_FRAME;
    port_opts.tx_frame_max = STM32_MAX_TX_FRAME;

    struct port_interface *port;
    if (port_open(&port_opts, &port) != PORT_ERR_OK) {
       fprintf(stderr, "Failed to open port: %s\n", port_opts.device);
       return PORT_INIT_FAILED;
    }
    char init_flag = 1;
    stmout = stm32_init(port, init_flag);
    if (!stmout) {
        printf("Unable to init STM32\n");
        return STM_INIT_FAILED;
    }
    return 0;
}

/* returns the first page whose start addr is >= "addr" */
int flash_addr_to_page_ceil(stm32_t* stm, uint32_t addr)
{
    int page;
    uint32_t *psize;

    if (!(addr >= stm->dev->fl_start && addr <= stm->dev->fl_end))
        return 0;

    page = 0;
    addr -= stm->dev->fl_start;
    psize = stm->dev->fl_ps;

    while (addr >= psize[0]) {
        addr -= psize[0];
        page++;
        if (psize[1])
            psize++;
    }

    return addr ? page + 1 : page;
}
