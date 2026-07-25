#include "disasm.h"
#include <stdio.h>

// Complete RISC-V ABI Register Name Table (32 registers)
static const char *REG_NAMES[32] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0",   "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6",   "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8",   "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

int rv_disasm(const rv_inst_t *inst, char *buf, size_t size) {
    if (!inst || !buf || size == 0) {
        return -1;
    }

    // Ensure register indices are within safe bounds [0..31]
    unsigned int rd  = inst->rd & 0x1F;
    unsigned int rs1 = inst->rs1 & 0x1F;
    unsigned int rs2 = inst->rs2 & 0x1F;

    switch (inst->opcode) {
        // LUI
        case 0x37: 
            return snprintf(buf, size, "lui    %s, 0x%X", REG_NAMES[rd], inst->imm);

        // AUIPC
        case 0x17: 
            return snprintf(buf, size, "auipc  %s, 0x%X", REG_NAMES[rd], inst->imm);

        // JAL
        case 0x6F: 
            return snprintf(buf, size, "jal    %s, %d", REG_NAMES[rd], inst->imm);

        // JALR
        case 0x67: 
            return snprintf(buf, size, "jalr   %s, %d(%s)", REG_NAMES[rd], inst->imm, REG_NAMES[rs1]);

        // Branch (B-type)
        case 0x63:
            switch (inst->funct3) {
                case 0x0: return snprintf(buf, size, "beq    %s, %s, %d", REG_NAMES[rs1], REG_NAMES[rs2], inst->imm);
                case 0x1: return snprintf(buf, size, "bne    %s, %s, %d", REG_NAMES[rs1], REG_NAMES[rs2], inst->imm);
                case 0x4: return snprintf(buf, size, "blt    %s, %s, %d", REG_NAMES[rs1], REG_NAMES[rs2], inst->imm);
                case 0x5: return snprintf(buf, size, "bge    %s, %s, %d", REG_NAMES[rs1], REG_NAMES[rs2], inst->imm);
                case 0x6: return snprintf(buf, size, "bltu   %s, %s, %d", REG_NAMES[rs1], REG_NAMES[rs2], inst->imm);
                case 0x7: return snprintf(buf, size, "bgeu   %s, %s, %d", REG_NAMES[rs1], REG_NAMES[rs2], inst->imm);
                default:  return snprintf(buf, size, "unknown-branch (0x%08X)", inst->raw);
            }

        // OP-IMM (I-type)
        case 0x13:
            switch (inst->funct3) {
                case 0x0: 
                    // NOP pseudo-instruction alias check (addi zero, zero, 0)
                    if (rd == 0 && rs1 == 0 && inst->imm == 0) {
                        return snprintf(buf, size, "nop");
                    }
                    return snprintf(buf, size, "addi   %s, %s, %d", REG_NAMES[rd], REG_NAMES[rs1], inst->imm);
                case 0x1: return snprintf(buf, size, "slli   %s, %s, %u", REG_NAMES[rd], REG_NAMES[rs1], inst->imm & 0x1F);
                case 0x2: return snprintf(buf, size, "slti   %s, %s, %d", REG_NAMES[rd], REG_NAMES[rs1], inst->imm);
                case 0x3: return snprintf(buf, size, "sltiu  %s, %s, %u", REG_NAMES[rd], REG_NAMES[rs1], (unsigned int)inst->imm);
                case 0x4: return snprintf(buf, size, "xori   %s, %s, %d", REG_NAMES[rd], REG_NAMES[rs1], inst->imm);
                case 0x5: 
                    if (inst->funct7 == 0x20)
                        return snprintf(buf, size, "srai   %s, %s, %u", REG_NAMES[rd], REG_NAMES[rs1], inst->imm & 0x1F);
                    return snprintf(buf, size, "srli   %s, %s, %u", REG_NAMES[rd], REG_NAMES[rs1], inst->imm & 0x1F);
                case 0x6: return snprintf(buf, size, "ori    %s, %s, %d", REG_NAMES[rd], REG_NAMES[rs1], inst->imm);
                case 0x7: return snprintf(buf, size, "andi   %s, %s, %d", REG_NAMES[rd], REG_NAMES[rs1], inst->imm);
                default:  return snprintf(buf, size, "unknown-op-imm (0x%08X)", inst->raw);
            }

        // OP (R-type)
        case 0x33:
            switch (inst->funct3) {
                case 0x0: 
                    if (inst->funct7 == 0x20) return snprintf(buf, size, "sub    %s, %s, %s", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);
                    return snprintf(buf, size, "add    %s, %s, %s", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);
                case 0x1: return snprintf(buf, size, "sll    %s, %s, %s", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);
                case 0x2: return snprintf(buf, size, "slt    %s, %s, %s", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);
                case 0x3: return snprintf(buf, size, "sltu   %s, %s, %s", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);
                case 0x4: return snprintf(buf, size, "xor    %s, %s, %s", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);
                case 0x5: 
                    if (inst->funct7 == 0x20) return snprintf(buf, size, "sra    %s, %s, %s", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);
                    return snprintf(buf, size, "srl    %s, %s, %s", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);
                case 0x6: return snprintf(buf, size, "or     %s, %s, %s", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);
                case 0x7: return snprintf(buf, size, "and    %s, %s, %s", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);
                default:  return snprintf(buf, size, "unknown-op (0x%08X)", inst->raw);
            }

        // LOAD (I-type)
        case 0x03:
            switch (inst->funct3) {
                case 0x0: return snprintf(buf, size, "lb     %s, %d(%s)", REG_NAMES[rd], inst->imm, REG_NAMES[rs1]);
                case 0x1: return snprintf(buf, size, "lh     %s, %d(%s)", REG_NAMES[rd], inst->imm, REG_NAMES[rs1]);
                case 0x2: return snprintf(buf, size, "lw     %s, %d(%s)", REG_NAMES[rd], inst->imm, REG_NAMES[rs1]);
                case 0x4: return snprintf(buf, size, "lbu    %s, %d(%s)", REG_NAMES[rd], inst->imm, REG_NAMES[rs1]);
                case 0x5: return snprintf(buf, size, "lhu    %s, %d(%s)", REG_NAMES[rd], inst->imm, REG_NAMES[rs1]);
                default:  return snprintf(buf, size, "unknown-load (0x%08X)", inst->raw);
            }

        // STORE (S-type)
        case 0x23:
            switch (inst->funct3) {
                case 0x0: return snprintf(buf, size, "sb     %s, %d(%s)", REG_NAMES[rs2], inst->imm, REG_NAMES[rs1]);
                case 0x1: return snprintf(buf, size, "sh     %s, %d(%s)", REG_NAMES[rs2], inst->imm, REG_NAMES[rs1]);
                case 0x2: return snprintf(buf, size, "sw     %s, %d(%s)", REG_NAMES[rs2], inst->imm, REG_NAMES[rs1]);
                default:  return snprintf(buf, size, "unknown-store (0x%08X)", inst->raw);
            }

        // SYSTEM
        case 0x73:
            if (inst->imm == 0) return snprintf(buf, size, "ecall");
            if (inst->imm == 1) return snprintf(buf, size, "ebreak");
            return snprintf(buf, size, "system (0x%08X)", inst->raw);

        default:
            return snprintf(buf, size, "unknown (0x%08X)", inst->raw);
    }
}
