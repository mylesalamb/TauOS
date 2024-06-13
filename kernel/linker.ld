ENTRY(_entry)
SECTIONS
{
        /* Dummy load address */
        . = 0x10000;
        __LOW_MEMORY = .;
        __START = .;
        __TEXT_START = .;
        .text : ALIGN(4096) {
                KEEP(*(.text.boot))
                *(.text)
        }
        __TEXT_END = .;
        
        .rela : ALIGN(8) {
                __RELA_START = .;
                *(.rela .rela*)
                __RELA_END = .;
        }
        
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
        __BSS_END = .;

        . = ALIGN(128);
        . += 4096;
        __kernel_init_stack = .;
        __END = .;
}
