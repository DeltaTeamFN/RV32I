#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <size_t.h>

typedef uint32_t rv_addr_t;
typedef uint32_t rv_reg_t;
typedef uint32_t rv_inst_raw_t;

#define RV_RAM_SIZE  (1024 * 1024) // 1 MB
#define RV_UART_MMIO 0x80000000

#endif // SHARED_TYPES_H
