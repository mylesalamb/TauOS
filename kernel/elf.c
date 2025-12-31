#include <types.h>
#include <elf.h>

#define ELF_MAGIC           (0x464C457F)
#define ELF_VERSION         (0x1)
#define ELF_CLASS_64BIT     (0x2)
#define ELF_TYPE_ET_REL     (0x1)
#define ELF_MACH_AARCH64    (0xB7)
int elf_check_header(struct elf_header *h)
{
	if (h->ei_magic != ELF_MAGIC)
		return -1;

	if (h->ei_version != ELF_VERSION)
		return -1;

	if (h->ei_class != ELF_CLASS_64BIT)
		return -1;

	if (h->e_type != ELF_TYPE_ET_REL)
		return -1;

	if (h->e_machine != ELF_MACH_AARCH64)
		return -1;

	return 0;
}
