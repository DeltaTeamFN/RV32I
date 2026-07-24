#ifndef DISASM_H
#define DISASM_H

#include "../shared/instruction.h"

// Converts the decoded instruction into a string buffer
void rv_disasm(const rv_inst_t *inst, char *buffer, size_t size);

#endif // DISASM_H
