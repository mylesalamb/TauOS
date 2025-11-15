.section ".text"

.macro exc_store_ctx
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

.macro exc_entry fwd
    b \fwd
    .balign 0x80
.endm

.macro recv type fwd

\type\():
    exc_store_ctx
    bl \fwd
    b exc_restore_ctx
.endm

exc_restore_ctx:
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

recv se_recv exc_recv_se
recv irq_recv exc_recv_irq
recv fiq_recv exc_recv_fiq
recv ser_recv exc_recv_ser
recv udf_recv exc_recv_udf

.balign 2048
_exc_table:
    /* Exception from current level using SP_EL0 */
    exc_entry  se_recv
    exc_entry  irq_recv
    exc_entry  fiq_recv
    exc_entry  ser_recv
    /* Exception from current level using SP_ELx */
    exc_entry  se_recv
    exc_entry  irq_recv
    exc_entry  fiq_recv
    exc_entry  ser_recv
    /* Lower execption level under aarch64 */
    exc_entry  udf_recv
    exc_entry  udf_recv
    exc_entry  udf_recv
    exc_entry  udf_recv
    /* Lower execption level under aarch32 */
    exc_entry  udf_recv
    exc_entry  udf_recv
    exc_entry  udf_recv
    exc_entry  udf_recv

/**
    Initializes the vector base address
*/
.global exc_init
exc_init:
    ldr x0, =_exc_table
    msr vbar_el1, x0
    msr daif, xzr
    isb
    ret
