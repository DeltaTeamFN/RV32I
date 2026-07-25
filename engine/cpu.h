#ifndef ENGINE_CPU_H
#define ENGINE_CPU_H

#include <stdbool.h>
#include "memory.h"
#include "../shared/instruction.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    rv_reg_t regs[32];  // General-purpose registers (x0-x31)
    rv_addr_t pc;       // Program Counter
    memory_t *mem;      // Pointer to attached memory instance
    bool halted;        // Execution halt status flag
} cpu_t;

/**
 * @brief Initializes CPU state, sets starting PC and attaches memory.
 * * @param[out] cpu         Pointer to CPU instance.
 * @param[in]  mem         Pointer to initialized memory layout.
 * @param[in]  entry_point Execution starting memory address.
 */
void cpu_init(cpu_t *cpu, memory_t *mem, rv_addr_t entry_point);

/**
 * @brief Executes a single instruction fetch-decode-execute step.
 * * @param[in,out] cpu Pointer to CPU instance.
 * @return true if instruction executed successfully, false if CPU halted or error occurred.
 */
bool cpu_step(cpu_t *cpu);

#ifdef __cplusplus
}
#endif

#endif // ENGINE_CPU_H
