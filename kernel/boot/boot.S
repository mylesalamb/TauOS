/* TauOS boot entrypoint */
.section ".text.boot"

#include "boot/sysregs.h"
#include "mm/pt.h"

.global _entry

#define R_AARCH64_RELATIVE 1027

#define MMU_PGD_FLAGS ( \
    MMU_DCR_TABLE   | \
    MMU_DCR_VALID )

#define MMU_PUD_FLAGS ( \
    MMU_DCR_ATTR_AP_RW_N    | \
    MMU_DCR_UXN             | \
    MMU_DCR_BLOCK           | \
    MMU_DCR_AF              | \
    MMU_DCR_VALID )

/* Linux compatible image header */
/* 
 * All the instructions until we patch
 * .rela.dyn must be PC relative
 * otherwise we depend on the load address
 * supplied at link time
 * Assumption that this symbol is placed at a 4kb boundary  
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

    /* TODO handle case if we didnt boot in el1 */
    mrs x0, CurrentEL
    lsr x0, x0, #2;

    cmp x0, #1
    b.ne hlt

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
    ldr x0, =__BSS_START
    ldr x1, =__BSS_END
    sub x1, x1, x0
    bl _b_memzero

    /* Clear idmap data */
    ldr x0, =__IDMAP_DATA_START
    ldr x1, =__IDMAP_DATA_END
    sub x1, x1, x0
    bl _b_memzero

    /* Prepare translation configuration, see boot/sysregs.h */
    ldr x0, =TCR_EL1_VAL
    msr tcr_el1, x0

    ldr x0, =MAIR_EL1_VAL
    msr mair_el1, x0

    /* Bootstrap a minimal identity mapping for preparing a higher half mapping */
    adrp x0, __idmap_blocks
    // adrp x1, __START
    ldr x1, =0x40000000

    msr ttbr0_el1, x0

    
    /* Creates a block map entry */
    /* x0: Address of the PGD -> PUD*/
    /* x1: Physical address of the start of the kernel */

    /* Create entry in PGD */
    lsr x2, x1, #MMU_PGD_SHIFT
    and x2, x2, #MMU_BLK_LEN - 1
    add x3, x0, #MMU_BLK_SIZE
    orr x3, x3, #MMU_PGD_FLAGS
    str x3, [x0, x2, lsl 3]
    
    add x0, x0, #MMU_BLK_SIZE
    /* Create entry in PUD */
    lsr x2, x1, #MMU_PUD_SHIFT
    and x2, x2, #MMU_BLK_LEN - 1
    ldr x3, =MMU_PUD_FLAGS | MMU_DCR_ATTR_NORM
    orr x3, x3, x1
    str x3, [x0, x2, lsl 3]

    ldr x1, =0x00000000
    lsr x2, x1, #MMU_PUD_SHIFT
    and x2, x2, #MMU_BLK_LEN - 1
    ldr x3, =MMU_PUD_FLAGS | MMU_DCR_ATTR_DEVICE
    orr x3, x3, x1
    str x3, [x0, x2, lsl 3]

    ldr x0, =SCTLR_EL1_VAL
    msr sctlr_el1, x0

    


    /* Restore FDT and jump to the kernel */
    mov x0, x23
    ldr x1, =kinit
    ldr x2, =__kernel_init_stack
    mov sp, x2
    br x1

/* Bytewise memzero */
_b_memzero:
    cbz x1, _b_memzero_ret
_b_memzero_loop:
    strb wzr, [x0], #1
    sub x1, x1, #1
    b _b_memzero
_b_memzero_ret:
    ret

hlt:
    b hlt

k_lk_addr:
    .dword __START
