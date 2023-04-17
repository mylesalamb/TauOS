.section ".text.boot"
/* TauOS boot entrypoint for qemu-virt board */
.globl _start
_start:
        //mrs x5, mpidr_el1
        //and x5, x5, #3
        //cbnz x5, core_hang
        
        // We actually account for the case of
        // bss < 64 bits, as we align to page size
        // bss sizes -> [0,4kb,8kb...]
        adr x5, __BSS_START
        adr x6, __END
        sub x6, x6, x5 // bss size in byte
        lsr x6, x6, #3 // bss size in 64 bits
bss_loop: 
        cbz x6, bss_loop_end
        str xzr, [x5], #8
        sub x6, x6, #1
        b bss_loop

bss_loop_end:
        ldr x3, =qemu_kinit
        ldr x1, =__kernel_init_stack
        mov sp, x1

        blr x3

// Secondary core, and main [if it returns] should halt 
core_hang:
        wfe
        b core_hang
