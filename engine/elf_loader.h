#ifndef ENGINE_ELF_LOADER_H
#define ENGINE_ELF_LOADER_H

#include "memory.h"

bool elf_load(const char *filepath, memory_t *mem, rv_addr_t *entry_point);

#endif // ENGINE_ELF_LOADER_H
