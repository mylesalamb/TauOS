/* TauOS boot entrypoint */
.section ".text.boot"

#include "boot/sysregs.h"
#include "mm/pt.h"
#include "mm/addr.h"

.global _entry

#define R_AARCH64_RELATIVE 1027

#define MMU_TABLE_FLAGS (       \
    MMU_DCR_TABLE           |   \
    MMU_DCR_VALID               \
    )

#define MMU_LEAF_FLAGS (        \
    MMU_DCR_ATTR_AP_RW_N    |   \
    MMU_DCR_UXN             |   \
    MMU_DCR_PAGE            |   \
    MMU_DCR_AF              |   \
    MMU_DCR_VALID               \
)

/*
 * Macro to align an address up to the page size
 *
 * Arguments:
 *   address - The address to align
 *
 * Returns:
 *   The aligned address
 */
.macro page_align address
    add \address, \address, #MMU_BLK_SIZE - 1
    and \address, \address, -MMU_BLK_SIZE
.endm

/*
 * Macro to calculate MMU entry boundaries
 *
 * Arguments:
 *   vstart  - The address to start mapping from
 *   vend    - The address to map to
 *   vshift  - The shift to apply to calculate the relevant page table entry
 *   table   - The table that will be used to write entries to
 *
 * Returns:
 *   table - Offset to the address of the first entry
 *   count - The number of entries to write
 *   fwd   - Address of the next level of page tables, accounting for multiple blocks
 */
.macro compute_page_entries vstart, vend, vshift, table, count, fwd
    
    /* Count entries, fwd as temp */
    lsr \fwd, \vstart, \vshift
    
    sub \count, \vend, #1
    lsr \count, \count, \vshift
    sub \count, \count, \fwd
    add \count, \count, #1

    /* Starting entry address  */
    and \fwd, \fwd, #MMU_BLK_LEN - 1
    add \table, \table, \fwd, lsl #3

    /* fwd contains the index, need to round up to the next block */
    add \fwd, \fwd, \count
    add \fwd, \table, \fwd, lsl #3

    # Forwarding entry address
    page_align \fwd
.endm

/*
 * Macro to write page entries
 *
 * Arguments:
 *   table - Address to start writing page entries to
 *   fwd   - Destination address to forward to (either a physical address or next-level table)
 *   incr  - The number of bytes to increment the fwd address per entry (block maps etc)
 *   count - Number of entries to write
 *   flags - Flags to associate with each entry
 *   tmp1  - Scratch register
 *
 * Returns:
 *   table - Aligned up to the next page table boundary
 */
.macro write_page_entries table, fwd, incr, count, flags, tmp1
1:
    cmp \count, xzr
    b.eq 1f
    mov \tmp1, \fwd
    orr \tmp1, \tmp1, \flags
    str \tmp1, [\table], #8
    
    add \fwd, \fwd, \incr
    sub \count, \count, #1
    b 1b
1:
    page_align \table
.endm

/*
 * Linux compatible image header
 *
 * All the instructions until we patch .rela.dyn
 * must be PC-relative; otherwise, we depend on the link address.
 *
 * Assumption that this symbol is placed at a 4kb boundary.
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

    /* Disable IRQ,FIQ until we setup intc + vbar */
    ldr x0, =DAIF_VAL 
    msr daif, x0

    /* Perform kernel 'reloc' adjust relative addresses s.t they match the load address */
    /* x0, contains the address the kernel was loaded to  */
    /* x1, contains the address the kernel was linked to  */
    adr x0, __START
    ldr x1, =__START
    ldr x7, =KVADDR_BASE
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
    orr x6, x6, x7
    str x6, [x4, x0]
    b reloc_loop

bss_setup:
    /* Clear bss, BSS size is some multiple of 64bits */
    adr x0, __BSS_START
    adr x1, __BSS_END
    sub x1, x1, x0
    bl _b_memzero

    /* Clear idmap data */
    adr x0, __IDMAP_DATA_START
    adr x1, __IDMAP_DATA_END
    sub x1, x1, x0
    bl _b_memzero

    /* Prepare translation configuration, see boot/sysregs.h */
    ldr x0, =TCR_EL1_VAL
    msr tcr_el1, x0

    ldr x0, =MAIR_EL1_VAL
    msr mair_el1, x0

    /* Bootstrap a minimal identity mapping for preparing a higher half mapping */
    adrp x0, __idmap_blocks
    adrp x1, __START
    adrp x2, __END
    ldr x5, =MMU_BLK_SIZE
    ldr x7, =MMU_TABLE_FLAGS
    msr ttbr0_el1, x0

    compute_page_entries x1, x2, #MMU_PGD_SHIFT, x0, x3, x4
    write_page_entries x0, x4, x5, x3, x7, x6

    compute_page_entries x1, x2, #MMU_PUD_SHIFT, x0, x3, x4
    write_page_entries x0, x4, x5, x3, x7, x6

    compute_page_entries x1, x2, #MMU_PMD_SHIFT, x0, x3, x4
    write_page_entries x0, x4, x5, x3, x7, x6

    compute_page_entries x1, x2, #MMU_PTE_SHIFT, x0, x3, x4
    adrp x4, __START
    ldr x7, =MMU_LEAF_FLAGS
    write_page_entries x0, x4, x5, x3, x7, x6

    dsb sy
    ldr x0, =SCTLR_EL1_VAL
    msr sctlr_el1, x0
    isb

    /* Restore FDT and jump to the kernel init */
    mov x0, x23
    adr x1, kinit
    adr x2, __kernel_init_stack
    mov sp, x2
    blr x1

    /* Higher half mappings are now usable */
    ldr x0, =KVADDR_BASE
    adr x1, __kernel_init_stack
    orr x0, x0, x1
    mov sp, x0

    mov x0, x23
    ldr x1, =kstart
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

/* Place to stop where early boot failures occur */
hlt:
    wfe
    b hlt
