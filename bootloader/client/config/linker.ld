ENTRY(_start)
SECTIONS
{
        . = 0x80000 - 2048;
        __LOW_MEMORY = .;
        __START = .;
        __TEXT_START = .;
        .text : {
                KEEP(*(.text.boot))
                *(.text)
        }
        . = ALIGN(8);
        __TEXT_END = .;
        __RODATA_START = .;
        .rodata :
        {
                *(.rodata)
        }
        . = ALIGN(8);
        __RODATA_END = .;
 
        __DATA_START = .;
        .data :
        {
                *(.data*)
        }
        . = ALIGN(8);
        __DATA_END = .;
 
        __BSS_START = .;
        .bss :
        {
                *(.bss*)
        }
        . = ALIGN(8); 
        __BSS_END = .;
        __END = .;
}
