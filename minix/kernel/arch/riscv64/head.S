/*
 * RISC-V 64 kernel entry point
 *
 * Entry from OpenSBI/bootloader:
 *   a0 = hart ID
 *   a1 = pointer to FDT (Flattened Device Tree)
 *
 * This code:
 * 1. Sets up initial stack
 * 2. Stores boot info
 * 3. Sets up initial page tables
 * 4. Enables paging (Sv39)
 * 5. Jumps to C code (kernel_main)
 */

#include "include/sconst.h"
#include "include/archconst.h"

    .section .text.boot
    .globl _start
    .type _start, @function

_start:
    /* Disable interrupts */
    csrw    sie, zero
    csrw    sip, zero

    /* Save hart ID in tp register for later use */
    mv      tp, a0

    /* Only boot hart 0 continues, others wait */
    bnez    a0, secondary_hart_wait

    /* Save FDT pointer */
    la      t0, _boot_fdt
    sd      a1, 0(t0)

    /* Clear BSS section */
    la      t0, __bss_start
    la      t1, __bss_end
1:
    bgeu    t0, t1, 2f
    sd      zero, 0(t0)
    addi    t0, t0, 8
    j       1b
2:

    /* Set up boot stack (grows downward) */
    la      sp, _boot_stack_top

    /* Set up trap vector (direct mode) */
    la      t0, trap_entry
    csrw    stvec, t0

    /* Set sscratch to 0 (kernel mode indicator) */
    csrw    sscratch, zero

    /* Call early init to set up page tables */
    call    pg_early_init

    /* Enable Sv39 paging */
    la      t0, _boot_pgdir
    srli    t0, t0, 12              /* PPN = physical addr >> 12 */
    li      t1, SATP_MODE_SV39
    or      t0, t0, t1
    csrw    satp, t0
    sfence.vma                      /* Flush TLB */

    /* Jump to high address (virtual) */
    la      t0, kernel_main_high
    jr      t0

kernel_main_high:
    /* Reload stack pointer with virtual address */
    la      sp, _boot_stack_top

    /* Call kernel main */
    call    kernel_main

    /* Should not return */
halt:
    wfi
    j       halt

/*
 * Secondary hart wait loop
 * Wait for IPI from boot hart to start
 */
secondary_hart_wait:
    /* Enable software interrupts */
    li      t0, SIE_SSIE
    csrs    sie, t0
    csrsi   sstatus, SSTATUS_SIE

    /* Wait for interrupt */
1:
    wfi

    /* Check if we should start */
    la      t0, _secondary_start_flag
    ld      t0, 0(t0)
    beqz    t0, 1b

    /* Disable interrupts again */
    csrci   sstatus, SSTATUS_SIE
    csrw    sie, zero

    /* Jump to secondary hart entry */
    j       secondary_hart_entry

/*
 * Secondary hart entry point (called after SMP init)
 */
    .globl secondary_hart_entry
    .type secondary_hart_entry, @function
secondary_hart_entry:
    /* Get hart ID from tp */
    mv      a0, tp

    /* Calculate per-hart stack */
    la      sp, _secondary_stacks
    li      t0, STACK_SIZE
    mul     t1, a0, t0
    add     sp, sp, t1
    add     sp, sp, t0              /* Point to top of stack */

    /* Set up trap vector */
    la      t0, trap_entry
    csrw    stvec, t0

    /* Set sscratch to 0 */
    csrw    sscratch, zero

    /* Load page table */
    la      t0, _boot_pgdir
    srli    t0, t0, 12
    li      t1, SATP_MODE_SV39
    or      t0, t0, t1
    csrw    satp, t0
    sfence.vma

    /* Call secondary hart C entry */
#ifdef CONFIG_SMP
    call    smp_ap_entry
#else
    j       halt
#endif

    /* Should not return */
    j       halt

/*
 * Trap entry point
 * All traps (interrupts, exceptions, syscalls) come here
 */
    .globl trap_entry
    .type trap_entry, @function
    .align 4
trap_entry:
    /* Check if we came from user mode */
    csrrw   sp, sscratch, sp        /* Swap sp and sscratch */
    bnez    sp, trap_from_user      /* If sscratch was 0, we're from kernel */

    /* From kernel mode - restore sp */
    csrr    sp, sscratch

trap_from_kernel:
    /* Save registers on kernel stack */
    addi    sp, sp, -TRAPFRAME_SIZE

    sd      ra, TF_RA(sp)
    sd      gp, TF_GP(sp)
    sd      tp, TF_TP(sp)
    sd      t0, TF_T0(sp)
    sd      t1, TF_T1(sp)
    sd      t2, TF_T2(sp)
    sd      s0, TF_S0(sp)
    sd      s1, TF_S1(sp)
    sd      a0, TF_A0(sp)
    sd      a1, TF_A1(sp)
    sd      a2, TF_A2(sp)
    sd      a3, TF_A3(sp)
    sd      a4, TF_A4(sp)
    sd      a5, TF_A5(sp)
    sd      a6, TF_A6(sp)
    sd      a7, TF_A7(sp)
    sd      s2, TF_S2(sp)
    sd      s3, TF_S3(sp)
    sd      s4, TF_S4(sp)
    sd      s5, TF_S5(sp)
    sd      s6, TF_S6(sp)
    sd      s7, TF_S7(sp)
    sd      s8, TF_S8(sp)
    sd      s9, TF_S9(sp)
    sd      s10, TF_S10(sp)
    sd      s11, TF_S11(sp)
    sd      t3, TF_T3(sp)
    sd      t4, TF_T4(sp)
    sd      t5, TF_T5(sp)
    sd      t6, TF_T6(sp)

    /* Save original sp */
    addi    t0, sp, TRAPFRAME_SIZE
    sd      t0, TF_SP(sp)

    /* Save CSRs */
    csrr    t0, sepc
    sd      t0, TF_SEPC(sp)
    csrr    t0, sstatus
    sd      t0, TF_SSTATUS(sp)
    csrr    t0, scause
    sd      t0, TF_SCAUSE(sp)
    csrr    t0, stval
    sd      t0, TF_STVAL(sp)

    /* Call C handler */
    mv      a0, sp
    call    exception_handler

    /* Restore and return */
    j       trap_return

trap_from_user:
    /* sp now points to kernel stack (from sscratch) */
    /* User sp is in sscratch */

    /* Save registers on kernel stack */
    addi    sp, sp, -TRAPFRAME_SIZE

    sd      ra, TF_RA(sp)
    sd      gp, TF_GP(sp)
    sd      tp, TF_TP(sp)
    sd      t0, TF_T0(sp)
    sd      t1, TF_T1(sp)
    sd      t2, TF_T2(sp)
    sd      s0, TF_S0(sp)
    sd      s1, TF_S1(sp)
    sd      a0, TF_A0(sp)
    sd      a1, TF_A1(sp)
    sd      a2, TF_A2(sp)
    sd      a3, TF_A3(sp)
    sd      a4, TF_A4(sp)
    sd      a5, TF_A5(sp)
    sd      a6, TF_A6(sp)
    sd      a7, TF_A7(sp)
    sd      s2, TF_S2(sp)
    sd      s3, TF_S3(sp)
    sd      s4, TF_S4(sp)
    sd      s5, TF_S5(sp)
    sd      s6, TF_S6(sp)
    sd      s7, TF_S7(sp)
    sd      s8, TF_S8(sp)
    sd      s9, TF_S9(sp)
    sd      s10, TF_S10(sp)
    sd      s11, TF_S11(sp)
    sd      t3, TF_T3(sp)
    sd      t4, TF_T4(sp)
    sd      t5, TF_T5(sp)
    sd      t6, TF_T6(sp)

    /* Save user sp */
    csrr    t0, sscratch
    sd      t0, TF_SP(sp)

    /* Save CSRs */
    csrr    t0, sepc
    sd      t0, TF_SEPC(sp)
    csrr    t0, sstatus
    sd      t0, TF_SSTATUS(sp)
    csrr    t0, scause
    sd      t0, TF_SCAUSE(sp)
    csrr    t0, stval
    sd      t0, TF_STVAL(sp)

    /* Clear sscratch (we're in kernel mode now) */
    csrw    sscratch, zero

    /* Call C handler */
    mv      a0, sp
    call    exception_handler

    /* Fall through to trap_return */

/*
 * Return from trap
 * Restore registers and sret
 */
    .globl trap_return
    .type trap_return, @function
trap_return:
    /* Restore CSRs */
    ld      t0, TF_SEPC(sp)
    csrw    sepc, t0
    ld      t0, TF_SSTATUS(sp)
    csrw    sstatus, t0

    /* Check if returning to user mode */
    andi    t0, t0, SSTATUS_SPP
    bnez    t0, return_to_kernel

return_to_user:
    /* Save kernel sp in sscratch for next trap */
    addi    t0, sp, TRAPFRAME_SIZE
    csrw    sscratch, t0

    /* Restore user sp */
    ld      sp, TF_SP(sp)
    addi    sp, sp, -TRAPFRAME_SIZE  /* Point back to trapframe for restore */
    j       restore_regs

return_to_kernel:
    /* Restore registers and return */
restore_regs:
    ld      ra, TF_RA(sp)
    ld      gp, TF_GP(sp)
    ld      tp, TF_TP(sp)
    ld      t0, TF_T0(sp)
    ld      t1, TF_T1(sp)
    ld      t2, TF_T2(sp)
    ld      s0, TF_S0(sp)
    ld      s1, TF_S1(sp)
    ld      a0, TF_A0(sp)
    ld      a1, TF_A1(sp)
    ld      a2, TF_A2(sp)
    ld      a3, TF_A3(sp)
    ld      a4, TF_A4(sp)
    ld      a5, TF_A5(sp)
    ld      a6, TF_A6(sp)
    ld      a7, TF_A7(sp)
    ld      s2, TF_S2(sp)
    ld      s3, TF_S3(sp)
    ld      s4, TF_S4(sp)
    ld      s5, TF_S5(sp)
    ld      s6, TF_S6(sp)
    ld      s7, TF_S7(sp)
    ld      s8, TF_S8(sp)
    ld      s9, TF_S9(sp)
    ld      s10, TF_S10(sp)
    ld      s11, TF_S11(sp)
    ld      t3, TF_T3(sp)
    ld      t4, TF_T4(sp)
    ld      t5, TF_T5(sp)
    ld      t6, TF_T6(sp)

    /* Restore sp last */
    ld      sp, TF_SP(sp)

    sret

    .section .data
    .align 3
    .globl _boot_fdt
_boot_fdt:
    .quad 0

    .globl _secondary_start_flag
_secondary_start_flag:
    .quad 0

    .section .bss
    .align 12
    .globl _boot_pgdir
_boot_pgdir:
    .space 4096

    .align 12
    .globl _boot_stack
_boot_stack:
    .space STACK_SIZE
    .globl _boot_stack_top
_boot_stack_top:

    /* Secondary hart stacks (for SMP) */
    .align 12
    .globl _secondary_stacks
_secondary_stacks:
    .space STACK_SIZE * 16          /* Support up to 16 harts */
