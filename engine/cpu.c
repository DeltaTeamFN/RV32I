#include "cpu.h"
#include "../disassemble/disasm.h"
#include <stdio.h>

void cpu_init(cpu_t *cpu, memory_t *mem, rv_addr_t entry_point) {
    if (!cpu) return;

    for (int i = 0; i < 32; i++) {
        cpu->regs[i] = 0;
    }
    cpu->pc = entry_point;
    cpu->mem = mem;
    cpu->halted = false;
}

bool cpu_step(cpu_t *cpu) {
    if (!cpu || cpu->halted || !cpu->mem) {
        return false;
    }

    // Fetch 32-bit instruction
    rv_inst_raw_t raw = mem_read32(cpu->mem, cpu->pc);

    // Decode instruction
    rv_inst_t inst = rv_decode(raw);
    rv_addr_t next_pc = cpu->pc + 4;

    // Helper shortcuts for registers and signed access
    uint8_t rd  = inst.rd;
    uint8_t rs1 = inst.rs1;
    uint8_t rs2 = inst.rs2;

    rv_sreg_t s_rs1 = (rv_sreg_t)cpu->regs[rs1];
    rv_sreg_t s_rs2 = (rv_sreg_t)cpu->regs[rs2];

    switch (inst.opcode) {
        // LUI
        case 0x37:
            if (rd != 0) cpu->regs[rd] = (rv_reg_t)inst.imm;
            break;

        // AUIPC
        case 0x17:
            if (rd != 0) cpu->regs[rd] = cpu->pc + inst.imm;
            break;

        // JAL
        case 0x6F:
            if (rd != 0) cpu->regs[rd] = next_pc;
            next_pc = cpu->pc + inst.imm;
            break;

        // JALR
        case 0x67: {
            rv_addr_t target = (cpu->regs[rs1] + inst.imm) & ~1u; // LSB set to 0
            if (rd != 0) cpu->regs[rd] = next_pc;
            next_pc = target;
            break;
        }

        // BRANCH (B-Type)
        case 0x63:
            switch (inst.funct3) {
                case 0x0: if (cpu->regs[rs1] == cpu->regs[rs2]) next_pc = cpu->pc + inst.imm; break; // BEQ
                case 0x1: if (cpu->regs[rs1] != cpu->regs[rs2]) next_pc = cpu->pc + inst.imm; break; // BNE
                case 0x4: if (s_rs1 < s_rs2)                    next_pc = cpu->pc + inst.imm; break; // BLT
                case 0x5: if (s_rs1 >= s_rs2)                   next_pc = cpu->pc + inst.imm; break; // BGE
                case 0x6: if (cpu->regs[rs1] < cpu->regs[rs2])  next_pc = cpu->pc + inst.imm; break; // BLTU
                case 0x7: if (cpu->regs[rs1] >= cpu->regs[rs2]) next_pc = cpu->pc + inst.imm; break; // BGEU
                default: break;
            }
            break;

        // LOAD (I-Type)
        case 0x03: {
            rv_addr_t addr = cpu->regs[rs1] + inst.imm;
            if (rd != 0) {
                switch (inst.funct3) {
                    case 0x0: cpu->regs[rd] = (int8_t)mem_read8(cpu->mem, addr);   break; // LB
                    case 0x1: cpu->regs[rd] = (int16_t)mem_read16(cpu->mem, addr); break; // LH
                    case 0x2: cpu->regs[rd] = mem_read32(cpu->mem, addr);          break; // LW
                    case 0x4: cpu->regs[rd] = mem_read8(cpu->mem, addr);           break; // LBU
                    case 0x5: cpu->regs[rd] = mem_read16(cpu->mem, addr);          break; // LHU
                    default: break;
                }
            }
            break;
        }

        // STORE (S-Type)
        case 0x23: {
            rv_addr_t addr = cpu->regs[rs1] + inst.imm;
            switch (inst.funct3) {
                case 0x0: mem_write8(cpu->mem, addr, (uint8_t)cpu->regs[rs2]);   break; // SB
                case 0x1: mem_write16(cpu->mem, addr, (uint16_t)cpu->regs[rs2]); break; // SH
                case 0x2: mem_write32(cpu->mem, addr, cpu->regs[rs2]);            break; // SW
                default: break;
            }
            break;
        }

        // OP-IMM (I-Type)
        case 0x13:
            if (rd != 0) {
                switch (inst.funct3) {
                    case 0x0: cpu->regs[rd] = cpu->regs[rs1] + inst.imm; break; // ADDI
                    case 0x1: cpu->regs[rd] = cpu->regs[rs1] << (inst.imm & 0x1F); break; // SLLI
                    case 0x2: cpu->regs[rd] = (s_rs1 < (rv_sreg_t)inst.imm) ? 1 : 0; break; // SLTI
                    case 0x3: cpu->regs[rd] = (cpu->regs[rs1] < (rv_reg_t)inst.imm) ? 1 : 0; break; // SLTIU
                    case 0x4: cpu->regs[rd] = cpu->regs[rs1] ^ inst.imm; break; // XORI
                    case 0x5:
                        if (inst.funct7 == 0x20) {
                            cpu->regs[rd] = (rv_reg_t)(s_rs1 >> (inst.imm & 0x1F)); // SRAI
                        } else {
                            cpu->regs[rd] = cpu->regs[rs1] >> (inst.imm & 0x1F); // SRLI
                        }
                        break;
                    case 0x6: cpu->regs[rd] = cpu->regs[rs1] | inst.imm; break; // ORI
                    case 0x7: cpu->regs[rd] = cpu->regs[rs1] & inst.imm; break; // ANDI
                    default: break;
                }
            }
            break;

        // OP (R-Type)
        case 0x33:
            if (rd != 0) {
                switch (inst.funct3) {
                    case 0x0:
                        if (inst.funct7 == 0x20) cpu->regs[rd] = cpu->regs[rs1] - cpu->regs[rs2]; // SUB
                        else cpu->regs[rd] = cpu->regs[rs1] + cpu->regs[rs2]; // ADD
                        break;
                    case 0x1: cpu->regs[rd] = cpu->regs[rs1] << (cpu->regs[rs2] & 0x1F); break; // SLL
                    case 0x2: cpu->regs[rd] = (s_rs1 < s_rs2) ? 1 : 0; break; // SLT
                    case 0x3: cpu->regs[rd] = (cpu->regs[rs1] < cpu->regs[rs2]) ? 1 : 0; break; // SLTU
                    case 0x4: cpu->regs[rd] = cpu->regs[rs1] ^ cpu->regs[rs2]; break; // XOR
                    case 0x5:
                        if (inst.funct7 == 0x20) {
                            cpu->regs[rd] = (rv_reg_t)(s_rs1 >> (cpu->regs[rs2] & 0x1F)); // SRA
                        } else {
                            cpu->regs[rd] = cpu->regs[rs1] >> (cpu->regs[rs2] & 0x1F); // SRL
                        }
                        break;
                    case 0x6: cpu->regs[rd] = cpu->regs[rs1] | cpu->regs[rs2]; break; // OR
                    case 0x7: cpu->regs[rd] = cpu->regs[rs1] & cpu->regs[rs2]; break; // AND
                    default: break;
                }
            }
            break;

        // SYSTEM
        case 0x73:
            cpu->halted = true;
            return false;

        default:
            // Unknown opcode: halt execution
            cpu->halted = true;
            return false;
    }

    // x0 must strictly remain 0 at all times
    cpu->regs[0] = 0;
    cpu->pc = next_pc;

    return true;
}
