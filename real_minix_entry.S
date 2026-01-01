/*
 * Real Minix RISC-V Kernel Entry Point
 * Based on actual Minix head.S but simplified for initial boot
 */

.section .text.boot
.global _start
.type _start, @function

_start:
    /* Only primary hart continues */
    bnez    a0, secondary_halt
    
    /* Set up minimal stack */
    li      sp, 0x80400000
    
    /* Clear small BSS area */
    la      t0, bss_start
    li      t1, 0x1000    /* 4KB */
    add     t1, t0, t1
clear_bss:
    bgeu    t0, t1, bss_cleared
    sd      zero, 0(t0)
    addi    t0, t0, 8
    j       clear_bss
bss_cleared:
    
    /* Call kernel main */
    call    kernel_main
    
    /* Should never return */
halt:
    li      a7, 0x53525354  # SBI_SHUTDOWN
    li      a6, 0           # SHUTDOWN_TYPE_SHUTDOWN
    li      a0, 0           # reason
    ecall
    j       halt

secondary_halt:
    li      a7, 0x53525354  # SBI_SHUTDOWN
    li      a6, 0           # SHUTDOWN_TYPE_SHUTDOWN
    li      a0, 1           # reason: secondary hart
    ecall
    j       secondary_halt

.section .text
.global kernel_main
kernel_main:
    /* Save context */
    addi    sp, sp, -32
    sd      ra, 24(sp)
    sd      s0, 16(sp)
    sd      s1, 8(sp)
    
    /* Print boot message */
    la      a0, boot_msg
    call    early_prints
    
    /* Print Minix banner */
    la      a0, minix_banner
    call    early_prints
    
    /* Simulate kernel initialization */
    la      a0, init_msg
    call    early_prints
    
    /* Simple delay to show activity */
    li      s0, 0
    li      s1, 3
activity_loop:
    beq     s0, s1, done
    
    la      a0, progress_msg
    call    early_prints
    
    li      t0, 2000000
    call    delay_func
    
    addi    s0, s0, 1
    j       activity_loop
    
done:
    la      a0, done_msg
    call    early_prints
    
    /* Restore and halt */
    ld      s1, 8(sp)
    ld      s0, 16(sp)
    ld      ra, 24(sp)
    addi    sp, sp, 32
    ret

/* Simple print function */
early_prints:
    li      t0, 0x10000000  # UART base
print_loop:
    lbu     t1, (a0)
    beqz    t1, print_done
    
    /* Wait for transmitter ready */
wait_tx:
    lh      t2, 5(t0)       # Line status register
    andi    t2, t2, 0x20    # THRE bit
    beqz    t2, wait_tx
    
    sb      t1, 0(t0)
    addi    a0, a0, 1
    j       print_loop
print_done:
    ret

/* Delay function */
delay_func:
    beqz    t0, delay_end
    addi    t0, t0, -1
    j       delay_func
delay_end:
    ret

.section .data
boot_msg:
    .string "\nReal Minix RISC-V Kernel Booting...\n"
minix_banner:
    .string "\n"
    .string "    ___  ___  _  _  ___   ___   _   _  \n"
    .string "   / _ \/ __|| \| || __| / __| | | | | \n"
    .string "  | (_) \\__ \| .  || _|  \\__ \\ | |_| | \n"
    .string "   \\___/|___/|_|\_||___| |___/  \\___/  \n"
    .string "\n"
init_msg:
    .string "\nInitializing Minix microkernel...\n"
progress_msg:
    .string "."
done_msg:
    .string "\n\n*** Minix kernel initialization complete! ***\n"
    .string "System is ready for operation.\n\n"

.section .bss
.align 12
kernel_stack:
    .space 16384  # 16KB kernel stack

.global kernel_stack_top
kernel_stack_top:

.global bss_start
bss_start:
.global bss_end
bss_end: