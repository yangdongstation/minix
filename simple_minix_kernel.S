/* Simple Minix RISC-V Kernel Test */

.section .text.boot
.global _start

_start:
    # Set stack
    li      sp, 0x80400000
    
    # Print 'M' to show we're alive
    li      t0, 0x10000000
    li      t1, 0x4D        # 'M'
    sb      t1, 0(t0)
    
    # Print 'i'
    li      t1, 0x69        # 'i'
    sb      t1, 0(t0)
    
    # Print 'n'
    li      t1, 0x6E        # 'n'
    sb      t1, 0(t0)
    
    # Print 'i'
    li      t1, 0x69        # 'i'
    sb      t1, 0(t0)
    
    # Print 'x'
    li      t1, 0x78        # 'x'
    sb      t1, 0(t0)
    
    # Print newline
    li      t1, 0x0A        # '\n'
    sb      t1, 0(t0)
    
    # Simple delay
    li      t2, 1000000
delay:
    addi    t2, t2, -1
    bnez    t2, delay
    
    # Halt
    li      a7, 0x53525354  # SBI_SHUTDOWN
    li      a6, 0           # SHUTDOWN_TYPE_SHUTDOWN
    li      a0, 0           # reason
    ecall
    
    # Fallback
halt:
    j       halt