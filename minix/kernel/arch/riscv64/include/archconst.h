/*
 * RISC-V 64 architecture constants for MINIX kernel
 */

#ifndef _RISCV64_KERNEL_ARCHCONST_H
#define _RISCV64_KERNEL_ARCHCONST_H

/* System clock frequency */
#ifndef DEFAULT_HZ
#define DEFAULT_HZ          1000
#endif

/* Page size */
#define RISCV_PAGE_SIZE     4096
#define RISCV_PAGE_SHIFT    12
#define RISCV_PAGE_MASK     (~(RISCV_PAGE_SIZE - 1))

/* Sv39 paging mode (3-level page table, 39-bit virtual address) */
#define RISCV_LEVELS        3
#define RISCV_PTES_PER_PT   512
#define RISCV_PTE_SHIFT     9

/* Sv39 page table entry flags */
#define RISCV_PTE_V         (1UL << 0)  /* Valid */
#define RISCV_PTE_R         (1UL << 1)  /* Readable */
#define RISCV_PTE_W         (1UL << 2)  /* Writable */
#define RISCV_PTE_X         (1UL << 3)  /* Executable */
#define RISCV_PTE_U         (1UL << 4)  /* User accessible */
#define RISCV_PTE_G         (1UL << 5)  /* Global */
#define RISCV_PTE_A         (1UL << 6)  /* Accessed */
#define RISCV_PTE_D         (1UL << 7)  /* Dirty */

/* Sv39 SATP mode */
#define SATP_MODE_SV39      (8UL << 60)
#define SATP_MODE_SV48      (9UL << 60)
#define SATP_PPN_MASK       ((1ULL << 44) - 1)

/* Virtual address space layout for Sv39 */
#define KERNEL_BASE         0xFFFFFFC000000000UL  /* -256GB */
#define KERNEL_SIZE         0x0000004000000000UL  /* 256GB */
#define USER_BASE           0x0000000000000000UL
#define USER_MAX            0x0000004000000000UL  /* 256GB */

/* QEMU virt platform memory map */
#define VIRT_DRAM_BASE      0x80000000UL          /* RAM starts here */
#define VIRT_UART0_BASE     0x10000000UL          /* UART */
#define VIRT_UART0_SIZE     0x100
#define VIRT_PLIC_BASE      0x0C000000UL          /* PLIC */
#define VIRT_PLIC_SIZE      0x4000000
#define VIRT_CLINT_BASE     0x02000000UL          /* CLINT (timer/IPI) */
#define VIRT_CLINT_SIZE     0x10000
#define VIRT_VIRTIO_BASE    0x10001000UL          /* VirtIO MMIO */
#define VIRT_VIRTIO_SIZE    0x1000
#define VIRT_VIRTIO_COUNT   8                     /* 8 VirtIO slots */

/* PLIC constants */
#define PLIC_NUM_SOURCES    1024
#define PLIC_NUM_PRIORITIES 7

/* Interrupt numbers for QEMU virt */
#define IRQ_UART0           10
#define IRQ_VIRTIO_START    1   /* VirtIO devices start at IRQ 1 */
#define IRQ_VIRTIO_END      8

/* RISC-V CSR bits */

/* sstatus bits */
#define SSTATUS_SIE         (1UL << 1)   /* Supervisor Interrupt Enable */
#define SSTATUS_SPIE        (1UL << 5)   /* Previous Interrupt Enable */
#define SSTATUS_SPP         (1UL << 8)   /* Previous Privilege (0=U, 1=S) */
#define SSTATUS_FS          (3UL << 13)  /* FPU State */
#define SSTATUS_FS_MASK     SSTATUS_FS
#define SSTATUS_FS_OFF      (0UL << 13)
#define SSTATUS_FS_INITIAL  (1UL << 13)
#define SSTATUS_FS_CLEAN    (2UL << 13)
#define SSTATUS_FS_DIRTY    (3UL << 13)
#define SSTATUS_SUM         (1UL << 18)  /* Supervisor User Memory access */

/* sie/sip bits */
#define SIE_SSIE            (1UL << 1)   /* Software Interrupt Enable */
#define SIE_STIE            (1UL << 5)   /* Timer Interrupt Enable */
#define SIE_SEIE            (1UL << 9)   /* External Interrupt Enable */

#define SIP_SSIP            (1UL << 1)   /* Software Interrupt Pending */
#define SIP_STIP            (1UL << 5)   /* Timer Interrupt Pending */
#define SIP_SEIP            (1UL << 9)   /* External Interrupt Pending */

/* scause values */
#define CAUSE_INTERRUPT     (1UL << 63)  /* Interrupt bit */
#define CAUSE_MASK          (~CAUSE_INTERRUPT)

/* Exception causes */
#define EXC_INST_MISALIGNED     0
#define EXC_INST_ACCESS         1
#define EXC_ILLEGAL_INST        2
#define EXC_BREAKPOINT          3
#define EXC_LOAD_MISALIGNED     4
#define EXC_LOAD_ACCESS         5
#define EXC_STORE_MISALIGNED    6
#define EXC_STORE_ACCESS        7
#define EXC_ECALL_U             8
#define EXC_ECALL_S             9
#define EXC_ECALL_M             11
#define EXC_INST_PAGE_FAULT     12
#define EXC_LOAD_PAGE_FAULT     13
#define EXC_STORE_PAGE_FAULT    15

/* Interrupt causes (with CAUSE_INTERRUPT bit set) */
#define IRQ_S_SOFT              1
#define IRQ_S_TIMER             5
#define IRQ_S_EXT               9

/* Clock frequency for QEMU virt (10 MHz) */
#ifndef CLOCK_FREQ
#define CLOCK_FREQ              10000000UL
#endif

/* Maximum number of CPUs supported */
#ifndef CONFIG_MAX_CPUS
#define CONFIG_MAX_CPUS         16
#endif

/* Stack sizes */
#define K_STACK_SIZE            4096
#define K_STACK_GUARD           4096

#define PG_ALLOCATEME           ((phys_bytes)-1)

#endif /* _RISCV64_KERNEL_ARCHCONST_H */
