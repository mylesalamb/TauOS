ENTRY(_start)
SECTIONS
{
        . = 0x80000;
        __LOW_MEMORY = .;
        __START = .;
        __TEXT_START = .;
        .text : {
                KEEP(*(.text.boot))
                *(.text)
        }
        . = ALIGN(4096);
        __TEXT_END = .;
        __RODATA_START = .;
        .rodata :
        {
                *(.rodata)
        }
        . = ALIGN(4096);
        __RODATA_END = .;
 
        __DATA_START = .;
        .data :
        {
                *(.data*)
        }
        . = ALIGN(4096);
        __DATA_END = .;
 
        __BSS_START = .;
        .bss :
        {
                *(.bss*)
        }
        . = ALIGN(4096); 
        __BSS_END = .;
        __END = .;
}
