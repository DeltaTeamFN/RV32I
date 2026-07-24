#include "cpu.h"
#include "../disassemble/disasm.h"
#include <stdio.h>

void cpu_init(cpu_t *cpu, memory_t *mem, rv_addr_t entry_point) {
    for (int i = 0; i < 32; i++) cpu->regs[i] = 0;
    cpu->pc = entry_point;
    cpu->mem = mem;
    cpu->halted = false;
}

bool cpu_step(cpu_t *cpu) {
    if (cpu->halted) return false;

    rv_inst_raw_t raw = mem_read32(cpu->mem, cpu->pc);
    if (raw == 0x00000073 || raw == 0x00000000) { // ECALL or NULL
        cpu->halted = true;
        return false;
    }

    rv_inst_t inst = rv_decode(raw);
    rv_addr_t next_pc = cpu->pc + 4;

    switch (inst.opcode) {
        case 0x37: cpu->regs[inst.rd] = inst.imm; break; // LUI
        case 0x17: cpu->regs[inst.rd] = cpu->pc + inst.imm; break; // AUIPC
        case 0x6F: cpu->regs[inst.rd] = next_pc; next_pc = cpu->pc + inst.imm; break; // JAL
        case 0x13: // ALU Imm
            if (inst.funct3 == 0) cpu->regs[inst.rd] = cpu->regs[inst.rs1] + inst.imm; // ADDI
            break;
        case 0x33: // ALU Reg
            if (inst.funct3 == 0) {
                if (inst.funct7 == 0x20) cpu->regs[inst.rd] = cpu->regs[inst.rs1] - cpu->regs[inst.rs2]; // SUB
                else cpu->regs[inst.rd] = cpu->regs[inst.rs1] + cpu->regs[inst.rs2]; // ADD
            }
            break;
        case 0x23: // Store
            if (inst.funct3 == 2) mem_write32(cpu->mem, cpu->regs[inst.rs1] + inst.imm, cpu->regs[inst.rs2]);
            break;
    }

    cpu->regs[0] = 0; // x0 is always 0
    cpu->pc = next_pc;
    return true;
}
