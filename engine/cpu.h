#ifndef ENGINE_CPU_H
#define ENGINE_CPU_H

#include "memory.h"
#include "../shared/instruction.h"

typedef struct {
    rv_reg_t regs[32];
    rv_addr_t pc;
    memory_t *mem;
    bool halted;
} cpu_t;

void cpu_init(cpu_t *cpu, memory_t *mem, rv_addr_t entry_point);
bool cpu_step(cpu_t *cpu);

#endif // ENGINE_CPU_H
