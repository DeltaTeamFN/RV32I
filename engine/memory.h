#ifndef ENGINE_MEMORY_H
#define ENGINE_MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../shared/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t *bytes;
    size_t size;
} memory_t;

/**
 * @brief Allocates and zeroes out the simulated RAM memory block.
 */
bool mem_init(memory_t *mem, size_t size);

/**
 * @brief Frees the allocated memory block.
 */
void mem_free(memory_t *mem);

// --- Read Operations ---
uint8_t  mem_read8(memory_t *mem, rv_addr_t addr);
uint16_t mem_read16(memory_t *mem, rv_addr_t addr);
uint32_t mem_read32(memory_t *mem, rv_addr_t addr);

// --- Write Operations ---
void mem_write8(memory_t *mem, rv_addr_t addr, uint8_t val);
void mem_write16(memory_t *mem, rv_addr_t addr, uint16_t val);
void mem_write32(memory_t *mem, rv_addr_t addr, uint32_t val);

#ifdef __cplusplus
}
#endif

#endif // ENGINE_MEMORY_H
