ENTRY(_entry)
SECTIONS
{
        /* Dummy load address */
        . = 0x10000;
        __LOW_MEMORY = .;
        __START = .;
        .text : ALIGN(4096) {
        	__TEXT_START = .;
                KEEP(*(.text.boot))
                *(.text)
        	__TEXT_END = .;
        }
        
        .rela : ALIGN(8) {
                __RELA_START = .;
                *(.rela .rela*)
                __RELA_END = .;
        }
        
        .rodata : ALIGN(4096)
        {
        	__RODATA_START = .;
                *(.rodata)
        	__RODATA_END = .;
        }
 
        .data : ALIGN(4096)
        {
        	__DATA_START = .;
                *(.data)
        	__DATA_END = .;
        }
        .bss : ALIGN(4096)
        {
        	__BSS_START = .;
                *(.bss)
        	__BSS_END = .;
        }

        . = ALIGN(128);
        . += 4096;
        __kernel_init_stack = .;
        __END = .;
}
