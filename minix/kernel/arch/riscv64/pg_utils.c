/*
 * RISC-V 64 page table utilities
 *
 * Sv39: 3-level page table
 * - 512 GB virtual address space
 * - 4KB, 2MB (megapage), 1GB (gigapage) pages
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"

/* Page table entry flags */
#define PTE_V       (1UL << 0)
#define PTE_R       (1UL << 1)
#define PTE_W       (1UL << 2)
#define PTE_X       (1UL << 3)
#define PTE_U       (1UL << 4)
#define PTE_G       (1UL << 5)
#define PTE_A       (1UL << 6)
#define PTE_D       (1UL << 7)

/* Kernel page directory */
extern u64_t _boot_pgdir[];

/*
 * Early page table initialization (called from head.S)
 * Sets up identity mapping and high kernel mapping
 */
void pg_early_init(void)
{
    u64_t *pgdir = _boot_pgdir;
    u64_t kernel_start = 0x80000000UL;
    u64_t kernel_size = 0x40000000UL;  /* 1GB */
    u64_t flags = PTE_V | PTE_R | PTE_W | PTE_X | PTE_A | PTE_D | PTE_G;

    /* Clear page directory */
    for (int i = 0; i < 512; i++) {
        pgdir[i] = 0;
    }

    /* Identity map first 1GB (gigapage) for boot */
    /* VPN[2] for 0x80000000 = 2 */
    pgdir[2] = ((kernel_start >> 12) << 10) | flags;

    /* Map kernel to high address (0xFFFFFFFF80000000) */
    /* VPN[2] for 0xFFFFFFFF80000000 = 510 */
    pgdir[510] = ((kernel_start >> 12) << 10) | flags;

    /* Also map first 1GB for device MMIO */
    /* VPN[2] for 0x00000000 = 0 */
    pgdir[0] = (0UL << 10) | flags;
}

/*
 * Map physical to virtual address
 */
void pg_map(phys_bytes phys, vir_bytes virt, size_t size, u64_t flags)
{
    /* TODO: Implement 4KB page mapping */
    (void)phys;
    (void)virt;
    (void)size;
    (void)flags;
}

/*
 * Unmap virtual address range
 */
void pg_unmap(vir_bytes virt, size_t size)
{
    /* TODO: Implement */
    (void)virt;
    (void)size;
}

/*
 * Create identity mapping for address range
 */
void pg_identity_map(phys_bytes start, phys_bytes end)
{
    /* TODO: Implement */
    (void)start;
    (void)end;
}

/*
 * Flush TLB (Translation Lookaside Buffer)
 */
void pg_flush_tlb(void)
{
    __asm__ __volatile__("sfence.vma" ::: "memory");
}

/*
 * Flush TLB for specific address
 */
void pg_flush_tlb_addr(vir_bytes addr)
{
    __asm__ __volatile__("sfence.vma %0" :: "r"(addr) : "memory");
}

/*
 * Flush TLB for specific ASID
 */
void pg_flush_tlb_asid(int asid)
{
    __asm__ __volatile__("sfence.vma zero, %0" :: "r"(asid) : "memory");
}

/*
 * Load page table for process
 */
void pg_load(struct proc *p)
{
    u64_t satp;

    /* TODO: Get page table root from proc structure */
    /* For now, use boot page directory */
    satp = ((phys_bytes)_boot_pgdir >> 12) | SATP_MODE_SV39;

    csr_write_satp(satp);
}

/*
 * Create new page table
 * Returns physical address of page directory
 */
phys_bytes pg_create(void)
{
    /* TODO: Allocate and initialize new page directory */
    return 0;
}

/*
 * Destroy page table
 */
void pg_destroy(phys_bytes pgdir)
{
    /* TODO: Free all page table pages */
    (void)pgdir;
}
