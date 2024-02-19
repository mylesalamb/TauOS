/* TauOS boot entrypoint */
.section ".text.boot"

.global _start
_start:
    ldr x0, =0x40080000
    ldr x1, =kinit
    ldr x2, =__kernel_init_stack
    mov sp, x2
    
    blr x1
hlt:
    b hlt