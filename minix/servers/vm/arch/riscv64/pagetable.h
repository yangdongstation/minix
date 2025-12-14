/*
 * RISC-V 64 page table definitions for VM server
 *
 * Sv39: 3-level page table with 39-bit virtual address space
 * - Level 2: 9 bits (512 entries) - Giga pages (1GB)
 * - Level 1: 9 bits (512 entries) - Mega pages (2MB)
 * - Level 0: 9 bits (512 entries) - Base pages (4KB)
 * - Page offset: 12 bits (4KB)
 */

#ifndef _PAGETABLE_H
#define _PAGETABLE_H 1

#include <stdint.h>
#include <machine/vm.h>

#include "vm.h"

/* Page table entry flags */
#define RISCV_PTE_V     (1UL << 0)  /* Valid */
#define RISCV_PTE_R     (1UL << 1)  /* Readable */
#define RISCV_PTE_W     (1UL << 2)  /* Writable */
#define RISCV_PTE_X     (1UL << 3)  /* Executable */
#define RISCV_PTE_U     (1UL << 4)  /* User accessible */
#define RISCV_PTE_G     (1UL << 5)  /* Global */
#define RISCV_PTE_A     (1UL << 6)  /* Accessed */
#define RISCV_PTE_D     (1UL << 7)  /* Dirty */

/* Reserved bits for software use (8-9) */
#define RISCV_PTE_SW0   (1UL << 8)
#define RISCV_PTE_SW1   (1UL << 9)

/* Mapping flags */
#define PTF_WRITE       RISCV_PTE_W
#define PTF_READ        RISCV_PTE_R
#define PTF_PRESENT     RISCV_PTE_V
#define PTF_USER        RISCV_PTE_U
#define PTF_GLOBAL      RISCV_PTE_G
#define PTF_NOCACHE     0           /* No direct cache control in RISC-V PTE */

/* Architecture constants */
#define ARCH_VM_DIR_ENTRIES     512
#define ARCH_BIG_PAGE_SIZE      (2UL * 1024 * 1024)  /* 2MB mega page */
#define ARCH_VM_ADDR_MASK       (~0xFFFUL)           /* Page frame mask */
#define ARCH_VM_PAGE_PRESENT    RISCV_PTE_V
#define ARCH_VM_PDE_MASK        0x1FF                /* 9 bits for each level */
#define ARCH_VM_PDE_PRESENT     RISCV_PTE_V
#define ARCH_VM_PTE_PRESENT     RISCV_PTE_V
#define ARCH_VM_PTE_USER        RISCV_PTE_U
#define ARCH_VM_PTE_RW          RISCV_PTE_W
#define ARCH_PAGEDIR_SIZE       4096                 /* Page table size */
#define ARCH_VM_BIGPAGE         0                    /* Mega page indicator */
#define ARCH_VM_PT_ENTRIES      512

/* For arch-specific PT routines to check if no bits outside
 * the regular flags are set.
 */
#define PTF_ALLFLAGS    (PTF_READ|PTF_WRITE|PTF_PRESENT|PTF_USER|PTF_GLOBAL|PTF_NOCACHE)

/* Page fault error code bits (from scause/stval) */
#define RISCV_PFE_INST  (1 << 0)    /* Instruction access fault */
#define RISCV_PFE_LOAD  (1 << 1)    /* Load access fault */
#define RISCV_PFE_STORE (1 << 2)    /* Store/AMO access fault */

/* Page fault analysis macros */
#define PFERR_NOPAGE(e) (((e) & 0xF) >= 12 && ((e) & 0xF) <= 15)  /* Page fault */
#define PFERR_PROT(e)   (((e) & 0xF) >= 1 && ((e) & 0xF) <= 7)    /* Access fault */
#define PFERR_WRITE(e)  (((e) & 0xF) == 15 || ((e) & 0xF) == 7)   /* Store fault */
#define PFERR_READ(e)   (((e) & 0xF) == 13 || ((e) & 0xF) == 5)   /* Load fault */

#define VM_PAGE_SIZE    4096

/* Virtual address decomposition (Sv39) */
#define RISCV_VA_OFFSET(va)     ((va) & 0xFFF)
#define RISCV_VA_VPN0(va)       (((va) >> 12) & 0x1FF)
#define RISCV_VA_VPN1(va)       (((va) >> 21) & 0x1FF)
#define RISCV_VA_VPN2(va)       (((va) >> 30) & 0x1FF)

/* Virtual address -> page table index macros */
#define ARCH_VM_PTE(v)  RISCV_VA_VPN0(v)
#define ARCH_VM_PDE(v)  RISCV_VA_VPN2(v)

/* PTE to physical address conversion */
#define RISCV_PTE_TO_PA(pte)    (((pte) >> 10) << 12)
#define RISCV_PA_TO_PTE(pa)     ((((pa) >> 12) << 10) | RISCV_PTE_V)

#endif /* _PAGETABLE_H */
