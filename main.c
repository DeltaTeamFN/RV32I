#include <stdio.h>
#include <string.h>
#include "engine/memory.h"
#include "engine/elf_loader.h"
#include "engine/cpu.h"
#include "disassemble/disasm.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <binary.elf> [--trace]\n", argv[0]);
        return 1;
    }

    bool trace_mode = (argc > 2 && strcmp(argv[2], "--trace") == 0);

    memory_t mem;
    if (!mem_init(&mem, RV_RAM_SIZE)) return 1;

    rv_addr_t entry_point = 0;
    if (!elf_load(argv[1], &mem, &entry_point)) {
        fprintf(stderr, "Failed to load ELF file.\n");
        mem_free(&mem);
        return 1;
    }

    cpu_t cpu;
    cpu_init(&cpu, &mem, entry_point);

    while (!cpu.halted) {
        if (trace_mode) {
            rv_inst_raw_t raw = mem_read32(&mem, cpu.pc);
            rv_inst_t inst = rv_decode(raw);
            char disasm_buf[64];
            rv_disasm(&inst, disasm_buf, sizeof(disasm_buf));
            printf("[0x%08X] %s\n", cpu.pc, disasm_buf);
        }
        if (!cpu_step(&cpu)) break;
    }

    mem_free(&mem);
    return 0;
}
