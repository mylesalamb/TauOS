.section ".text.boot"

/*
TauOS boot entrypoint
*/

#define BIT(x) (1 << x)

#define SPSR_EL2_EL1H (0b0101)
#define SPSR_EL2_IMASK (0xf << 6)

#define SPSR_EL2_VAL  (SPSR_EL2_EL1H | SPSR_EL2_IMASK)

#define HCR_EL2_RW  BIT(31)
#define HCR_EL2_SWIO BIT(1)
#define HCR_EL2_VAL   (HCR_EL2_SWIO | HCR_EL2_RW )


#define SCTLR_EL1_SA0 BIT(4)
#define SCTLR_EL1_SA BIT(3)
#define SCTLR_EL1_A BIT(1)
#define SCTLR_EL1_MMU_ENABLE BIT(0)
#define SCTLR_EL1_VAL ((3 << 28) | (3 << 22) | (1 << 20) | (1 << 11) )

/* Compile with -mgeneral-regs-only */
#define CPACR_EL1_SIMD (3 << 20)
#define CPACR_EL1_VAL CPACR_EL1_SIMD

/* MMU regs and flags */
#define MAIR_NORM_IDX   0x0
#define MAIR_DEVICE_IDX 0x1
#define MAIR_NORM_VALUE 0x44
#define MAIR_DEVICE_VALUE 0x00
#define MAIR_EL1_VAL (MAIR_NORM_VALUE << (8 * MAIR_NORM_IDX)) | (MAIR_DEVICE_VALUE << (8 * MAIR_DEVICE_IDX))

#define TCR_EL1_T0SZ 0x10
#define TCR_EL1_T1SZ (0x10 << 16)
#define TCR_EL1_TG1 (0x2 << 30)
#define TCR_EL1_VAL (TCR_EL1_TG1 | TCR_EL1_T1SZ | TCR_EL1_T0SZ )

#define MMU_DESCR_ACCESS BIT(10)
#define MMU_DESCR_VALID BIT(0)
#define MMU_DESCR_INT  BIT(1)
#define MMU_DESCR_MAIR_SHIFT 2
#define MMU_DESCR_NS    BIT(2)
#define MMU_DESCR_EL1_RWX BIT(54)

#define MMU_TABLE_FLAGS (MMU_DESCR_VALID | MMU_DESCR_INT )
#define MMU_K_BLK_FLAGS (MMU_DESCR_VALID | MMU_DESCR_NS | MMU_DESCR_ACCESS | MMU_DESCR_EL1_RWX | (MAIR_NORM_IDX   << 2))
#define MMU_K_DEV_FLAGS (MMU_DESCR_VALID | MMU_DESCR_NS | MMU_DESCR_ACCESS | MMU_DESCR_EL1_RWX | (MAIR_DEVICE_IDX << 2))


.globl _start
_start:
        // Set spsr
        ldr x5, =SPSR_EL2_VAL
        msr SPSR_EL2, x5
        // Set HCR
        ldr x5, =HCR_EL2_VAL
        msr HCR_EL2, x5

        // Set sctl (kernel mode)
        ldr x5, =SCTLR_EL1_VAL
        msr SCTLR_EL1, x5
        
        ldr x5, =CPACR_EL1_VAL
        msr CPACR_EL1, x5

        // drop to kernel mode
        adr x5, el1_entry
        msr ELR_EL2, x5
        eret

el1_entry:

        mrs x5, mpidr_el1
        and x5, x5, #3
        cbnz x5, core_hang
        
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
        bl __mmu_bootstrap_tables

        ldr x1, =MAIR_EL1_VAL
        msr mair_el1, x1

        ldr x1, =TCR_EL1_VAL
        msr tcr_el1, x1

        adrp x1, __init_pgd
        msr ttbr0_el1, x1
        adrp x1, __pgd_start
        msr ttbr1_el1, x1

        ldr x3, =kinit
        ldr x1, =__kernel_init_stack
        mov sp, x1

        mov x1, #SCTLR_EL1_MMU_ENABLE
        mrs x2, sctlr_el1
        orr x1, x1, x2
        msr sctlr_el1, x1

        blr x3

// Secondary core, and main [if it returns] should halt 
core_hang:
        wfe
        b core_hang


/* TODO: refactor this absolute rubbish assembler */
/* Some functions to bootstrap a minimal set of tables that we can work with */
/* So that we can jump to C and map peripherals */
/*
 * x0: physical address of the page table
 * x1: physical address that we are mapping
 * x2: start of the virtual region that we are mapping
 * x3: end of the virtual region that we are mapping
 * x4: flags to be added to the lower attributes of the entries
 * x5: step size, number of bytes encompassed in a singular entry
 * x6: shift to apply to the address to get the index
 */
__mmu_alloc_term_entry:
        
        /* vaddr that we are mapping as index*/
        lsr x2, x2, x6
        and x2, x2, #0x1ff

        /* End index */
        lsr x3, x3, x6
        and x3, x3, #0x1ff

        /* Shift of the low bits from the phys addr, could use and here instead*/
        lsr x1, x1, x6
        lsl x1, x1, x6
        orr x1, x1, x4

        
__alloc_loop:
        str x1, [x0, x2, lsl #3]
        add x2, x2, #1
        add x1, x1, x5
        cmp x2, x3
        b.ls __alloc_loop
        
        ret

/*
 * Allocate an entry in an intermediate / non-leaf level in the page table
 * arguments:
 * r0: physical address of the page table
 * r1: virtual address that we are mapping
 * r2: shift to apply to the virtual address to get the index 
 * r3: address of the next page table
 */
__mmu_alloc_int_entry:

        /* Index in the page table*/
        lsr x1, x1, x2
        and x1, x1, #0x1ff 

        /* Indicate that descriptor is valid and refers to a lower level table*/
        orr x3, x3, x4
        /* Store the pointer to next in the page table at byte x1 mul 8*/
        str x3, [x0, x1, lsl #3] 

        ret

/* So we do two things here, we bootstrap */
/* a basic identity mapping for the lower half, so that we can do a jump to the higher half */
/* We do the identity map, with a l2 / 1gb block descriptor */
/* We do the higher half mapping with 4kb block descriptors*/
/* Why this way? our kernel expects a load address of 0x80000*/
/* Our lower half identity map must preserve this */
/* and we want to load our higher half part from 0xffff0000....*/
__mmu_bootstrap_tables:
        mov x29, x30
        
        /* Prepare init pgd */
        adrp x0, __init_pgd
        adrp x1, __TEXT_START
        mov x2, #30
        adrp x3, __init_pud
        ldr x4, =MMU_TABLE_FLAGS
        bl __mmu_alloc_int_entry


        /* Prepare 1gb block mapping */
        adr x0, __init_pud
        mov x1, #0
        mov x2, #0
        mov x3, 1
        lsr x3, x3, #30 //40000000
        ldr x4, =MMU_K_BLK_FLAGS
        ldr x5, =(1024 * 1024 * 1024)
        ldr x6, =30
        bl __mmu_alloc_term_entry


        /* Prepare pgd */
        adrp x0, __pgd_start  
        ldr x1, =__TEXT_START 
        mov x2, #39
        adrp x3, __pud_start
        ldr x4, =MMU_TABLE_FLAGS
        bl __mmu_alloc_int_entry


        /* Prepare pud */

        adrp x0, __pud_start
        ldr x1, =__TEXT_START 
        mov x2, #30
        adrp x3, __pmd_start
        ldr x4, =MMU_TABLE_FLAGS
        bl __mmu_alloc_int_entry
        
        /* prepare pmd*/
        adrp x0, __pmd_start
        ldr x1, =__TEXT_START 
        mov x2, #21
        adrp x3, __pte_start
        ldr x4, =MMU_TABLE_FLAGS
        bl __mmu_alloc_int_entry
        
        /* 4kb descriptors are weird and need an extra bit? */
        adrp x0, __pte_start
        mov x1, #0x80000
        ldr x2, =__TEXT_START
        ldr x3, =__END
        ldr x4, =(MMU_K_BLK_FLAGS | MMU_DESCR_INT)
        ldr x5, =(1024 * 4 )
        ldr x6, =12
        bl __mmu_alloc_term_entry

        /* Remap the peripheral base address */
        adrp x0, __pmd_start
        mov x1, #0xfe000000
        ldr x2, =0x400000
        ldr x3, =0x600000
        ldr x4, =MMU_K_DEV_FLAGS
        ldr x5, =(1024 * 1024 * 2)
        ldr x6, =21

        bl __mmu_alloc_term_entry
        mov x30, x29
        ret
