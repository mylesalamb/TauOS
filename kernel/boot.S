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
#define SCTLR_EL1_VAL (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11) 

// I should probably disable si
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
        
        // Set the stack pointer
        // mov can load an 8 bit immediate
        // and we cant use ldr directly with sp
        ldr x5, =__LOW_MEMORY
        mov sp, x5
        
        // We actually account for the case of
        // bss < 64 bits, as we align to page size
        // bss sizes -> [0,4kb,8kb...]
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
        bl kinit

// Secondary core, and main [if it returns] should halt 
core_hang:
        wfe
        b core_hang
