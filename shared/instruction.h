#ifndef SHARED_INSTRUCTION_H
#define SHARED_INSTRUCTION_H

#include <stdint.h>
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FMT_R, 
    FMT_I, 
    FMT_S, 
    FMT_B, 
    FMT_U, 
    FMT_J, 
    FMT_UNKNOWN
} inst_format_t;

typedef struct {
    rv_inst_raw_t raw;
    uint8_t opcode;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t funct3;
    uint8_t funct7;
    int32_t imm;
    inst_format_t format;
} rv_inst_t;

/**
 * @brief Decodes a raw 32-bit RISC-V instruction into a structured format.
 */
static inline rv_inst_t rv_decode(rv_inst_raw_t raw) {
    rv_inst_t inst = {0};
    inst.raw    = raw;
    inst.opcode = raw & 0x7F;
    inst.rd     = (raw >> 7) & 0x1F;
    inst.funct3 = (raw >> 12) & 0x7;
    inst.rs1    = (raw >> 15) & 0x1F;
    inst.rs2    = (raw >> 20) & 0x1F;
    inst.funct7 = (raw >> 25) & 0x7F;

    // Sign extension calculations for immediate values
    int32_t i_imm = (int32_t)raw >> 20;

    int32_t s_imm = ((int32_t)(raw & 0xFE000000) >> 20) | 
                    ((raw >> 7) & 0x1F);

    int32_t b_imm = ((int32_t)(raw & 0x80000000) >> 19) | 
                    ((raw & 0x80) << 4) |
                    ((raw & 0x7E000000) >> 20) | 
                    ((raw & 0x0F00) >> 7);

    int32_t u_imm = (int32_t)(raw & 0xFFFFF000);

    // FIXED: Corrected bit 20 mask from 0x01000000 -> 0x00100000
    int32_t j_imm = ((int32_t)(raw & 0x80000000) >> 11) | 
                    (raw & 0xFF000) |
                    ((raw & 0x00100000) >> 9) | 
                    ((raw & 0x7FE00000) >> 20);

    switch (inst.opcode) {
        case 0x33: // OP (R-type)
            inst.format = FMT_R; 
            break;

        case 0x13: // OP-IMM
        case 0x67: // JALR
        case 0x03: // LOAD
        case 0x73: // SYSTEM (ECALL/EBREAK)
        case 0x0F: // FENCE
            inst.format = FMT_I; 
            inst.imm = i_imm; 
            break;

        case 0x23: // STORE
            inst.format = FMT_S; 
            inst.imm = s_imm; 
            break;

        case 0x63: // BRANCH
            inst.format = FMT_B; 
            inst.imm = b_imm; 
            break;

        case 0x37: // LUI
        case 0x17: // AUIPC
            inst.format = FMT_U; 
            inst.imm = u_imm; 
            break;

        case 0x6F: // JAL
            inst.format = FMT_J; 
            inst.imm = j_imm; 
            break;

        default:
            inst.format = FMT_UNKNOWN; 
            break;
    }

    return inst;
}

#ifdef __cplusplus
}
#endif

#endif // SHARED_INSTRUCTION_H
