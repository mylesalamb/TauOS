#include <mm/pt.h>

#define PAGES(va_start, va_end, shift) (((va_end >> shift) - (va_start >> shift)) + 1)
#define PGD_PAGES(va_start, va_end) (PAGES(va_start, va_end, MMU_PGD_SHIFT))
#define PUD_PAGES(va_start, va_end) (PAGES(va_start, va_end, MMU_PUD_SHIFT))
#define PMD_PAGES(va_start, va_end) (PAGES(va_start, va_end, MMU_PMD_SHIFT))
#define PTE_PAGES(va_start, va_end) (PAGES(va_start, va_end, MMU_PTE_SHIFT))
#define IDMAP_PAGES(va_start, va_end) ((PGD_PAGES(va_start, va_end) + PUD_PAGES(va_start, va_end) + PMD_PAGES(va_start, va_end) + PTE_PAGES(va_start, va_end)) * MMU_BLK_SIZE)


ENTRY(_entry)
SECTIONS
{
        /* Dummy load address */
        /* All we want is a 4k aligned address */
        . = 0x0000;
        __LOW_MEMORY = .;
        __START = .;
        .text : ALIGN(4096) {
        	__TEXT_START = .;
                KEEP(*(.text.boot))
                *(.text)
        	__TEXT_END = .;
        } : text_segment
        
        
        
        .rodata : ALIGN(4096)
        {
        	__RODATA_START = .;
                *(.rodata)
        } : rodata_segment
        .rela : ALIGN(8) {
                __RELA_START = .;
                *(.rela .rela*)
                __RELA_END = .;
                __RODATA_END = .;
        } : rodata_segment
 
        .data : ALIGN(4096)
        {
        	__DATA_START = .;
                *(.data)
        	__DATA_END = .;
        } : data_segment
        .bss : ALIGN(4096)
        {
        	__BSS_START = .;
                *(.bss)
        	__BSS_END = .;
        } : data_segment


        .idmap_data : ALIGN(4096) {
                __IDMAP_DATA_START = .;
                . += 4096 * 2;
                __kernel_init_stack = .;
                __idmap_blocks = .;
                . += IDMAP_PAGES(__START, __END);
                __idmap_blocks_end = .;
                __IDMAP_DATA_END = .;
        } : data_segment

        . = ALIGN(4096);
        __END = .;


}

PHDRS {
        text_segment PT_LOAD FLAGS(5);
        data_segment PT_LOAD FLAGS(6);
        rodata_segment PT_LOAD FLAGS(4);
}
