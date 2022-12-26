ENTRY(_start)
SECTIONS
{
        /* Load address has to be this as we map in 2mb sections 0x80000 */
        /* The physical addr that the kernel is loaded to is not aligned on 2mb */
        . = 0xffff000000000000;
        __LOW_MEMORY = .;
        __START = .;
        __TEXT_START = .;
        .text : ALIGN(4096) {
                KEEP(*(.text.boot))
                *(.text)
        }
        __TEXT_END = .;
        __RODATA_START = .;
        .rodata : ALIGN(4096)
        {
                *(.rodata)
        }
        __RODATA_END = .;
 
        __DATA_START = .;
        .data : ALIGN(4096)
        {
                *(.data)
        }
        __DATA_END = .;
 
        __BSS_START = .;
        .bss : ALIGN(4096)
        {
                *(.bss)
        }

        /* Initial page tables for the higher half */
        . = ALIGN(128);
        . += 4096;
        __kernel_init_stack = .;

        . = ALIGN(4096);
        __pgd_start = .;
        . += 4096;
        __pgd_end = .;
        __pud_start = .;
        . += 4096;
        __pud_end = .;
        __pmd_start = .;
        . += 4096;
        __pmd_end = .;
        __pte_start = .;
        . += 4096;
        __pte_end = .;
        __K_END = .;

        /* Initial page table for lower half */
        __init_pgd = .;
        . += 4096;
        __init_pgd_end = .;
        __init_pud = .;
        . += 4096;
        __init_pud_end = .;
        __END = .;

}
