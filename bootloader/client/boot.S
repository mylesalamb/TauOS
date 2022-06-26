.section ".text.boot"

/*
TauOS bootloader entrypoint 
*/

.globl _start
_start:
        /* move armstub arguments somewhere */
        /* GCC doesnt appear to use high resgisters*/

        mov x10, x0
        mov x11, x1
        mov x12, x2
        mov x13, x3

        ldr x0, =0x80000
        ldr x1, =__TEXT_START
        ldr x2, =__END
        sub x2, x2, x1
        lsr x2, x2, #3

relocate_loop:
        ldr x4, [x0], #8
        str x4, [x1], #8
        sub x2, x2, #1
        cbnz x2, relocate_loop

        /* Absolute jump to the relocated boot stub */
        ldr x30, =post_relocate
        ret

post_relocate:
        
        /* Clear out BSS segment */
        ldr x0, =__BSS_START
        ldr x1, =__BSS_END
        sub x1, x1, x0
        lsr x1, x1, #3

bss_loop:
        cbz x1, bss_loop_end
        str xzr, [x0], #8
        sub x1, x1, #1
        b bss_loop
bss_loop_end:

        ldr x0, =__LOW_MEMORY
        mov sp, x0

        /* Relative branch as we jumped to the relocated code */
        bl bmain
        bl bother
        mov x0, x10
        mov x1, x11
        mov x2, x12
        mov x3, x13

        /* Absolute jump to loaded kernel start */
        ldr x30, =0x80000
        ret

