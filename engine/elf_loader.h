#ifndef ENGINE_ELF_LOADER_H
#define ENGINE_ELF_LOADER_H

#include <stdbool.h>
#include "memory.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parses a 32-bit RISC-V ELF file and loads executable segments into memory.
 *
 * @param[in]  filepath    Path to the .elf binary file.
 * @param[out] mem         Pointer to target memory structure.
 * @param[out] entry_point Pointer where the execution start address (PC) will be stored.
 * @return true on successful parsing and memory load, false on error.
 */
bool elf_load(const char *filepath, memory_t *mem, rv_addr_t *entry_point);

#ifdef __cplusplus
}
#endif

#endif // ENGINE_ELF_LOADER_H
