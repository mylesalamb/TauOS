ENTRY(_start)
SECTIONS
{
        /* Load address has to be this as we map in 2mb sections 0x80000 */
        /* The physical addr that the kernel is loaded to is not aligned on 2mb */
        . = 0x40080000;
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
        __END = .;
}