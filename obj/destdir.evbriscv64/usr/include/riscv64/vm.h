/*
 * RISC-V 64 virtual memory definitions for MINIX
 *
 * Sv39: 39-bit virtual address space with 3-level page tables
 */

#ifndef _RISCV64_VM_H
#define _RISCV64_VM_H

/* Page sizes */
#define RISCV_PAGE_SIZE         4096            /* 4KB base page */
#define RISCV_MEGA_PAGE_SIZE    (2 * 1024 * 1024)   /* 2MB mega page */
#define RISCV_GIGA_PAGE_SIZE    (1024 * 1024 * 1024) /* 1GB giga page */

/* Page table structure */
#define RISCV_VM_PT_ENTRIES     512             /* Entries per page table */
#define RISCV_VM_DIR_ENTRIES    512             /* Entries in page directory */

/* PTE flags */
#define RISCV_VM_PTE_V          (1UL << 0)      /* Valid */
#define RISCV_VM_PTE_R          (1UL << 1)      /* Readable */
#define RISCV_VM_PTE_W          (1UL << 2)      /* Writable */
#define RISCV_VM_PTE_X          (1UL << 3)      /* Executable */
#define RISCV_VM_PTE_U          (1UL << 4)      /* User */
#define RISCV_VM_PTE_G          (1UL << 5)      /* Global */
#define RISCV_VM_PTE_A          (1UL << 6)      /* Accessed */
#define RISCV_VM_PTE_D          (1UL << 7)      /* Dirty */

/* Combined permission flags */
#define RISCV_VM_PTE_RW         (RISCV_VM_PTE_R | RISCV_VM_PTE_W)
#define RISCV_VM_PTE_RX         (RISCV_VM_PTE_R | RISCV_VM_PTE_X)
#define RISCV_VM_PTE_RWX        (RISCV_VM_PTE_R | RISCV_VM_PTE_W | RISCV_VM_PTE_X)

/* SATP (Supervisor Address Translation and Protection) modes */
#define RISCV_SATP_MODE_BARE    0UL
#define RISCV_SATP_MODE_SV39    (8UL << 60)
#define RISCV_SATP_MODE_SV48    (9UL << 60)
#define RISCV_SATP_MODE_SV57    (10UL << 60)

/* Virtual address masks */
#define RISCV_VM_ADDR_MASK      (~0xFFFUL)
#define RISCV_VM_PDE_MASK       0x1FF
#define RISCV_VM_PTE_MASK       0x1FF

/* Page fault error codes (scause values) */
#define RISCV_EXC_INST_MISALIGNED   0
#define RISCV_EXC_INST_ACCESS       1
#define RISCV_EXC_ILLEGAL_INST      2
#define RISCV_EXC_BREAKPOINT        3
#define RISCV_EXC_LOAD_MISALIGNED   4
#define RISCV_EXC_LOAD_ACCESS       5
#define RISCV_EXC_STORE_MISALIGNED  6
#define RISCV_EXC_STORE_ACCESS      7
#define RISCV_EXC_ECALL_U           8
#define RISCV_EXC_ECALL_S           9
#define RISCV_EXC_INST_PAGE_FAULT   12
#define RISCV_EXC_LOAD_PAGE_FAULT   13
#define RISCV_EXC_STORE_PAGE_FAULT  15

/* Macros for virtual address manipulation (Sv39) */
#define RISCV_VM_PDE(v)     (((v) >> 30) & 0x1FF)  /* Level 2 index */
#define RISCV_VM_PMD(v)     (((v) >> 21) & 0x1FF)  /* Level 1 index */
#define RISCV_VM_PTE(v)     (((v) >> 12) & 0x1FF)  /* Level 0 index */
#define RISCV_VM_OFFSET(v)  ((v) & 0xFFF)          /* Page offset */

/* PTE to physical address and back */
#define RISCV_PTE_TO_PHYS(pte)  (((pte) >> 10) << 12)
#define RISCV_PHYS_TO_PTE(pa)   ((((pa) >> 12) << 10) | RISCV_VM_PTE_V)

/* Compatibility with MINIX VM abstractions */
#define ARM_VM_PTE_PRESENT  RISCV_VM_PTE_V
#define ARM_VM_PTE_USER     RISCV_VM_PTE_U
#define ARM_VM_PTE_RW       RISCV_VM_PTE_RW
#define ARM_VM_PTE_RO       RISCV_VM_PTE_R
#define ARM_PAGE_SIZE       RISCV_PAGE_SIZE
#define ARM_VM_DIR_ENTRIES  RISCV_VM_DIR_ENTRIES
#define ARM_VM_PT_ENTRIES   RISCV_VM_PT_ENTRIES
#define ARM_SECTION_SIZE    RISCV_MEGA_PAGE_SIZE
#define ARM_PAGEDIR_SIZE    (RISCV_VM_DIR_ENTRIES * 8)
#define ARM_VM_ADDR_MASK    RISCV_VM_ADDR_MASK
#define ARM_VM_PDE_MASK     RISCV_VM_PDE_MASK
#define ARM_VM_PDE_PRESENT  RISCV_VM_PTE_V
#define ARM_VM_SECTION      0
#define ARM_VM_PTE_DEVICE   0
#define ARM_VM_PTE_WB       0
#define ARM_VM_PTE_WT       0
#define ARM_VM_PTE_S        0
#define ARM_VM_PTE_SUPER    0

/* Page fault analysis */
#define ARM_VM_PFE_FS(e)    ((e) & 0x1F)
#define ARM_VM_PFE_W        (1 << 5)
#define ARM_VM_PFE_L1PERM   0x0D
#define ARM_VM_PFE_L2PERM   0x0F

/* i386 compatibility */
#define I386_VM_PRESENT     RISCV_VM_PTE_V
#define I386_VM_WRITE       RISCV_VM_PTE_W
#define I386_VM_READ        RISCV_VM_PTE_R
#define I386_VM_USER        RISCV_VM_PTE_U
#define I386_VM_GLOBAL      RISCV_VM_PTE_G
#define I386_VM_PWT         0
#define I386_VM_PCD         0
#define I386_PAGE_SIZE      RISCV_PAGE_SIZE
#define I386_BIG_PAGE_SIZE  RISCV_MEGA_PAGE_SIZE
#define I386_VM_DIR_ENTRIES RISCV_VM_DIR_ENTRIES
#define I386_VM_PT_ENTRIES  RISCV_VM_PT_ENTRIES
#define I386_VM_ADDR_MASK   RISCV_VM_ADDR_MASK
#define I386_VM_PDE_MASK    RISCV_VM_PDE_MASK
#define I386_VM_BIGPAGE     0
#define I386_VM_PFE_P       (1 << 0)
#define I386_VM_PFE_W       (1 << 1)
#define I386_VM_PDE(v)      RISCV_VM_PDE(v)
#define I386_VM_PTE(v)      RISCV_VM_PTE(v)

#endif /* _RISCV64_VM_H */
