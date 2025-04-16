/* TauOS boot entrypoint */
.section ".text.boot"

.global _entry

#define R_AARCH64_RELATIVE 1027

/* Linux compatible image header */
/* 
 * All the instructions until we patch
 * .rela.dyn must be PC relative
 * otherwise we depend on the load address
 * supplied at link time  
 */
_entry:
    /* Jump instruction, pc relative */
    b _start
    .word 0
    /* Text offset */
    .dword 0
    /* Image size */
    .dword 0
    /* flags */
    .dword 0
    /* reserved */
    .dword 0
    .dword 0 
    .dword 0
    /* magic */
    .word 0x644d5241
    /* reserved*/
    .word 0

_start:

    /* Save the address of the FDT */
    mov x23, x0
    /* Read core id and halt non primary cores */
    mrs x0, mpidr_el1
    and x0, x0, #3
    cbnz x0, hlt

    mrs x0, CurrentEL

    /* Perform kernel 'reloc' adjust relative addresses s.t they match the load address */
    /* x0, contains the address the kernel was loaded to  */
    /* x1, contains the address the kernel was linked to  */
    adr x0, __START
    ldr x1, k_lk_addr
    sub x0, x0, x1

    adr x2, __RELA_START
    adr x3, __RELA_END

reloc_loop:
    cmp x2, x3
    b.hs bss_setup
    /* Load two and go three forward, ptr always aligned */
    ldp x4, x5, [x2], #24
    /* Load the value we skipped */
    ldr x6, [x2, #-8]
    cmp x5, #R_AARCH64_RELATIVE
    b.ne reloc_loop
    /* Relocation here */
    add x6, x6, x0
    str x6, [x4, x0]
    b reloc_loop

bss_setup:
    /* Clear bss, BSS size is some multiple of 64bits */
    ldr x5, =__BSS_START
    ldr x6, =__BSS_END
    sub x6, x6, x5 // bss size in byte
    lsr x6, x6, #3 // bss size in 64 bits

bss_loop: 
    cbz x6, bss_loop_end
    str xzr, [x5], #8
    sub x6, x6, #1
    b bss_loop

bss_loop_end:
    /* Restore FDT and jump to the kernel */
    mov x0, x23
    ldr x1, =kinit
    ldr x2, =__kernel_init_stack
    mov sp, x2
    br x1

hlt:
    b hlt

k_lk_addr:
    .dword __START
