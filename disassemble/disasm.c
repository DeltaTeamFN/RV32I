#include "disasm.h"
#include <stdio.h>

static const char* REG_NAMES[32] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0",   "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6",   "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "t3",   "t4", "t5", "t6"
};

void rv_disasm(const rv_inst_t *inst, char *buf, size_t size) {
    switch (inst->opcode) {
        case 0x37: snprintf(buf, size, "lui    %s, 0x%X", REG_NAMES[inst->rd], inst->imm >> 12); break;
        case 0x17: snprintf(buf, size, "auipc  %s, 0x%X", REG_NAMES[inst->rd], inst->imm >> 12); break;
        case 0x6F: snprintf(buf, size, "jal    %s, %d", REG_NAMES[inst->rd], inst->imm); break;
        case 0x13:
            if (inst->funct3 == 0) snprintf(buf, size, "addi   %s, %s, %d", REG_NAMES[inst->rd], REG_NAMES[inst->rs1], inst->imm);
            else snprintf(buf, size, "op-imm %s, %s, %d", REG_NAMES[inst->rd], REG_NAMES[inst->rs1], inst->imm);
            break;
        case 0x33:
            if (inst->funct3 == 0 && inst->funct7 == 0) snprintf(buf, size, "add    %s, %s, %s", REG_NAMES[inst->rd], REG_NAMES[inst->rs1], REG_NAMES[inst->rs2]);
            else if (inst->funct3 == 0 && inst->funct7 == 0x20) snprintf(buf, size, "sub    %s, %s, %s", REG_NAMES[inst->rd], REG_NAMES[inst->rs1], REG_NAMES[inst->rs2]);
            else snprintf(buf, size, "op-reg %s, %s, %s", REG_NAMES[inst->rd], REG_NAMES[inst->rs1], REG_NAMES[inst->rs2]);
            break;
        case 0x23: snprintf(buf, size, "sw     %s, %d(%s)", REG_NAMES[inst->rs2], inst->imm, REG_NAMES[inst->rs1]); break;
        default:   snprintf(buf, size, "unknown (0x%08X)", inst->raw); break;
    }
}
