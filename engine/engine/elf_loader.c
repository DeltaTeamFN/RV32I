#include "elf_loader.h"
#include <stdio.h>

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
} Elf32_Header;

typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
} Elf32_PHeader;

bool elf_load(const char *filepath, memory_t *mem, rv_addr_t *entry_point) {
    FILE *f = fopen(filepath, "rb");
    if (!f) return false;

    Elf32_Header hdr;
    if (fread(&hdr, sizeof(hdr), 1, f) != 1 || hdr.e_ident[0] != 0x7F || hdr.e_machine != 0xF3) {
        fclose(f);
        return false;
    }

    *entry_point = hdr.e_entry;

    fseek(f, hdr.e_phoff, SEEK_SET);
    for (int i = 0; i < hdr.e_phnum; i++) {
        Elf32_PHeader phdr;
        if (fread(&phdr, sizeof(phdr), 1, f) != 1) break;
        if (phdr.p_type == 1 && phdr.p_vaddr + phdr.p_filesz <= mem->size) { // PT_LOAD
            long current_pos = ftell(f);
            fseek(f, phdr.p_offset, SEEK_SET);
            fread(&mem->bytes[phdr.p_vaddr], phdr.p_filesz, 1, f);
            fseek(f, current_pos, SEEK_SET);
        }
    }

    fclose(f);
    return true;
}
