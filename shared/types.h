#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// RISC-V RV32 Core Data Types
typedef uint32_t rv_addr_t;     // 32-bit Memory Address
typedef uint32_t rv_reg_t;      // 32-bit Register Value (Unsigned)
typedef int32_t  rv_sreg_t;     // 32-bit Register Value (Signed)
typedef uint32_t rv_inst_raw_t; // Raw 32-bit Instruction Word

// Memory & MMIO Definitions
#define RV_RAM_SIZE  (1024 * 1024) // 1 MB Default RAM Size
#define RV_UART_MMIO 0x80000000    // UART Memory-Mapped I/O Address

#ifdef __cplusplus
}
#endif

#endif // SHARED_TYPES_H
