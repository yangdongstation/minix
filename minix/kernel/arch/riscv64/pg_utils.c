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

#define UART_THR        0x00
#define UART_LSR        0x05
#define UART_LSR_THRE   0x20

#define PAGE_SIZE RISCV_PAGE_SIZE
#define RISCV_GIGA_PAGE_SIZE (1UL << 30)

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

static inline void *pg_phys_to_virt(phys_bytes pa)
{
	if (pa < VIRT_DRAM_BASE)
		return (void *)(vir_bytes)pa;
	return (void *)(vir_bytes)(KERNEL_BASE + (pa - VIRT_DRAM_BASE));
}

static phys_bytes pg_free_list;

static inline void early_uart_putc(int c)
{
	volatile u8_t *uart = (volatile u8_t *)VIRT_UART0_BASE;

	while ((uart[UART_LSR] & UART_LSR_THRE) == 0)
		;
	uart[UART_THR] = (u8_t)c;
}

static phys_bytes pg_alloc_page(kinfo_t *cbi)
{
	int m;
	multiboot_memory_map_t *mmap;
	phys_bytes page;

	if (pg_free_list != 0) {
		page = pg_free_list;
		pg_free_list = *(phys_bytes *)pg_phys_to_virt(page);
		return page;
	}

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

static void pg_free_page(phys_bytes page)
{
	*(phys_bytes *)pg_phys_to_virt(page) = pg_free_list;
	pg_free_list = page;
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
			memset(pg_phys_to_virt(new_pt), 0, PAGE_SIZE);
			pt[idx] = PA_TO_PTE(new_pt) | PTE_V;
			pte = pt[idx];
		} else if (pte & (PTE_R | PTE_W | PTE_X)) {
			phys_bytes new_pt;
			phys_bytes base;
			phys_bytes child_size;
			u64_t flags;
			int i;

			if (!create)
				return NULL;

			/* Split large-page leaf so we can map smaller pages. */
			new_pt = pg_alloc_page(&kinfo);
			memset(pg_phys_to_virt(new_pt), 0, PAGE_SIZE);

			base = PTE_TO_PA(pte);
			flags = pte & (PTE_R | PTE_W | PTE_X | PTE_U |
				PTE_G | PTE_A | PTE_D);
			child_size = (phys_bytes)1ULL <<
				(RISCV_PAGE_SHIFT + (level - 1) * RISCV_PTE_SHIFT);

			for (i = 0; i < RISCV_PTES_PER_PT; i++) {
				((u64_t *)pg_phys_to_virt(new_pt))[i] =
					PA_TO_PTE(base + (phys_bytes)i * child_size) |
					flags | PTE_V;
			}

			pt[idx] = PA_TO_PTE(new_pt) | PTE_V;
			pte = pt[idx];
		}

		pt = (u64_t *)pg_phys_to_virt(PTE_TO_PA(pte));
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

void pg_extend_kernel_map(phys_bytes start, phys_bytes size)
{
	u64_t flags = PTE_V | PTE_R | PTE_W | PTE_X | PTE_A | PTE_D | PTE_G;
	phys_bytes map_size;
	phys_bytes map_base;
	int entries;
	int i;

	if (size == 0)
		return;

	map_base = rounddown(start, RISCV_GIGA_PAGE_SIZE);
	map_size = roundup(size + (start - map_base), RISCV_GIGA_PAGE_SIZE);
	if (map_size > KERNEL_SIZE)
		map_size = KERNEL_SIZE;

	entries = map_size / RISCV_GIGA_PAGE_SIZE;
	for (i = 0; i < entries; i++) {
		vir_bytes va = KERNEL_BASE +
			(vir_bytes)i * RISCV_GIGA_PAGE_SIZE;
		int idx = VPN2(va);

		_boot_pgdir[idx] = PA_TO_PTE(map_base +
			(phys_bytes)i * RISCV_GIGA_PAGE_SIZE) | flags;
	}

	pg_flush_tlb();
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
    sfence_vma_all();
}

/*
 * Flush TLB for specific address
 */
void pg_flush_tlb_addr(vir_bytes addr)
{
    (void)addr;
    sfence_vma_all();
}

/*
 * Flush TLB for specific ASID
 */
void pg_flush_tlb_asid(int asid)
{
    (void)asid;
    sfence_vma_all();
}

/*
 * Load page table for process
 */
void pg_load(struct proc *p)
{
    u64_t satp;
    phys_bytes root;

    if (p && p->p_seg.p_satp != 0)
        root = p->p_seg.p_satp;
    else
        root = (phys_bytes)_boot_pgdir;

    satp = (root >> 12) | SATP_MODE_SV39;

    early_uart_putc('A');
    __asm__ __volatile__("csrw " RISCV64_STRINGIFY(CSR_SATP) ", %0"
        :: "r"(satp) : "memory");
    early_uart_putc('B');
    sfence_vma_all();
    early_uart_putc('C');
    direct_print("rv64: pg_load leave\n");
}

void pg_dump_mapping(vir_bytes va)
{
	u64_t *pt = _boot_pgdir;
	u64_t pte;
	int level;

	direct_print("rv64: map va=");
	direct_print_hex((u64_t)va);
	direct_print(" ");

	for (level = 2; level >= 0; level--) {
		int idx;
		u64_t page_size;
		phys_bytes pa;

		switch (level) {
		case 2: idx = VPN2(va); break;
		case 1: idx = VPN1(va); break;
		default: idx = VPN0(va); break;
		}

		pte = pt[idx];
		if (!(pte & PTE_V)) {
			direct_print("pte=0\n");
			return;
		}

		if ((pte & (PTE_R | PTE_W | PTE_X)) || level == 0) {
			page_size = (u64_t)1ULL <<
				(RISCV_PAGE_SHIFT + level * RISCV_PTE_SHIFT);
			pa = PTE_TO_PA(pte) | (va & (page_size - 1));

			direct_print("pte=");
			direct_print_hex(pte);
			direct_print(" pa=");
			direct_print_hex((u64_t)pa);
			direct_print(" L");
			direct_print_dec((u64_t)level);
			direct_print("\n");
			return;
		}

		pt = (u64_t *)pg_phys_to_virt(PTE_TO_PA(pte));
	}
}

/*
 * Create new page table
 * Returns physical address of page directory
 */
phys_bytes pg_create(void)
{
	phys_bytes pgdir;

	pgdir = pg_alloc_page(&kinfo);
	memset(pg_phys_to_virt(pgdir), 0, PAGE_SIZE);
	return pgdir;
}

static void pg_free_pt_level(u64_t *pt, int level)
{
	int i;

	for (i = 0; i < RISCV_PTES_PER_PT; i++) {
		u64_t pte = pt[i];

		if (!(pte & PTE_V))
			continue;
		if (pte & (PTE_R | PTE_W | PTE_X))
			continue;
		if (level == 0)
			continue;

		pg_free_pt_level((u64_t *)pg_phys_to_virt(PTE_TO_PA(pte)),
			level - 1);
		pg_free_page(PTE_TO_PA(pte));
		pt[i] = 0;
	}
}

/*
 * Destroy page table
 */
void pg_destroy(phys_bytes pgdir)
{
	if (pgdir == 0 || pgdir == (phys_bytes)_boot_pgdir)
		return;

	pg_free_pt_level((u64_t *)pg_phys_to_virt(pgdir), 2);
	pg_free_page(pgdir);
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
