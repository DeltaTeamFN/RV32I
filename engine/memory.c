#include "memory.h"
#include <stdlib.h>
#include <stdio.h>

bool mem_init(memory_t *mem, size_t size) {
    if (!mem) return false;
    mem->bytes = (uint8_t *)calloc(1, size);
    mem->size = size;
    return mem->bytes != NULL;
}

void mem_free(memory_t *mem) {
    if (mem && mem->bytes) {
        free(mem->bytes);
        mem->bytes = NULL;
        mem->size = 0;
    }
}

// --- READ OPERATIONS ---

uint8_t mem_read8(memory_t *mem, rv_addr_t addr) {
    if (!mem || !mem->bytes || addr >= mem->size) {
        return 0;
    }
    return mem->bytes[addr];
}

uint16_t mem_read16(memory_t *mem, rv_addr_t addr) {
    if (!mem || !mem->bytes || addr + 1 >= mem->size) {
        return 0;
    }
    // Safe Little-Endian assembly
    return (uint16_t)mem->bytes[addr] |
           ((uint16_t)mem->bytes[addr + 1] << 8);
}

uint32_t mem_read32(memory_t *mem, rv_addr_t addr) {
    if (!mem || !mem->bytes || addr + 3 >= mem->size) {
        return 0;
    }
    // Safe Little-Endian assembly
    return (uint32_t)mem->bytes[addr] |
           ((uint32_t)mem->bytes[addr + 1] << 8) |
           ((uint32_t)mem->bytes[addr + 2] << 16) |
           ((uint32_t)mem->bytes[addr + 3] << 24);
}

// --- WRITE OPERATIONS ---

void mem_write8(memory_t *mem, rv_addr_t addr, uint8_t val) {
    // UART MMIO Output handling
    if (addr == RV_UART_MMIO) {
        putchar((char)val);
        fflush(stdout);
        return;
    }

    if (!mem || !mem->bytes || addr >= mem->size) {
        return;
    }
    mem->bytes[addr] = val;
}

void mem_write16(memory_t *mem, rv_addr_t addr, uint16_t val) {
    if (!mem || !mem->bytes || addr + 1 >= mem->size) {
        return;
    }
    mem->bytes[addr]     = (uint8_t)(val & 0xFF);
    mem->bytes[addr + 1] = (uint8_t)((val >> 8) & 0xFF);
}

void mem_write32(memory_t *mem, rv_addr_t addr, uint32_t val) {
    // UART MMIO Output handling
    if (addr == RV_UART_MMIO) {
        putchar((char)(val & 0xFF));
        fflush(stdout);
        return;
    }

    if (!mem || !mem->bytes || addr + 3 >= mem->size) {
        return;
    }
    mem->bytes[addr]     = (uint8_t)(val & 0xFF);
    mem->bytes[addr + 1] = (uint8_t)((val >> 8) & 0xFF);
    mem->bytes[addr + 2] = (uint8_t)((val >> 16) & 0xFF);
    mem->bytes[addr + 3] = (uint8_t)((val >> 24) & 0xFF);
}
