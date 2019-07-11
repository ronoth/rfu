//
// Created by steven on 7/4/2019.
//

#ifndef DFU_STM32FLASH_LIB_H
#define DFU_STM32FLASH_LIB_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "stm32.h"

#define PORT_INIT_FAILED -2
#define STM_INIT_FAILED -1

int initPort(const char *device, stm32_t *stmout);

static int is_addr_in_ram(stm32_t *stm, uint32_t addr) {
    return addr >= stm->dev->ram_start && addr < stm->dev->ram_end;
}

static int is_addr_in_flash(stm32_t *stm, uint32_t addr) {
    return addr >= stm->dev->fl_start && addr < stm->dev->fl_end;
}

/* returns the page that contains address "addr" */
static int flash_addr_to_page_floor(stm32_t *stm, uint32_t addr) {
    int page;
    uint32_t *psize;

    if (!is_addr_in_flash(stm, addr))
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

    return page;
}

/* returns the first page whose start addr is >= "addr" */
int flash_addr_to_page_ceil(stm32_t *stm, uint32_t addr);

#ifdef __cplusplus
}
#endif

#endif //DFU_STM32FLASH_LIB_H
