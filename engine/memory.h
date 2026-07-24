#ifndef ENGINE_MEMORY_H
#define ENGINE_MEMORY_H

#include "../shared/types.h"

typedef struct {
    uint8_t *bytes;
    size_t size;
} memory_t;

bool mem_init(memory_t *mem, size_t size);
void mem_free(memory_t *mem);

uint32_t mem_read32(memory_t *mem, rv_addr_t addr);
void mem_write32(memory_t *mem, rv_addr_t addr, uint32_t val);

#endif // ENGINE_MEMORY_H
