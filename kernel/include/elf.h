#ifndef ELF_H
#define ELF_H 1

#include <types.h>

struct __attribute__((packed)) elf_header {
	u32 ei_magic;
	u8 ei_class;
	u8 ei_version;
	u8 ei_osabi;
	u8 ei_abiversion;
	u8 _ei_padding[7];

	/* Hardcoded for 64 bit */
	u16 e_type;
	u16 e_machine;
	u32 e_version;
	u64 e_entry;
	u64 e_phoff;
	u64 e_shoff;
	u32 e_flags;
	u16 e_ehsize;
	u16 e_phentsize;
	u16 e_phnum;
	u16 e_shentsize;
	u16 e_shnum;
	u16 e_shstrndx;
};

struct __attribute__((packed)) elf_program_header {
	u32 p_type;
	u32 p_flags;
	u64 p_offset;
	u64 p_vaddr;
	u64 p_paddr;
	u64 p_filesz;
	u64 p_memsz;
	u64 p_align;
};

struct __attribute__((packed)) elf_section_header {
	u32 sh_name;
	u32 sh_type;
	u64 sh_addr;
	u64 sh_offset;
	u64 sh_size;
	u32 sh_link;
	u32 sh_info;
	u64 sh_addralign;
	u64 sh_entsize;
};

#endif
