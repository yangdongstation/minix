/*
 * RISC-V 64 page table utilities
 *
 * Sv39: 3-level page table
 * - 512 GB virtual address space
 * - 4KB, 2MB (megapage), 1GB (gigapage) pages
 */

#include <assert.h>
#include <string.h>

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

#define PAGE_SIZE RISCV_PAGE_SIZE

/* Extract VPN fields from virtual address */
#define VPN0(va)    (((va) >> 12) & 0x1FF)
#define VPN1(va)    (((va) >> 21) & 0x1FF)
#define VPN2(va)    (((va) >> 30) & 0x1FF)

#define PTE_TO_PA(pte)  (((pte) >> 10) << 12)
#define PA_TO_PTE(pa)   (((pa) >> 12) << 10)

/* Kernel page directory */
extern u64_t _boot_pgdir[];

#ifndef MULTIBOOT_MEMORY_AVAILABLE
#define MULTIBOOT_MEMORY_AVAILABLE 1
#endif

static phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	int m;
	multiboot_memory_map_t *mmap;

	for (m = cbi->mmap_size - 1; m >= 0; m--) {
		mmap = &cbi->memmap[m];
		if (!mmap->mm_length)
			continue;
		assert(!(mmap->mm_length % PAGE_SIZE));
		assert(!(mmap->mm_base_addr % PAGE_SIZE));

		mmap->mm_length -= PAGE_SIZE;
		cbi->kernel_allocated_bytes_dynamic += PAGE_SIZE;

		return mmap->mm_base_addr + mmap->mm_length;
	}

	panic("can't find free memory");
}

static u64_t *pg_walk(u64_t *pgdir, vir_bytes va, int create)
{
	u64_t *pt = pgdir;
	int level;

	for (level = 2; level > 0; level--) {
		int idx;
		u64_t pte;

		switch (level) {
		case 2: idx = VPN2(va); break;
		case 1: idx = VPN1(va); break;
		default: idx = 0; break;
		}

		pte = pt[idx];
		if (!(pte & PTE_V)) {
			phys_bytes new_pt;

			if (!create)
				return NULL;

			new_pt = pg_alloc_page(&kinfo);
			memset((void *)new_pt, 0, PAGE_SIZE);
			pt[idx] = PA_TO_PTE(new_pt) | PTE_V;
			pte = pt[idx];
		}

		pt = (u64_t *)PTE_TO_PA(pte);
	}

	return &pt[VPN0(va)];
}

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
	vir_bytes vaddr;
	phys_bytes paddr;
	size_t left;
	u64_t pte_flags;

	if (size == 0)
		return;

	vaddr = rounddown(virt, PAGE_SIZE);
	paddr = phys;
	left = roundup(size + (virt - vaddr), PAGE_SIZE);

	pte_flags = flags | PTE_V | PTE_A | PTE_D;

	while (left > 0) {
		u64_t *pte;
		phys_bytes map_phys = paddr;

		if (phys == PG_ALLOCATEME)
			map_phys = pg_alloc_page(&kinfo);

		pte = pg_walk(_boot_pgdir, vaddr, 1);
		if (pte == NULL)
			panic("pg_map: no pte for 0x%lx", vaddr);

		*pte = PA_TO_PTE(map_phys) | pte_flags;

		vaddr += PAGE_SIZE;
		if (phys != PG_ALLOCATEME)
			paddr += PAGE_SIZE;
		left -= PAGE_SIZE;
	}

	pg_flush_tlb();
}

/*
 * Unmap virtual address range
 */
void pg_unmap(vir_bytes virt, size_t size)
{
	vir_bytes vaddr;
	size_t left;

	if (size == 0)
		return;

	vaddr = rounddown(virt, PAGE_SIZE);
	left = roundup(size + (virt - vaddr), PAGE_SIZE);

	while (left > 0) {
		u64_t *pte = pg_walk(_boot_pgdir, vaddr, 0);
		if (pte != NULL)
			*pte = 0;
		vaddr += PAGE_SIZE;
		left -= PAGE_SIZE;
	}

	pg_flush_tlb();
}

/*
 * Create identity mapping for address range
 */
void pg_identity_map(phys_bytes start, phys_bytes end)
{
	phys_bytes phys;

	start = rounddown(start, PAGE_SIZE);
	end = roundup(end, PAGE_SIZE);

	for (phys = start; phys < end; phys += PAGE_SIZE)
		pg_map(phys, phys, PAGE_SIZE,
			PTE_R | PTE_W | PTE_X | PTE_G);
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
	phys_bytes pgdir;

	pgdir = pg_alloc_page(&kinfo);
	memset((void *)pgdir, 0, PAGE_SIZE);
	return pgdir;
}

/*
 * Destroy page table
 */
void pg_destroy(phys_bytes pgdir)
{
    /* TODO: Free all page table pages */
    (void)pgdir;
}

void add_memmap(kinfo_t *cbi, u64_t addr, u64_t len)
{
    int m;
    phys_bytes highmark;

    if (len == 0)
        return;

    addr = roundup(addr, RISCV_PAGE_SIZE);
    len = rounddown(len, RISCV_PAGE_SIZE);
    if (len == 0)
        return;

    for (m = 0; m < MAXMEMMAP; m++) {
        if (cbi->memmap[m].mm_length)
            continue;

        cbi->memmap[m].mm_base_addr = addr;
        cbi->memmap[m].mm_length = len;
        cbi->memmap[m].type = MULTIBOOT_MEMORY_AVAILABLE;

        if (m >= cbi->mmap_size)
            cbi->mmap_size = m + 1;

        highmark = addr + len;
        if (highmark > cbi->mem_high_phys)
            cbi->mem_high_phys = highmark;

        return;
    }

    panic("no available memmap slot");
}
