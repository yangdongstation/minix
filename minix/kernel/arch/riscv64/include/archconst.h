/*
 * RISC-V 64 architecture constants for MINIX kernel
 */

#ifndef _RISCV64_KERNEL_ARCHCONST_H
#define _RISCV64_KERNEL_ARCHCONST_H

#ifdef __ASSEMBLY__
#define RISCV_UL(x)         x
#define RISCV_ULL(x)        x
#else
#define RISCV_UL(x)         x##UL
#define RISCV_ULL(x)        x##ULL
#endif

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
#define RISCV_PTE_V         (RISCV_UL(1) << 0)  /* Valid */
#define RISCV_PTE_R         (RISCV_UL(1) << 1)  /* Readable */
#define RISCV_PTE_W         (RISCV_UL(1) << 2)  /* Writable */
#define RISCV_PTE_X         (RISCV_UL(1) << 3)  /* Executable */
#define RISCV_PTE_U         (RISCV_UL(1) << 4)  /* User accessible */
#define RISCV_PTE_G         (RISCV_UL(1) << 5)  /* Global */
#define RISCV_PTE_A         (RISCV_UL(1) << 6)  /* Accessed */
#define RISCV_PTE_D         (RISCV_UL(1) << 7)  /* Dirty */

/* Sv39 SATP mode */
#define SATP_MODE_SV39      (RISCV_UL(8) << 60)
#define SATP_MODE_SV48      (RISCV_UL(9) << 60)
#define SATP_PPN_MASK       ((RISCV_ULL(1) << 44) - 1)

/* Virtual address space layout for Sv39 */
#define KERNEL_BASE         RISCV_UL(0xFFFFFFFF80000000)  /* -2GB */
#define KERNEL_SIZE         RISCV_UL(0x0000000080000000)  /* 2GB */
#define USER_BASE           RISCV_UL(0x0000000000000000)
#define USER_MAX            RISCV_UL(0x0000004000000000)  /* 256GB */

/* QEMU virt platform memory map */
#define VIRT_DRAM_BASE      RISCV_UL(0x80000000)  /* RAM starts here */
#define VIRT_UART0_BASE     RISCV_UL(0x10000000)  /* UART */
#define VIRT_UART0_SIZE     0x100
#define VIRT_PLIC_BASE      RISCV_UL(0x0C000000)  /* PLIC */
#define VIRT_PLIC_SIZE      0x4000000
#define VIRT_CLINT_BASE     RISCV_UL(0x02000000)  /* CLINT (timer/IPI) */
#define VIRT_CLINT_SIZE     0x10000
#define VIRT_VIRTIO_BASE    RISCV_UL(0x10001000)  /* VirtIO MMIO */
#define VIRT_VIRTIO_SIZE    0x1000
#define VIRT_VIRTIO_COUNT   8                     /* 8 VirtIO slots */

/* Boot module info (provided by QEMU loader) */
#define RISCV_BOOTINFO_ADDR RISCV_UL(0x80F00000)
#define RISCV_BOOTINFO_MAGIC RISCV_UL(0x584E494D) /* "MINX" */

/* PLIC constants */
#define PLIC_NUM_SOURCES    1024
#define PLIC_NUM_PRIORITIES 7

/* Interrupt numbers for QEMU virt */
#define IRQ_UART0           10
#define IRQ_VIRTIO_START    1   /* VirtIO devices start at IRQ 1 */
#define IRQ_VIRTIO_END      8

/* RISC-V CSR numbers */
#define CSR_SSTATUS         0x100
#define CSR_SIE             0x104
#define CSR_STVEC           0x105
#define CSR_SSCRATCH        0x140
#define CSR_SEPC            0x141
#define CSR_SCAUSE          0x142
#define CSR_STVAL           0x143
#define CSR_SIP             0x144
#define CSR_SATP            0x180

/* Encoded instructions for older assemblers */
#define RISCV_WFI_INSN      0x10500073
#define RISCV_SFENCE_VMA_INSN 0x12000073
#define RISCV_SRET_INSN     0x10200073

/* RISC-V CSR bits */

/* sstatus bits */
#define SSTATUS_SIE         (RISCV_UL(1) << 1)   /* Supervisor Interrupt Enable */
#define SSTATUS_SPIE        (RISCV_UL(1) << 5)   /* Previous Interrupt Enable */
#define SSTATUS_SPP         (RISCV_UL(1) << 8)   /* Previous Privilege (0=U, 1=S) */
#define SSTATUS_FS          (RISCV_UL(3) << 13)  /* FPU State */
#define SSTATUS_FS_MASK     SSTATUS_FS
#define SSTATUS_FS_OFF      (RISCV_UL(0) << 13)
#define SSTATUS_FS_INITIAL  (RISCV_UL(1) << 13)
#define SSTATUS_FS_CLEAN    (RISCV_UL(2) << 13)
#define SSTATUS_FS_DIRTY    (RISCV_UL(3) << 13)
#define SSTATUS_SUM         (RISCV_UL(1) << 18)  /* Supervisor User Memory access */

/* sie/sip bits */
#define SIE_SSIE            (RISCV_UL(1) << 1)   /* Software Interrupt Enable */
#define SIE_STIE            (RISCV_UL(1) << 5)   /* Timer Interrupt Enable */
#define SIE_SEIE            (RISCV_UL(1) << 9)   /* External Interrupt Enable */

#define SIP_SSIP            (RISCV_UL(1) << 1)   /* Software Interrupt Pending */
#define SIP_STIP            (RISCV_UL(1) << 5)   /* Timer Interrupt Pending */
#define SIP_SEIP            (RISCV_UL(1) << 9)   /* External Interrupt Pending */

/* scause values */
#define CAUSE_INTERRUPT     (RISCV_UL(1) << 63)  /* Interrupt bit */
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
#define CLOCK_FREQ              RISCV_UL(10000000)
#endif

/* Maximum number of CPUs supported */
#ifndef CONFIG_MAX_CPUS
#define CONFIG_MAX_CPUS         16
#endif

/* Stack sizes */
#define K_STACK_SIZE            16384
#define K_STACK_GUARD           4096

#define PG_ALLOCATEME           ((phys_bytes)-1)

#endif /* _RISCV64_KERNEL_ARCHCONST_H */
