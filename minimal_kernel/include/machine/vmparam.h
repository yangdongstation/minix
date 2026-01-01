/*
 * RISC-V 64 Virtual Memory Parameters for MINIX
 * Based on kernel.lds memory layout
 */

#ifndef _MACHINE_VMPARAM_H_
#define _MACHINE_VMPARAM_H_

/*
 * Kernel memory layout:
 * - Physical load address: 0x80200000
 * - Virtual address: 0xFFFFFFFF80200000 (high half)
 */

#define KERNBASE        0xFFFFFFFF80000000UL    /* Kernel base virtual address */
#define KERNTEXTOFF     0xFFFFFFFF80200000UL    /* Kernel text start */
#define KERNLOAD        0x80200000UL            /* Kernel physical load address */

/*
 * User VM space constants
 */
#define USRSTACK        0x0000003fffff0000UL    /* User stack top */
#define USRSTACKSIZE    (16*1024*1024)          /* 16MB user stack */

/*
 * Virtual memory constants
 */
#define NBPG            4096                    /* Page size */
#define PGSHIFT         12                      /* LOG2(NBPG) */
#define PGOFSET         (NBPG-1)                /* Byte offset into page */
#define NPTEPG          (NBPG/(sizeof (pt_entry_t)))

/*
 * Page table entry constants (Sv39)
 */
#define PTE_V           0x001                   /* Valid */
#define PTE_R           0x002                   /* Read */
#define PTE_W           0x004                   /* Write */
#define PTE_X           0x008                   /* Execute */
#define PTE_U           0x010                   /* User */
#define PTE_G           0x020                   /* Global */
#define PTE_A           0x040                   /* Accessed */
#define PTE_D           0x080                   /* Dirty */

/*
 * Memory sizes
 */
#define MAXTSIZ         (64*1024*1024)          /* Max text size (64MB) */
#define MAXDSIZ         (512*1024*1024)         /* Max data size (512MB) */
#define MAXSSIZ         (16*1024*1024)          /* Max stack size (16MB) */

#endif /* _MACHINE_VMPARAM_H_ */