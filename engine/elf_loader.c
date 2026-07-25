#include "elf_loader.h"
#include <stdio.h>
#include <string.h>

#define ELF_MAGIC 0x464C457FU // "\x7FELF"
#define EM_RISCV  0xF3        // RISC-V Architecture Code

typedef struct {
    uint8_t  e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} Elf32_Header;

typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} Elf32_PHeader;

bool elf_load(const char *filepath, memory_t *mem, rv_addr_t *entry_point) {
    if (!filepath || !mem || !entry_point) {
        return false;
    }

    FILE *f = fopen(filepath, "rb");
    if (!f) {
        return false;
    }

    Elf32_Header hdr;
    if (fread(&hdr, sizeof(hdr), 1, f) != 1) {
        fclose(f);
        return false;
    }

    // Check ELF Magic (\x7FELF) and RISC-V Architecture Machine ID (0xF3)
    if (hdr.e_ident[0] != 0x7F || hdr.e_ident[1] != 'E' || 
        hdr.e_ident[2] != 'L'  || hdr.e_ident[3] != 'F' || 
        hdr.e_machine != EM_RISCV) {
        fclose(f);
        return false;
    }

    *entry_point = hdr.e_entry;

    // Iterate over Program Headers
    for (int i = 0; i < hdr.e_phnum; i++) {
        Elf32_PHeader phdr;
        
        // Seek to program header offset
        long ph_offset = hdr.e_phoff + (i * sizeof(Elf32_PHeader));
        fseek(f, ph_offset, SEEK_SET);

        if (fread(&phdr, sizeof(phdr), 1, f) != 1) {
            continue;
        }

        // PT_LOAD segment (p_type == 1)
        if (phdr.p_type == 1) {
            // Check memory boundary limits
            if (phdr.p_vaddr + phdr.p_memsz > mem->size) {
                fclose(f);
                return false;
            }

            // Read segment bytes from file into RAM
            if (phdr.p_filesz > 0) {
                fseek(f, phdr.p_offset, SEEK_SET);
                if (fread(&mem->bytes[phdr.p_vaddr], 1, phdr.p_filesz, f) != phdr.p_filesz) {
                    fclose(f);
                    return false;
                }
            }

            // Zero-fill remaining BSS segment space if p_memsz > p_filesz
            if (phdr.p_memsz > phdr.p_filesz) {
                memset(&mem->bytes[phdr.p_vaddr + phdr.p_filesz], 0, phdr.p_memsz - phdr.p_filesz);
            }
        }
    }

    fclose(f);
    return true;
}
