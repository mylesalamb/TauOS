ENTRY(_start)
SECTIONS
{
        /* Load address has to be this as we map in 2mb sections 0x80000 */
        /* The physical addr that the kernel is loaded to is not aligned on 2mb */
        . = 0xffff000000080000;
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
        
        /* From the pmd we can map in 2mb sections */
        /* Mapping up to 1Gb of memory */
        __pmd_start = .;
        . += 4096;
        __pmd_end = .;
        __K_END = .;

        /* Initial page table for lower half */
        /* We overlay this with the initial table for the vmm, ie the component that maps requested pages into the kernel address space */
        /* So that we can manipulate dynamically allocated memory*/
        __init_pgd = .;
        __vmm_pud = .;
        . += 4096;
        __vmm_pud_end = .;
        __init_pgd_end = .;
        
        __init_pud = .;
        __vmm_pmd = .;
        . += 4096;
        __vmm_pmd_end = .;
        __init_pud_end = .;

        __vmm_pte = .;
        . += 4096;
        __vmm_pte_end = .;
        __END = .;

}
