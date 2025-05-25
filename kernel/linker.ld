ENTRY(_entry)
SECTIONS
{
        /* Dummy load address */
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
                . += 4096;
                __kernel_init_stack = .;
                __idmap_blocks = .;
                . += 4096 * 3;
                __idmap_blocks_end = .;
                __IDMAP_DATA_END = .;
        } : data_segment

        __END = .;


}

PHDRS {
        text_segment PT_LOAD FLAGS(5);
        data_segment PT_LOAD FLAGS(6);
        rodata_segment PT_LOAD FLAGS(4);
}
