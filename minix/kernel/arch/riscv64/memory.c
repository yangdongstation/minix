/*
 * RISC-V 64 memory management
 *
 * Implements Sv39 paging with 4KB pages
 * Virtual address space: 512 GB (39 bits)
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"

/* Page table entry flags */
#define PTE_V       (1UL << 0)  /* Valid */
#define PTE_R       (1UL << 1)  /* Readable */
#define PTE_W       (1UL << 2)  /* Writable */
#define PTE_X       (1UL << 3)  /* Executable */
#define PTE_U       (1UL << 4)  /* User accessible */
#define PTE_G       (1UL << 5)  /* Global */
#define PTE_A       (1UL << 6)  /* Accessed */
#define PTE_D       (1UL << 7)  /* Dirty */

/* Page table levels */
#define PT_LEVELS   3
#define PT_ENTRIES  512
#define PT_SHIFT    9

/* Extract VPN fields from virtual address */
#define VPN0(va)    (((va) >> 12) & 0x1FF)
#define VPN1(va)    (((va) >> 21) & 0x1FF)
#define VPN2(va)    (((va) >> 30) & 0x1FF)

/* Convert PTE to physical address */
#define PTE_TO_PA(pte)  (((pte) >> 10) << 12)
#define PA_TO_PTE(pa)   ((((pa) >> 12) << 10) | PTE_V)

/* Kernel page table (set up in head.S) */
extern u64_t _boot_pgdir[];

/* Physical memory layout */
static phys_bytes mem_low;      /* Start of usable memory */
static phys_bytes mem_high;     /* End of usable memory */

/* Simple page allocator */
static phys_bytes free_page_list;

/*
 * Initialize memory management
 */
void mem_init(phys_bytes usedlimit)
{
    /* Set up memory bounds */
    mem_low = usedlimit;
    mem_high = PHYS_MEM_END;  /* From device tree or default */

    /* Initialize page allocator */
    free_page_list = 0;

    /* TODO: Parse device tree for actual memory layout */
}

/*
 * Allocate a physical page
 */
static phys_bytes page_alloc(void)
{
    phys_bytes page;

    if (free_page_list != 0) {
        /* Get from free list */
        page = free_page_list;
        free_page_list = *(phys_bytes *)page;
        return page;
    }

    /* Allocate from high memory */
    if (mem_low + PAGE_SIZE <= mem_high) {
        page = mem_low;
        mem_low += PAGE_SIZE;
        return page;
    }

    return 0;  /* Out of memory */
}

/*
 * Free a physical page
 */
static void page_free(phys_bytes page)
{
    *(phys_bytes *)page = free_page_list;
    free_page_list = page;
}

/*
 * Walk page table, optionally creating entries
 */
static u64_t *walk_pt(u64_t *pgdir, vir_bytes va, int create)
{
    u64_t *pt = pgdir;
    int level;

    for (level = 2; level > 0; level--) {
        int idx;

        switch (level) {
        case 2: idx = VPN2(va); break;
        case 1: idx = VPN1(va); break;
        default: idx = 0; break;
        }

        u64_t pte = pt[idx];

        if (!(pte & PTE_V)) {
            if (!create)
                return NULL;

            /* Allocate new page table */
            phys_bytes new_pt = page_alloc();
            if (new_pt == 0)
                return NULL;

            /* Clear new page table */
            memset((void *)new_pt, 0, PAGE_SIZE);

            /* Install in parent */
            pt[idx] = PA_TO_PTE(new_pt);
            pte = pt[idx];
        }

        /* Get next level page table */
        pt = (u64_t *)PTE_TO_PA(pte);
    }

    return &pt[VPN0(va)];
}

/*
 * Map a virtual address range
 */
int vm_map_range(struct proc *p, phys_bytes phys, vir_bytes vir,
                 size_t bytes, u32_t flags)
{
    u64_t *pgdir;
    size_t offset;
    u64_t pte_flags;

    /* Get process page directory */
    if (p == NULL) {
        pgdir = _boot_pgdir;
    } else {
        /* TODO: Get from proc structure */
        pgdir = _boot_pgdir;
    }

    /* Convert MINIX flags to RISC-V PTE flags */
    pte_flags = PTE_V | PTE_A | PTE_D;
    if (flags & VM_READ)
        pte_flags |= PTE_R;
    if (flags & VM_WRITE)
        pte_flags |= PTE_W;
    if (flags & VM_EXEC)
        pte_flags |= PTE_X;
    if (flags & VM_USER)
        pte_flags |= PTE_U;

    /* Map pages */
    for (offset = 0; offset < bytes; offset += PAGE_SIZE) {
        u64_t *pte = walk_pt(pgdir, vir + offset, 1);
        if (pte == NULL)
            return -1;

        *pte = PA_TO_PTE(phys + offset) | pte_flags;
    }

    /* Flush TLB */
    pg_flush_tlb();

    return 0;
}

/*
 * Unmap a virtual address range
 */
void vm_unmap_range(struct proc *p, vir_bytes vir, size_t bytes)
{
    u64_t *pgdir;
    size_t offset;

    if (p == NULL) {
        pgdir = _boot_pgdir;
    } else {
        pgdir = _boot_pgdir;
    }

    for (offset = 0; offset < bytes; offset += PAGE_SIZE) {
        u64_t *pte = walk_pt(pgdir, vir + offset, 0);
        if (pte != NULL) {
            *pte = 0;
        }
    }

    pg_flush_tlb();
}

/*
 * Initialize VM for a process
 */
void vm_init(struct proc *newptproc)
{
    /* TODO: Create new page table for process */
    (void)newptproc;
}

/*
 * Map user virtual address to physical
 */
phys_bytes umap_local(struct proc *p, int seg, vir_bytes vir, vir_bytes bytes)
{
    u64_t *pgdir;
    u64_t *pte;

    (void)seg;
    (void)bytes;

    if (p == NULL) {
        pgdir = _boot_pgdir;
    } else {
        pgdir = _boot_pgdir;
    }

    pte = walk_pt(pgdir, vir, 0);
    if (pte == NULL || !(*pte & PTE_V))
        return 0;

    return PTE_TO_PA(*pte) | (vir & (PAGE_SIZE - 1));
}
