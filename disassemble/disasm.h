#ifndef RV32I_DISASM_H
#define RV32I_DISASM_H

#include <stddef.h>
#include "../shared/instruction.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Converts a decoded RV32I instruction into human-readable assembly text.
 *
 * @param[in]  inst   Pointer to the decoded instruction structure.
 * @param[out] buffer Output string buffer.
 * @param[in]  size   Maximum allowed size of the buffer.
 * @return int Number of characters written, or a negative value on error.
 */
int rv_disasm(const rv_inst_t *inst, char *buffer, size_t size);

#ifdef __cplusplus
}
#endif

#endif // RV32I_DISASM_H
