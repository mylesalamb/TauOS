.section ".text"

.globl irq_init_vectors
.globl irq_enable
.globl irq_disable

.macro irq_entry handle
        b \handle
.balign 0x80
.endm

// Store register context 
// Leaving a ptr to struct to register context in x0
// This should be an expected argument for each type of reciever that we define
// in the higher level C functions
.macro irq_store_ctx
        sub sp,  sp,  #(18 * 8 * 2) // 31 general purpose + EL1 config registers
        stp x0,  x1,  [sp, #(0 * 8 * 2)]
        stp x2,  x3,  [sp, #(1 * 8 * 2)]
        stp x4,  x5,  [sp, #(2 * 8 * 2)]
        stp x6,  x7,  [sp, #(3 * 8 * 2)]
        stp x8,  x9,  [sp, #(4 * 8 * 2)]
        stp x10, x11, [sp, #(5 * 8 * 2)]
        stp x12, x13, [sp, #(6 * 8 * 2)]
        stp x14, x15, [sp, #(7 * 8 * 2)]
        stp x16, x17, [sp, #(8 * 8 * 2)]
        stp x18, x19, [sp, #(9 * 8 * 2)]
        stp x20, x21, [sp, #(10 * 8 * 2)]
        stp x22, x23, [sp, #(11 * 8 * 2)]
        stp x24, x25, [sp, #(12 * 8 * 2)]
        stp x26, x27, [sp, #(13 * 8 * 2)]
        stp x28, x29, [sp, #(14 * 8 * 2)]

        mrs x0, ESR_EL1
        mrs x1, FAR_EL1
        mrs x2, SPSR_EL1
        mrs x3, ELR_EL1

        stp x30, x0,  [sp, #(15 * 8 * 2)]
        stp x1,  x2,  [sp, #(16 * 8 * 2)]
        stp x3,  xzr, [sp, #(17 * 8 * 2)]
        mov x0, sp

.endm


irq_restore_ctx:
        ldp x0,  x1,  [sp, #(0 * 8 * 2)]
        ldp x2,  x3,  [sp, #(1 * 8 * 2)]
        ldp x4,  x5,  [sp, #(2 * 8 * 2)]
        ldp x6,  x7,  [sp, #(3 * 8 * 2)]
        ldp x8,  x9,  [sp, #(4 * 8 * 2)]
        ldp x10, x11, [sp, #(5 * 8 * 2)]
        ldp x12, x13, [sp, #(6 * 8 * 2)]
        ldp x14, x15, [sp, #(7 * 8 * 2)]
        ldp x16, x17, [sp, #(8 * 8 * 2)]
        ldp x18, x19, [sp, #(9 * 8 * 2)]
        ldp x20, x21, [sp, #(10 * 8 * 2)]
        ldp x22, x23, [sp, #(11 * 8 * 2)]
        ldp x24, x25, [sp, #(12 * 8 * 2)]
        ldp x26, x27, [sp, #(13 * 8 * 2)]
        ldp x28, x29, [sp, #(14 * 8 * 2)]

        add sp,  sp,  #(18 * 8 * 2) // 31 general purpose + EL1 config registers
        eret

irq_enable:
        msr DAIFClr, #0xf
        ret

irq_disable:
        msr DAIFSet, #0xf
        ret

irq_init_vectors:
        adr x0, _irq_table
        msr VBAR_EL1, x0
        ret


// For each type of interrupt that occurs
// define a handover to a higher level function
// to handle the event with the irq_ctx
.macro irq_block rcver
        irq_store_ctx
        bl \rcver
        b irq_restore_ctx
.endm

se_stub:
        irq_block se_rcver

irq_stub:
        irq_block irq_rcver

fiq_stub:
        irq_block fiq_rcver

ser_stub:
        irq_block ser_rcver

// Undefined stub, for things
// I havent implemented yet
ud_stub:
        irq_block ud_rcver

.balign 2048
_irq_table:
        // Current exception level using EL0 stack pointer
        irq_entry ud_stub // se
        irq_entry ud_stub // irq 
        irq_entry ud_stub // fiq 
        irq_entry ud_stub // ser 

        // Current exception level, using ELx stack pointer
        irq_entry se_stub // se  
        irq_entry irq_stub // irq 
        irq_entry fiq_stub // fiq 
        irq_entry ser_stub // ser 

        // Lower exception level in aarch64
        irq_entry ud_stub // se  
        irq_entry ud_stub // irq 
        irq_entry ud_stub // fiq 
        irq_entry ud_stub // ser 

        // Lower exception level in aarch32
        irq_entry ud_stub // se  
        irq_entry ud_stub // irq 
        irq_entry ud_stub // fiq 
        irq_entry ud_stub // ser 

