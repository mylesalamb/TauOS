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
        
        /* Set the stack pointer just below the kernel */
        /* We can use this memory until we turn the mmu on */
        ldr x5, =__LOW_MEMORY
        mov sp, x5
        
        // We actually account for the case of
        // bss < 64 bits, as we align to page size
        // bss sizes -> [0,4kb,8kb...]
        ldr x5, =__BSS_START
        ldr x6, =__END
        sub x6, x6, x5 // bss size in byte
        lsr x6, x6, #3 // bss size in 64 bits
bss_loop: 
        cbz x6, bss_loop_end
        str xzr, [x5], #8
        sub x6, x6, #1
        b bss_loop

bss_loop_end:
        bl __mmu_bootstrap_tables
        bl kinit

// Secondary core, and main [if it returns] should halt 
core_hang:
        wfe
        b core_hang


/* TODO: refactor this absolute rubbish assembler */
/* Some functions to bootstrap a minimal set of tables that we can work with */
/* So that we can jump to C and map peripherals */
__mmu_alloc_term_entry:
        stp x0, x30, [sp, #-16]!

        ldp x0, x30, [sp], #16
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
        stp x0, x30, [sp, #-16]!
        stp x1, x2, [sp, #-16]!
        stp x3, xzr, [sp, #-16]!

        /* Index in the page table*/
        lsr x1, x1, x2
        and x1, x1, #0x1ff 

        /* Indicate that descriptor is valid and refers to a lower level table*/
        orr x3, x3, #3
        /* Store the pointer to next in the page table at byte x1 mul 8*/
        str x3, [x0, x1, lsl #3] 


        ldp x3, xzr, [sp], #16
        ldp x1, x2, [sp], #16
        ldp x0, x30, [sp], #16
        ret

__mmu_bootstrap_tables:
        stp x0, x30, [sp, #-16]!
        
        /* Prepare pgd */
        adrp x0, __pgd_start  
        adrp x1, __TEXT_START 
        mov x2, #39
        adrp x3, __pud_start
        bl __mmu_alloc_int_entry


        /* Prepare pud */
        mov x0, x3
        mov x2, #30
        adrp x3, __pmd_start
        bl __mmu_alloc_int_entry

        ldp x0, x30, [sp], #16
        ret
