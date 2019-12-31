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
#include <string>

#include "serial-gpio.h"
#include "port.h"
#include "stm32.h"
#include "sleepms.h"
#include "parsers/binary.h"
#include "sys/types.h"


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
    stm = stm32_init(stm->port, 1);
    sleep_ms(100);
    uint32_t execute = stm->dev->fl_start;
    printf("\nStarting execution at address 0x%08x... ", execute);
    if (stm32_go(stm, execute) == STM32_ERR_OK)
        printf("done.\n");
    else
        printf("failed.\n");
}

void printDevInfo(stm32_t *stm, port_interface *port) {
    printf("Interface %s: %s\n", port->name, port->get_cfg_str(port));

    printf("Version      : 0x%02x\n", stm->bl_version);
    if (port->flags & PORT_GVR_ETX) {
        printf("Option 1     : 0x%02x\n", stm->option1);
        printf("Option 2     : 0x%02x\n", stm->option2);
    }
    printf("Device ID    : 0x%04x (%s)\n", stm->pid, stm->dev->name);
    printf("- RAM        : %dKiB  (%db reserved by bootloader)\n", (stm->dev->ram_end - 0x20000000) / 1024, stm->dev->ram_start - 0x20000000);
    printf("- Flash      : %dKiB (size first sector: %dx%d)\n", (stm->dev->fl_end - stm->dev->fl_start ) / 1024, stm->dev->fl_pps, stm->dev->fl_ps[0]);
    printf("- Option RAM : %db\n", stm->dev->opt_end - stm->dev->opt_start + 1);
    printf("- System RAM : %dKiB\n", (stm->dev->mem_end - stm->dev->mem_start) / 1024);
}


void writeFlash(stm32_t *stm, std::string file) {

    stm = stm32_init(stm->port, 1);
    off_t offset = 0;
    uint32_t addr = 0x08000000;
    uint32_t end =  0x08030000;
    unsigned int size = 0;
    unsigned int len = 0;
    bool verify = 0;
    unsigned int max_wlen;
    uint8_t		buffer[256];
    /* now try binary */
    void *p_st = NULL; // pointer to "storage"
    parser_t	*parser		= NULL;
    parser = &PARSER_BINARY;

    stm32_err_t s_err;
    const char* filename = file.c_str();

    max_wlen = STM32_MAX_TX_FRAME - 2;	/* skip len and crc */
    max_wlen &= ~3;	/* 32 bit aligned */

//    max_rlen = STM32_MAX_RX_FRAME;
    //max_rlen = max_rlen < max_wlen ? max_rlen : max_wlen;

    p_st = parser->init();


    if (!p_st) {
        fprintf(stderr, "%s Parser failed to initialize\n", parser->name);
        goto close;
    }
    if (parser->open(p_st, filename, 0) != PARSER_ERR_OK) {
        fprintf(stderr, "%s parser failed to open file %s\n", parser->name, filename);
    }
    size = parser->size(p_st);

    printf("Writing file %s\n", filename);
    while(addr < end && offset < size) {

        uint32_t left	= end - addr;
        len		= max_wlen > left ? left : max_wlen;
        len		= len > size - offset ? size - offset : len;

        if (parser->read(p_st, buffer, &len) != PARSER_ERR_OK) {
            fprintf(stderr, "Parser Error\n");
            goto close;
        }

        if (len == 0) {
            if (filename[0] == '-') {
                break;
            } else {
                fprintf(stderr, "Failed to read input file\n");
                goto close;
            }
        }

        s_err = stm32_write_memory(stm, addr, buffer, len);
        if (s_err != STM32_ERR_OK) {
            fprintf(stderr, "Failed to write memory at address 0x%08x\n", addr);
            goto close;
        }

        addr	+= len;
        offset	+= len;

        fprintf(stdout,
                "\rWrote %saddress 0x%08x (%.2f%%) ",
                verify ? "and verified " : "",
                addr,
                (100.0f / size) * offset
        );
        fflush(stdout);

    }

    close:
    if (p_st) parser->close(p_st);
}

void toggleBootStart(port_interface *port) {
    port->gpio(port, GPIO_RTS,1);
    sleep_ms(100);
    port->gpio(port, GPIO_DTR, 1);
    sleep_ms(100);
    port->gpio(port, GPIO_RTS, 0);
    sleep_ms(100);
}

void toggleBootFinish(port_interface *port) {
    port->gpio(port, GPIO_DTR, 0);
    sleep_ms(100);
    port->gpio(port, GPIO_RTS, 1);
    sleep_ms(100);
    port->gpio(port, GPIO_RTS, 0);
    sleep_ms(100);
}
