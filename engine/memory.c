#include "memory.h"
#include <stdlib.h>
#include <stdio.h>

bool mem_init(memory_t *mem, size_t size) {
    mem->bytes = calloc(1, size);
    mem->size = size;
    return mem->bytes != NULL;
}

void mem_free(memory_t *mem) {
    if (mem->bytes) free(mem->bytes);
}

uint32_t mem_read32(memory_t *mem, rv_addr_t addr) {
    if (addr + 3 >= mem->size) return 0;
    return *(uint32_t*)&mem->bytes[addr];
}

void mem_write32(memory_t *mem, rv_addr_t addr, uint32_t val) {
    if (addr == RV_UART_MMIO) {
        putchar((char)(val & 0xFF));
        fflush(stdout);
        return;
    }
    if (addr + 3 < mem->size) {
        *(uint32_t*)&mem->bytes[addr] = val;
    }
}
