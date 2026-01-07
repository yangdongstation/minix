
#define _SYSTEM 1

#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/config.h>
#include <minix/const.h>
#include <minix/ds.h>
#include <minix/endpoint.h>
#include <minix/minlib.h>
#include <minix/type.h>
#include <minix/ipc.h>
#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/safecopies.h>
#include <minix/cpufeature.h>
#include <minix/bitmap.h>
#include <minix/debug.h>

#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#include "proto.h"
#include "glo.h"
#include "util.h"
#include "vm.h"
#include "sanitycheck.h"

static int vm_self_pages;
static int pt_init_done;
#if defined(__riscv64__)
static int pt_copying;
#endif
#if defined(__riscv64__)
static int pt_dmap_ready;
#endif

#if defined(__riscv64__)
static int pt_l0alloc(pt_t *pt, int pde, int pte1, int verify);
static int pt_l1alloc(pt_t *pt, int pde, int verify);
static u64_t *pt_get_pte(pt_t *pt, vir_bytes v);
static int vm_dmap_in_range(vir_bytes v, int pages);
static int vm_phys_in_dmap(phys_bytes p, int pages);

static int pt_l0alloc(pt_t *pt, int pde, int pte1, int verify)
{
	phys_bytes pt_phys;
	u64_t *p;

	if (pt->pt_pt[pde][pte1] & ARCH_VM_PDE_PRESENT) {
		if (pt->pt_pt[pde][pte1] & RISCV_PTE_LEAF) {
			phys_bytes base;
			u64_t flags;
			int i, r;

			if (verify)
				return EFAULT;
			if (!pt->pt_pt_l0[pde]) {
				phys_bytes l0_phys;
				u64_t **l0 = vm_allocpage(&l0_phys, VMP_PAGETABLE);
				if (!l0)
					return ENOMEM;
				memset(l0, 0, VM_PAGE_SIZE);
				pt->pt_pt_l0[pde] = l0;
			}
			if (pt->pt_pt_l0[pde] && pt->pt_pt_l0[pde][pte1])
				return OK;
			if (!(p = vm_allocpage(&pt_phys, VMP_PAGETABLE)))
				return ENOMEM;
			memset(p, 0, VM_PAGE_SIZE);
			base = RISCV_PTE_TO_PA(pt->pt_pt[pde][pte1]);
			flags = pt->pt_pt[pde][pte1] &
				(RISCV_PTE_R | RISCV_PTE_W | RISCV_PTE_X |
				RISCV_PTE_U | RISCV_PTE_G | RISCV_PTE_A |
				RISCV_PTE_D);
			for (i = 0; i < ARCH_VM_PT_ENTRIES; i++) {
				p[i] = RISCV_PA_TO_PTE(base +
					(phys_bytes)i * VM_PAGE_SIZE) | flags;
			}
			pt->pt_pt_l0[pde][pte1] = p;
			pt->pt_pt[pde][pte1] = RISCV_PA_TO_PTE(pt_phys);
			r = sys_vmctl(SELF, VMCTL_FLUSHTLB, 0);
			if (r != OK)
				panic("VMCTL_FLUSHTLB failed: %d", r);
			return OK;
		}
		if (!pt->pt_pt_l0[pde]) {
			phys_bytes l0_phys;
			u64_t **l0 = vm_allocpage(&l0_phys, VMP_PAGETABLE);
			if (!l0)
				return ENOMEM;
			memset(l0, 0, VM_PAGE_SIZE);
			pt->pt_pt_l0[pde] = l0;
		}
		if (pt->pt_pt_l0[pde] && pt->pt_pt_l0[pde][pte1])
			return OK;
		if (verify)
			return EFAULT;
		{
			phys_bytes l0_phys;

			l0_phys = RISCV_PTE_TO_PA(pt->pt_pt[pde][pte1]);
			if (pt_dmap_ready && vm_phys_in_dmap(l0_phys, 1)) {
				pt->pt_pt_l0[pde][pte1] = (u64_t *)(
				    VM_OWN_DMAPBASE +
				    (vir_bytes)(l0_phys - VM_OWN_DMAP_PHYS_BASE));
				return OK;
			}
			if (!(p = vm_allocpage(&pt_phys, VMP_PAGETABLE)))
				return ENOMEM;
			if (sys_abscopy(l0_phys, pt_phys, VM_PAGE_SIZE) != OK) {
				vm_freepages((vir_bytes)p, 1);
				return EFAULT;
			}
			pt->pt_pt_l0[pde][pte1] = p;
			pt->pt_pt[pde][pte1] = RISCV_PA_TO_PTE(pt_phys);
			return OK;
		}
	}

	if (verify)
		return EFAULT;

	if (!(p = vm_allocpage(&pt_phys, VMP_PAGETABLE)))
		return ENOMEM;
	if (pt->pt_pt_l0[pde] && pt->pt_pt_l0[pde][pte1]) {
		vm_freepages((vir_bytes)p, 1);
		return OK;
	}

	memset(p, 0, VM_PAGE_SIZE);
	if (!pt->pt_pt_l0[pde]) {
		phys_bytes l0_phys;
		u64_t **l0 = vm_allocpage(&l0_phys, VMP_PAGETABLE);
		if (!l0) {
			vm_freepages((vir_bytes)p, 1);
			return ENOMEM;
		}
		memset(l0, 0, VM_PAGE_SIZE);
		pt->pt_pt_l0[pde] = l0;
	}
	pt->pt_pt_l0[pde][pte1] = p;
	pt->pt_pt[pde][pte1] = RISCV_PA_TO_PTE(pt_phys);

	return OK;
}

static int pt_l1alloc(pt_t *pt, int pde, int verify)
{
	phys_bytes pt_phys;
	phys_bytes l0_phys;
	phys_bytes base;
	u64_t flags;
	u64_t *p;
	u64_t **l0;
	int i, r;

	if (!(pt->pt_dir[pde] & RISCV_PTE_LEAF))
		return OK;
	if (verify)
		return EFAULT;
	if (pt->pt_pt[pde])
		return OK;

	if (!(p = vm_allocpage(&pt_phys, VMP_PAGETABLE)))
		return ENOMEM;
	memset(p, 0, VM_PAGE_SIZE);

	if (!(l0 = vm_allocpage(&l0_phys, VMP_PAGETABLE))) {
		vm_freepages((vir_bytes)p, 1);
		return ENOMEM;
	}
	memset(l0, 0, VM_PAGE_SIZE);

	base = RISCV_PTE_TO_PA(pt->pt_dir[pde]);
	flags = pt->pt_dir[pde] &
		(RISCV_PTE_R | RISCV_PTE_W | RISCV_PTE_X |
		RISCV_PTE_U | RISCV_PTE_G | RISCV_PTE_A |
		RISCV_PTE_D);
	for (i = 0; i < ARCH_VM_PT_ENTRIES; i++) {
		p[i] = RISCV_PA_TO_PTE(base +
			(phys_bytes)i * (VM_PAGE_SIZE * ARCH_VM_PT_ENTRIES)) |
			flags;
	}

	pt->pt_pt[pde] = p;
	pt->pt_pt_l0[pde] = l0;
	pt->pt_dir[pde] = RISCV_PA_TO_PTE(pt_phys);
	r = sys_vmctl(SELF, VMCTL_FLUSHTLB, 0);
	if (r != OK)
		panic("VMCTL_FLUSHTLB failed: %d", r);

	return OK;
}

static u64_t *pt_get_pte(pt_t *pt, vir_bytes v)
{
	int pde = ARCH_VM_PDE(v);
	int pte1 = ARCH_VM_PTE1(v);
	int pte0 = ARCH_VM_PTE(v);

	if (!(pt->pt_dir[pde] & ARCH_VM_PDE_PRESENT))
		return NULL;
	if (!pt->pt_pt[pde] || !pt->pt_pt_l0[pde])
		return NULL;
	if (!(pt->pt_pt[pde][pte1] & ARCH_VM_PDE_PRESENT))
		return NULL;
	if (pt->pt_pt[pde][pte1] & RISCV_PTE_LEAF)
		return NULL;
	if (!pt->pt_pt_l0[pde][pte1])
		return NULL;

	return &pt->pt_pt_l0[pde][pte1][pte0];
}

static int vm_dmap_in_range(vir_bytes v, int pages)
{
	vir_bytes end = v + (vir_bytes)pages * VM_PAGE_SIZE;

	return v >= VM_OWN_DMAPBASE && end <= VM_OWN_DMAPTOP && end >= v;
}

static int vm_phys_in_dmap(phys_bytes p, int pages)
{
	phys_bytes end = p + (phys_bytes)pages * VM_PAGE_SIZE;

	return p >= VM_OWN_DMAP_PHYS_BASE &&
		end <= VM_OWN_DMAP_PHYS_TOP && end >= p;
}
#endif

/* PDE used to map in kernel, kernel physical address. */
#define MAX_PAGEDIR_PDES 5
static struct pdm {
	int		pdeno;
#if defined(__riscv64__)
	u64_t		val;
#else
	u32_t		val;
#endif
	phys_bytes	phys;
#if defined(__riscv64__)
	u64_t		*page_directories;
	u64_t		*page_directories_l0;
#else
	u32_t		*page_directories;
#endif
} pagedir_mappings[MAX_PAGEDIR_PDES];

static multiboot_module_t *kern_mb_mod = NULL;
static size_t kern_size = 0;
#if defined(__riscv64__)
static phys_bytes kern_phys_base;
#endif
static int kern_start_pde = -1;

/* big page size available in hardware? */
static int bigpage_ok = 1;

/* Our process table entry. */
struct vmproc *vmprocess = &vmproc[VM_PROC_NR];

/* Spare memory, ready to go after initialization, to avoid a
 * circular dependency on allocating memory and writing it into VM's
 * page table.
 */
#if SANITYCHECKS
#define SPAREPAGES 200
#define STATIC_SPAREPAGES 190
#else
#if defined(__arm__)
# define SPAREPAGES 150
# define STATIC_SPAREPAGES 140 
#elif defined(__riscv64__)
# define SPAREPAGES 512
# define STATIC_SPAREPAGES 480
#else
# define SPAREPAGES 20
# define STATIC_SPAREPAGES 15 
#endif /* __arm__ */
#endif

#ifdef __i386__
static u32_t global_bit = 0;
#endif

#define SPAREPAGEDIRS 1
#define STATIC_SPAREPAGEDIRS 1

int missing_sparedirs = SPAREPAGEDIRS;
static struct {
	void *pagedir;
	phys_bytes phys;
} sparepagedirs[SPAREPAGEDIRS];

#define is_staticaddr(v) ((vir_bytes) (v) < VM_OWN_HEAPSTART)

#define MAX_KERNMAPPINGS 10
static struct {
	phys_bytes	phys_addr;	/* Physical addr. */
	phys_bytes	len;		/* Length in bytes. */
	vir_bytes	vir_addr;	/* Offset in page table. */
	int		flags;
} kern_mappings[MAX_KERNMAPPINGS];
int kernmappings = 0;

#if defined(__riscv64__)
static void pt_verify_kernel_mappings(pt_t *pt, const char *tag)
{
	int i;
	int missing = 0;
	u64_t entry;

	if (!pt || kern_start_pde < 0)
		return;

	entry = pt->pt_dir[kern_start_pde];
	if (!(entry & ARCH_VM_PTE_PRESENT) || !(entry & RISCV_PTE_LEAF)) {
		printf("VM: %s missing kern pde=%d entry=0x%llx\n",
		    tag, kern_start_pde, (unsigned long long)entry);
		missing++;
	}

	entry = pt->pt_dir[2];
	if (!(entry & ARCH_VM_PTE_PRESENT) || !(entry & RISCV_PTE_LEAF)) {
		printf("VM: %s missing identity pde=2 entry=0x%llx\n",
		    tag, (unsigned long long)entry);
		missing++;
	}

	for (i = 0; i < kernmappings; i++) {
		u64_t *pte = pt_get_pte(pt, kern_mappings[i].vir_addr);

		if (!pte || !(*pte & ARCH_VM_PTE_PRESENT)) {
			printf("VM: %s missing kernmap i=%d vir=0x%lx\n",
			    tag, i, (unsigned long)kern_mappings[i].vir_addr);
			missing++;
		}
	}

	if (missing) {
		printf("VM: %s kernel mapping missing=%d\n", tag, missing);
	}
}
#endif

/* Clicks must be pages, as
 *  - they must be page aligned to map them
 *  - they must be a multiple of the page size
 *  - it's inconvenient to have them bigger than pages, because we often want
 *    just one page
 * May as well require them to be equal then.
 */
#if CLICK_SIZE != VM_PAGE_SIZE
#error CLICK_SIZE must be page size.
#endif

static void *spare_pagequeue;
static char static_sparepages[VM_PAGE_SIZE*STATIC_SPAREPAGES] 
	__aligned(VM_PAGE_SIZE);

#if defined(__arm__)
static char static_sparepagedirs[ARCH_PAGEDIR_SIZE*STATIC_SPAREPAGEDIRS + ARCH_PAGEDIR_SIZE] __aligned(ARCH_PAGEDIR_SIZE);
#endif

void pt_assert(pt_t *pt)
{
	char dir[4096];
	pt_clearmapcache();
	if((sys_vmctl(SELF, VMCTL_FLUSHTLB, 0)) != OK) {
		panic("VMCTL_FLUSHTLB failed");
	}
	sys_physcopy(NONE, pt->pt_dir_phys, SELF, (vir_bytes) dir, sizeof(dir), 0);
	assert(!memcmp(dir, pt->pt_dir, sizeof(dir)));
}

#if SANITYCHECKS
/*===========================================================================*
 *				pt_sanitycheck		     		     *
 *===========================================================================*/
void pt_sanitycheck(pt_t *pt, const char *file, int line)
{
/* Basic pt sanity check. */
	int slot;

	MYASSERT(pt);
	MYASSERT(pt->pt_dir);
	MYASSERT(pt->pt_dir_phys);

	for(slot = 0; slot < ELEMENTS(vmproc); slot++) {
		if(pt == &vmproc[slot].vm_pt)
			break;
	}

	if(slot >= ELEMENTS(vmproc)) {
		panic("pt_sanitycheck: passed pt not in any proc");
	}

	MYASSERT(usedpages_add(pt->pt_dir_phys, VM_PAGE_SIZE) == OK);
}
#endif

/*===========================================================================*
 *				findhole		     		     *
 *===========================================================================*/
static vir_bytes findhole(int pages)
{
/* Find a space in the virtual address space of VM. */
	int try_restart;
	static vir_bytes lastv = 0;
	pt_t *pt = &vmprocess->vm_pt;
	vir_bytes vmin, vmax;
	vir_bytes curv;
	vir_bytes holev = NO_MEM;
	int holesize = -1;

	vmin = VM_OWN_MMAPBASE;
	vmax = VM_OWN_MMAPTOP;

	/* Input sanity check. */
	assert(vmin + VM_PAGE_SIZE >= vmin);
	assert(vmax >= vmin + VM_PAGE_SIZE);
	assert((vmin % VM_PAGE_SIZE) == 0);
	assert((vmax % VM_PAGE_SIZE) == 0);
	assert(pages > 0);

	curv = lastv;
	if(curv < vmin || curv >= vmax)
		curv = vmin;

	try_restart = 1;

	/* Start looking for a free page starting at vmin. */
	while(curv < vmax) {
		int pte;

		assert(curv >= vmin);
		assert(curv < vmax);

#if defined(__riscv64__)
		{
			u64_t *entry = pt_get_pte(pt, curv);
			if (entry && (*entry & ARCH_VM_PTE_PRESENT)) {
				holev = NO_MEM;
				holesize = 0;
				goto nextaddr;
			}
		}
#else
		int pde = ARCH_VM_PDE(curv);
		pte = ARCH_VM_PTE(curv);

		if((pt->pt_dir[pde] & ARCH_VM_PDE_PRESENT) &&
		   (pt->pt_pt[pde][pte] & ARCH_VM_PTE_PRESENT)) {
			/* there is a page here - so keep looking for holes */
			holev = NO_MEM;
			holesize = 0;
		} else
#endif
		{
			/* there is no page here - so we have a hole, a bigger
			 * one if we already had one
			 */
			if(holev == NO_MEM) {
				holev = curv;
				holesize = 1;
			} else holesize++;

			assert(holesize > 0);
			assert(holesize <= pages);

			/* if it's big enough, return it */
			if(holesize == pages) {
				lastv = curv + VM_PAGE_SIZE;
				return holev;
			}
		}

nextaddr:
		curv+=VM_PAGE_SIZE;

		/* if we reached the limit, start scanning from the beginning if
		 * we haven't looked there yet
		 */
		if(curv >= vmax && try_restart) {
			try_restart = 0;
			curv = vmin;
		}
	}

	printf("VM: out of virtual address space in vm\n");

	return NO_MEM;
}

/*===========================================================================*
 *				vm_freepages		     		     *
 *===========================================================================*/
void vm_freepages(vir_bytes vir, int pages)
{
	assert(!(vir % VM_PAGE_SIZE)); 

	if(is_staticaddr(vir)) {
		printf("VM: not freeing static page\n");
		return;
	}

#if defined(__riscv64__)
	if (pt_init_done && vm_dmap_in_range(vir, pages)) {
		phys_bytes phys =
			VM_OWN_DMAP_PHYS_BASE +
			(phys_bytes)(vir - VM_OWN_DMAPBASE);
		free_mem(ABS2CLICK(phys), pages);
		return;
	}
#endif

	if(pt_writemap(vmprocess, &vmprocess->vm_pt, vir,
		MAP_NONE, pages*VM_PAGE_SIZE, 0,
		WMF_OVERWRITE | WMF_FREE) != OK)
		panic("vm_freepages: pt_writemap failed");

	vm_self_pages--;

#if SANITYCHECKS
	/* If SANITYCHECKS are on, flush tlb so accessing freed pages is
	 * always trapped, also if not in tlb.
	 */
	if((sys_vmctl(SELF, VMCTL_FLUSHTLB, 0)) != OK) {
		panic("VMCTL_FLUSHTLB failed");
	}
#endif
}

/*===========================================================================*
 *				vm_getsparepage		     		     *
 *===========================================================================*/
static void *vm_getsparepage(phys_bytes *phys)
{
	void *ptr;
	if(reservedqueue_alloc(spare_pagequeue, phys, &ptr) != OK) {
		return NULL;
	}
	assert(ptr);
	return ptr;
}

/*===========================================================================*
 *				vm_getsparepagedir	      		     *
 *===========================================================================*/
static void *vm_getsparepagedir(phys_bytes *phys)
{
	int s;
	assert(missing_sparedirs >= 0 && missing_sparedirs <= SPAREPAGEDIRS);
	for(s = 0; s < SPAREPAGEDIRS; s++) {
		if(sparepagedirs[s].pagedir) {
			void *sp;
			sp = sparepagedirs[s].pagedir;
			*phys = sparepagedirs[s].phys;
			sparepagedirs[s].pagedir = NULL;
			missing_sparedirs++;
			assert(missing_sparedirs >= 0 && missing_sparedirs <= SPAREPAGEDIRS);
			return sp;
		}
	}
	return NULL;
}

void *vm_mappages(phys_bytes p, int pages)
{
	vir_bytes loc;
	int r;
	pt_t *pt = &vmprocess->vm_pt;

	/* Where in our virtual address space can we put it? */
	loc = findhole(pages);
	if(loc == NO_MEM) {
		printf("vm_mappages: findhole failed\n");
		return NULL;
	}

	/* Map this page into our address space. */
	if((r=pt_writemap(vmprocess, pt, loc, p, VM_PAGE_SIZE*pages,
		ARCH_VM_PTE_PRESENT | ARCH_VM_PTE_USER | ARCH_VM_PTE_RW
#if defined(__arm__)
		| ARM_VM_PTE_CACHED
#endif
		, 0)) != OK) {
		printf("vm_mappages writemap failed\n");
		return NULL;
	}

	if((r=sys_vmctl(SELF, VMCTL_FLUSHTLB, 0)) != OK) {
		panic("VMCTL_FLUSHTLB failed: %d", r);
	}

	assert(loc);

	return (void *) loc;
}


/*===========================================================================*
 *				vm_allocpage		     		     *
 *===========================================================================*/
void *vm_allocpages(phys_bytes *phys, int reason, int pages)
{
/* Allocate a page for use by VM itself. */
	phys_bytes newpage;
	static int level = 0;
	void *ret;
	u32_t mem_flags = 0;

	assert(reason >= 0 && reason < VMP_CATEGORIES);

	assert(pages > 0);

	level++;

	assert(level >= 1);
	assert(level <= 2);

	if((level > 1) || !pt_init_done
#if defined(__riscv64__)
		|| pt_copying
#endif
		) {
		void *s;

		if(pages == 1) s=vm_getsparepage(phys);
		else if(pages == 4) s=vm_getsparepagedir(phys);
		else panic("%d pages", pages);

		level--;
		if(!s) {
			VM_STACKTRACE();
			printf("VM: warning: out of spare pages\n");
		}
		if(!is_staticaddr(s)) vm_self_pages++;
		return s;
	}

#if defined(__arm__)
	if (reason == VMP_PAGEDIR) {
		mem_flags |= PAF_ALIGN16K;
	}
#endif

	/* Allocate page of memory for use by VM. As VM
	 * is trusted, we don't have to pre-clear it.
	 */
	if((newpage = alloc_mem(pages, mem_flags)) == NO_MEM) {
		level--;
		printf("VM: vm_allocpage: alloc_mem failed\n");
		return NULL;
	}

	*phys = CLICK2ABS(newpage);

#if defined(__riscv64__)
	if (pt_init_done && pt_dmap_ready &&
	    (reason == VMP_PAGETABLE || reason == VMP_PAGEDIR) &&
	    vm_phys_in_dmap(*phys, pages)) {
		level--;
		vm_self_pages++;
		return (void *)(VM_OWN_DMAPBASE +
			(vir_bytes)(*phys - VM_OWN_DMAP_PHYS_BASE));
	}
#endif

	if(!(ret = vm_mappages(*phys, pages))) {
		level--;
		printf("VM: vm_allocpage: vm_mappages failed\n");
		return NULL;
	}

	level--;
	vm_self_pages++;

	return ret;
}

void *vm_allocpage(phys_bytes *phys, int reason)
{
	return vm_allocpages(phys, reason, 1);
}

/*===========================================================================*
 *				vm_pagelock		     		     *
 *===========================================================================*/
void vm_pagelock(void *vir, int lockflag)
{
/* Mark a page allocated by vm_allocpage() unwritable, i.e. only for VM. */
	vir_bytes m = (vir_bytes) vir;
	int r;
	u32_t flags = ARCH_VM_PTE_PRESENT | ARCH_VM_PTE_USER;
	pt_t *pt;

	pt = &vmprocess->vm_pt;

	assert(!(m % VM_PAGE_SIZE));

	if(!lockflag)
		flags |= ARCH_VM_PTE_RW;
#if defined(__arm__)
	else
		flags |= ARCH_VM_PTE_RO;

	flags |= ARM_VM_PTE_CACHED ;
#endif

	/* Update flags. */
	if((r=pt_writemap(vmprocess, pt, m, 0, VM_PAGE_SIZE,
		flags, WMF_OVERWRITE | WMF_WRITEFLAGSONLY)) != OK) {
		panic("vm_lockpage: pt_writemap failed");
	}

	if((r=sys_vmctl(SELF, VMCTL_FLUSHTLB, 0)) != OK) {
		panic("VMCTL_FLUSHTLB failed: %d", r);
	}

	return;
}

/*===========================================================================*
 *				vm_addrok		     		     *
 *===========================================================================*/
int vm_addrok(void *vir, int writeflag)
{
	pt_t *pt = &vmprocess->vm_pt;
	vir_bytes v = (vir_bytes) vir;

#if defined(__riscv64__)
	{
		u64_t *entry = pt_get_pte(pt, v);

		if (!entry) {
			printf("addr not ok: missing pte for 0x%lx\n",
			    (unsigned long)v);
			return 0;
		}
		if(!(*entry & ARCH_VM_PTE_PRESENT)) {
			printf("addr not ok: missing pte for 0x%lx\n",
			    (unsigned long)v);
			return 0;
		}
		if(writeflag && !(*entry & ARCH_VM_PTE_RW)) {
			printf("addr not ok: pte present but unwritable\n");
			return 0;
		}
		return 1;
	}
#else
	int pde, pte;

	pde = ARCH_VM_PDE(v);
	pte = ARCH_VM_PTE(v);

	if(!(pt->pt_dir[pde] & ARCH_VM_PDE_PRESENT)) {
		printf("addr not ok: missing pde %d\n", pde);
		return 0;
	}

#if defined(__i386__) || defined(__riscv64__)
	if(writeflag &&
		!(pt->pt_dir[pde] & ARCH_VM_PTE_RW)) {
		printf("addr not ok: pde %d present but pde unwritable\n", pde);
		return 0;
	}
#elif defined(__arm__)
	if(writeflag &&
		 (pt->pt_dir[pde] & ARCH_VM_PTE_RO)) {
		printf("addr not ok: pde %d present but pde unwritable\n", pde);
		return 0;
	}

#endif
	if(!(pt->pt_pt[pde][pte] & ARCH_VM_PTE_PRESENT)) {
		printf("addr not ok: missing pde %d / pte %d\n",
			pde, pte);
		return 0;
	}

#if defined(__i386__) || defined(__riscv64__)
	if(writeflag &&
		!(pt->pt_pt[pde][pte] & ARCH_VM_PTE_RW)) {
		printf("addr not ok: pde %d / pte %d present but unwritable\n",
			pde, pte);
#elif defined(__arm__)
	if(writeflag &&
		 (pt->pt_pt[pde][pte] & ARCH_VM_PTE_RO)) {
		printf("addr not ok: pde %d / pte %d present but unwritable\n",
			pde, pte);
#endif
		return 0;
	}

	return 1;
#endif
}

/*===========================================================================*
 *				pt_ptalloc		     		     *
 *===========================================================================*/
static int pt_ptalloc(pt_t *pt, int pde, u32_t flags)
{
/* Allocate a page table and write its address into the page directory. */
	int i;
	phys_bytes pt_phys;
#if defined(__riscv64__)
	u64_t *p;
	u64_t **l0;
	phys_bytes l0_phys;
#else
	u32_t *p;
#endif

	/* Argument must make sense. */
	assert(pde >= 0 && pde < ARCH_VM_DIR_ENTRIES);
	assert(!(flags & ~(PTF_ALLFLAGS)));

	/* We don't expect to overwrite page directory entry, nor
	 * storage for the page table.
	 */
	assert(!(pt->pt_dir[pde] & ARCH_VM_PDE_PRESENT));
	assert(!pt->pt_pt[pde]);

	/* Get storage for the page table. The allocation call may in fact
	 * recursively create the directory entry as a side effect. In that
	 * case, we free the newly allocated page and do nothing else.
	 */
	if (!(p = vm_allocpage(&pt_phys, VMP_PAGETABLE)))
		return ENOMEM;
	if (pt->pt_pt[pde]) {
		vm_freepages((vir_bytes) p, 1);
#if defined(__riscv64__)
		if (!pt->pt_pt_l0[pde]) {
			l0 = vm_allocpage(&l0_phys, VMP_PAGETABLE);
			if (!l0)
				return ENOMEM;
			memset(l0, 0, VM_PAGE_SIZE);
			pt->pt_pt_l0[pde] = l0;
		}
#endif
		assert(pt->pt_pt[pde]);
		return OK;
	}
	pt->pt_pt[pde] = p;

	for(i = 0; i < ARCH_VM_PT_ENTRIES; i++)
		pt->pt_pt[pde][i] = 0;	/* Empty entry. */

#if defined(__riscv64__)
	l0 = vm_allocpage(&l0_phys, VMP_PAGETABLE);
	if (!l0) {
		vm_freepages((vir_bytes)p, 1);
		pt->pt_pt[pde] = NULL;
		return ENOMEM;
	}
	memset(l0, 0, VM_PAGE_SIZE);
	pt->pt_pt_l0[pde] = l0;
#endif

	/* Make page directory entry.
	 * The PDE is always 'present,' 'writable,' and 'user accessible,'
	 * relying on the PTE for protection.
	 */
#if defined(__riscv64__)
	pt->pt_dir[pde] = RISCV_PA_TO_PTE(pt_phys);
#elif defined(__i386__)
	pt->pt_dir[pde] = (pt_phys & ARCH_VM_ADDR_MASK) | flags
		| ARCH_VM_PDE_PRESENT | ARCH_VM_PTE_USER | ARCH_VM_PTE_RW;
#elif defined(__arm__)
	pt->pt_dir[pde] = (pt_phys & ARCH_VM_PDE_MASK)
		| ARCH_VM_PDE_PRESENT | ARM_VM_PDE_DOMAIN; //LSC FIXME
#endif

	return OK;
}

/*===========================================================================*
 *			    pt_ptalloc_in_range		     		     *
 *===========================================================================*/
int pt_ptalloc_in_range(pt_t *pt, vir_bytes start, vir_bytes end,
	u32_t flags, int verify)
{
/* Allocate all the page tables in the range specified. */
	int pde, first_pde, last_pde;
#if defined(__riscv64__)
	int pte1, first_pte1, last_pte1;
#endif

	first_pde = ARCH_VM_PDE(start);
	last_pde = ARCH_VM_PDE(end-1);

	assert(first_pde >= 0);
	assert(last_pde < ARCH_VM_DIR_ENTRIES);

	/* Scan all page-directory entries in the range. */
	for(pde = first_pde; pde <= last_pde; pde++) {
		assert(!(pt->pt_dir[pde] & ARCH_VM_BIGPAGE));
#if defined(__riscv64__)
		if (pt->pt_dir[pde] & RISCV_PTE_LEAF) {
			int r = pt_l1alloc(pt, pde, verify);
			if (r != OK)
				return r;
		}
#endif
		if(!(pt->pt_dir[pde] & ARCH_VM_PDE_PRESENT)) {
			int r;
			if(verify) {
				printf("pt_ptalloc_in_range: no pde %d\n", pde);
				return EFAULT;
			}
			assert(!pt->pt_dir[pde]);
			if((r=pt_ptalloc(pt, pde, flags)) != OK) {
				/* Couldn't do (complete) mapping.
				 * Don't bother freeing any previously
				 * allocated page tables, they're
				 * still writable, don't point to nonsense,
				 * and pt_ptalloc leaves the directory
				 * and other data in a consistent state.
				 */
				return r;
			}
			assert(pt->pt_pt[pde]);
		}
		assert(pt->pt_pt[pde]);
		assert(pt->pt_dir[pde]);
		assert(pt->pt_dir[pde] & ARCH_VM_PDE_PRESENT);

#if defined(__riscv64__)
		first_pte1 = (pde == first_pde) ? ARCH_VM_PTE1(start) : 0;
		last_pte1 = (pde == last_pde) ? ARCH_VM_PTE1(end-1)
		    : ARCH_VM_PT_ENTRIES - 1;
		for(pte1 = first_pte1; pte1 <= last_pte1; pte1++) {
			if(!(pt->pt_pt[pde][pte1] & ARCH_VM_PDE_PRESENT)) {
				int r;
				if(verify) {
					printf("pt_ptalloc_in_range: no pde %d pte1 %d\n",
					    pde, pte1);
					return EFAULT;
				}
				if((r = pt_l0alloc(pt, pde, pte1, verify)) != OK)
					return r;
			}
		}
#endif
	}

	return OK;
}

static const char *ptestr(u64_t pte)
{
#define FLAG(constant, name) {						\
	if(pte & (constant)) { strcat(str, name); strcat(str, " "); }	\
}

	static char str[30];
	if(!(pte & ARCH_VM_PTE_PRESENT)) {
		return "not present";
	}
	str[0] = '\0';
#if defined(__i386__) || defined(__riscv64__)
	FLAG(ARCH_VM_PTE_RW, "W");
#elif defined(__arm__)
	if(pte & ARCH_VM_PTE_RO) {
	    strcat(str, "R ");
	} else {
	    strcat(str, "W ");
	}
#endif
	FLAG(ARCH_VM_PTE_USER, "U");
#if defined(__i386__)
	FLAG(I386_VM_PWT, "PWT");
	FLAG(I386_VM_PCD, "PCD");
	FLAG(I386_VM_ACC, "ACC");
	FLAG(I386_VM_DIRTY, "DIRTY");
	FLAG(I386_VM_PS, "PS");
	FLAG(I386_VM_GLOBAL, "G");
	FLAG(I386_VM_PTAVAIL1, "AV1");
	FLAG(I386_VM_PTAVAIL2, "AV2");
	FLAG(I386_VM_PTAVAIL3, "AV3");
#elif defined(__arm__)
	FLAG(ARM_VM_PTE_SUPER, "S");
	FLAG(ARM_VM_PTE_S, "SH");
	FLAG(ARM_VM_PTE_WB, "WB");
	FLAG(ARM_VM_PTE_WT, "WT");
#endif

	return str;
}

/*===========================================================================*
 *			     pt_map_in_range		     		     *
 *===========================================================================*/
int pt_map_in_range(struct vmproc *src_vmp, struct vmproc *dst_vmp,
	vir_bytes start, vir_bytes end)
{
/* Transfer all the mappings from the pt of the source process to the pt of
 * the destination process in the range specified.
 */
	int pde, pte;
#if defined(__riscv64__)
	int r;
#endif
	vir_bytes viraddr;
	pt_t *pt, *dst_pt;

	pt = &src_vmp->vm_pt;
	dst_pt = &dst_vmp->vm_pt;

	end = end ? end : VM_DATATOP;
	assert(start % VM_PAGE_SIZE == 0);
	assert(end % VM_PAGE_SIZE == 0);

	assert( /* ARCH_VM_PDE(start) >= 0 && */ start <= end);
	assert(ARCH_VM_PDE(end) < ARCH_VM_DIR_ENTRIES);

#if LU_DEBUG
	printf("VM: pt_map_in_range: src = %d, dst = %d\n",
		src_vmp->vm_endpoint, dst_vmp->vm_endpoint);
	printf("VM: pt_map_in_range: transferring from 0x%08x (pde %d pte %d) to 0x%08x (pde %d pte %d)\n",
		start, ARCH_VM_PDE(start), ARCH_VM_PTE(start),
		end, ARCH_VM_PDE(end), ARCH_VM_PTE(end));
#endif

	/* Scan all page-table entries in the range. */
	for(viraddr = start; viraddr <= end; viraddr += VM_PAGE_SIZE) {
		pde = ARCH_VM_PDE(viraddr);
#if defined(__riscv64__)
		{
			u64_t *src_entry;
			u64_t *dst_entry;
			int pte1;

			src_entry = pt_get_pte(pt, viraddr);
			if (!src_entry || !(*src_entry & ARCH_VM_PTE_PRESENT)) {
				if(viraddr == VM_DATATOP) break;
				continue;
			}

			if (!(dst_pt->pt_dir[pde] & ARCH_VM_PDE_PRESENT)) {
				r = pt_ptalloc(dst_pt, pde, 0);
				if (r != OK)
					return r;
			}
			pte1 = ARCH_VM_PTE1(viraddr);
			if ((r = pt_l0alloc(dst_pt, pde, pte1, 0)) != OK)
				return r;
			dst_entry = pt_get_pte(dst_pt, viraddr);
			if (!dst_entry)
				return EFAULT;
			*dst_entry = *src_entry;

			if(viraddr == VM_DATATOP) break;
			continue;
		}
#endif
		if(!(pt->pt_dir[pde] & ARCH_VM_PDE_PRESENT)) {
			if(viraddr == VM_DATATOP) break;
			continue;
		}
		pte = ARCH_VM_PTE(viraddr);
		if(!(pt->pt_pt[pde][pte] & ARCH_VM_PTE_PRESENT)) {
			if(viraddr == VM_DATATOP) break;
			continue;
		}

		/* Transfer the mapping. */
		dst_pt->pt_pt[pde][pte] = pt->pt_pt[pde][pte];
		assert(dst_pt->pt_pt[pde]);

                if(viraddr == VM_DATATOP) break;
	}

	return OK;
}

/*===========================================================================*
 *				pt_ptmap		     		     *
 *===========================================================================*/
int pt_ptmap(struct vmproc *src_vmp, struct vmproc *dst_vmp)
{
/* Transfer mappings to page dir and page tables from source process and
 * destination process.
 */
	int pde, r;
	phys_bytes physaddr;
	vir_bytes viraddr;
	pt_t *pt;

	pt = &src_vmp->vm_pt;

#if LU_DEBUG
	printf("VM: pt_ptmap: src = %d, dst = %d\n",
		src_vmp->vm_endpoint, dst_vmp->vm_endpoint);
#endif

	/* Transfer mapping to the page directory. */
	viraddr = (vir_bytes) pt->pt_dir;
	physaddr = pt->pt_dir_phys & ARCH_VM_ADDR_MASK;
#if defined(__i386__) || defined(__riscv64__)
	if((r=pt_writemap(dst_vmp, &dst_vmp->vm_pt, viraddr, physaddr, VM_PAGE_SIZE,
		ARCH_VM_PTE_PRESENT | ARCH_VM_PTE_USER | ARCH_VM_PTE_RW,
#elif defined(__arm__)
	if((r=pt_writemap(dst_vmp, &dst_vmp->vm_pt, viraddr, physaddr, ARCH_PAGEDIR_SIZE,
		ARCH_VM_PTE_PRESENT | ARCH_VM_PTE_USER |
		ARM_VM_PTE_CACHED ,
#endif
		WMF_OVERWRITE)) != OK) {
		return r;
	}
#if LU_DEBUG
	printf("VM: pt_ptmap: transferred mapping to page dir: 0x%08x (0x%08x)\n",
		viraddr, physaddr);
#endif

	/* Scan all non-reserved page-directory entries. */
	for(pde=0; pde < kern_start_pde; pde++) {
		if(!(pt->pt_dir[pde] & ARCH_VM_PDE_PRESENT)) {
			continue;
		}

		if(!pt->pt_pt[pde]) { panic("pde %d empty\n", pde); }

		/* Transfer mapping to the page table. */
		viraddr = (vir_bytes) pt->pt_pt[pde];
#if defined(__i386__)
		physaddr = pt->pt_dir[pde] & ARCH_VM_ADDR_MASK;
#elif defined(__riscv64__)
		physaddr = RISCV_PTE_TO_PA(pt->pt_dir[pde]);
#elif defined(__arm__)
		physaddr = pt->pt_dir[pde] & ARCH_VM_PDE_MASK;
#endif
		assert(viraddr);
		if((r=pt_writemap(dst_vmp, &dst_vmp->vm_pt, viraddr, physaddr, VM_PAGE_SIZE,
			ARCH_VM_PTE_PRESENT | ARCH_VM_PTE_USER | ARCH_VM_PTE_RW
#ifdef __arm__
			| ARM_VM_PTE_CACHED
#endif
			,
			WMF_OVERWRITE)) != OK) {
			return r;
		}

#if defined(__riscv64__)
		if (pt->pt_pt_l0[pde]) {
			int pte1;
			for (pte1 = 0; pte1 < ARCH_VM_PT_ENTRIES; pte1++) {
				if (!pt->pt_pt_l0[pde][pte1])
					continue;
				if (!(pt->pt_pt[pde][pte1] & ARCH_VM_PDE_PRESENT))
					continue;
				if (pt->pt_pt[pde][pte1] & RISCV_PTE_LEAF)
					continue;
				viraddr = (vir_bytes)pt->pt_pt_l0[pde][pte1];
				physaddr = RISCV_PTE_TO_PA(pt->pt_pt[pde][pte1]);
				if((r=pt_writemap(dst_vmp, &dst_vmp->vm_pt,
					viraddr, physaddr, VM_PAGE_SIZE,
					ARCH_VM_PTE_PRESENT | ARCH_VM_PTE_USER | ARCH_VM_PTE_RW,
					WMF_OVERWRITE)) != OK) {
					return r;
				}
			}
		}
#endif
	}

	return OK;
}

void pt_clearmapcache(void)
{
	/* Make sure kernel will invalidate tlb when using current
	 * pagetable (i.e. vm's) to make new mappings before new cr3
	 * is loaded.
	 */
	if(sys_vmctl(SELF, VMCTL_CLEARMAPCACHE, 0) != OK)
		panic("VMCTL_CLEARMAPCACHE failed");
}

int pt_writable(struct vmproc *vmp, vir_bytes v)
{
#if defined(__riscv64__)
	u64_t *entryp;
#else
	u64_t entry;
#endif
	pt_t *pt = &vmp->vm_pt;
	assert(!(v % VM_PAGE_SIZE));

#if defined(__riscv64__)
	entryp = pt_get_pte(pt, v);
	assert(entryp);
	return ((*entryp & PTF_WRITE) ? 1 : 0);
#else
	int pde = ARCH_VM_PDE(v);
	int pte = ARCH_VM_PTE(v);
	assert(pt->pt_dir[pde] & ARCH_VM_PDE_PRESENT);
	assert(pt->pt_pt[pde]);

	entry = pt->pt_pt[pde][pte];

#if defined(__i386__) || defined(__riscv64__)
	return((entry & PTF_WRITE) ? 1 : 0);
#elif defined(__arm__)
	return((entry & ARCH_VM_PTE_RO) ? 0 : 1);
#endif
#endif
}

/*===========================================================================*
 *				pt_writemap		     		     *
 *===========================================================================*/
int pt_writemap(struct vmproc * vmp,
			pt_t *pt,
			vir_bytes v,
			phys_bytes physaddr,
			size_t bytes,
			u32_t flags,
			u32_t writemapflags)
{
/* Write mapping into page table. Allocate a new page table if necessary. */
/* Page directory and table entries for this virtual address. */
	int p, pages;
	int verify = 0;
	int ret = OK;

#ifdef CONFIG_SMP
	int vminhibit_clear = 0;
	/* FIXME
	 * don't do it everytime, stop the process only on the first change and
	 * resume the execution on the last change. Do in a wrapper of this
	 * function
	 */
	if (vmp && vmp->vm_endpoint != NONE && vmp->vm_endpoint != VM_PROC_NR &&
			!(vmp->vm_flags & VMF_EXITING)) {
		sys_vmctl(vmp->vm_endpoint, VMCTL_VMINHIBIT_SET, 0);
		vminhibit_clear = 1;
	}
#endif

	if(writemapflags & WMF_VERIFY)
		verify = 1;

	assert(!(bytes % VM_PAGE_SIZE));
	assert(!(flags & ~(PTF_ALLFLAGS)));

	pages = bytes / VM_PAGE_SIZE;

	/* MAP_NONE means to clear the mapping. It doesn't matter
	 * what's actually written into the PTE if PRESENT
	 * isn't on, so we can just write MAP_NONE into it.
	 */
	assert(physaddr == MAP_NONE || (flags & ARCH_VM_PTE_PRESENT));
	assert(physaddr != MAP_NONE || !flags);

	/* First make sure all the necessary page tables are allocated,
	 * before we start writing in any of them, because it's a pain
	 * to undo our work properly.
	 */
	ret = pt_ptalloc_in_range(pt, v, v + VM_PAGE_SIZE*pages, flags, verify);
	if(ret != OK) {
		printf("VM: writemap: pt_ptalloc_in_range failed\n");
		goto resume_exit;
	}

	/* Now write in them. */
	for(p = 0; p < pages; p++) {
		u64_t entry;
#if defined(__riscv64__)
		u64_t *pte_ptr;
		int pte1 = ARCH_VM_PTE1(v);
#endif
		int pde = ARCH_VM_PDE(v);
		int pte = ARCH_VM_PTE(v);

		assert(!(v % VM_PAGE_SIZE));
		assert(pte >= 0 && pte < ARCH_VM_PT_ENTRIES);
		assert(pde >= 0 && pde < ARCH_VM_DIR_ENTRIES);

		/* Page table has to be there. */
		assert(pt->pt_dir[pde] & ARCH_VM_PDE_PRESENT);

		/* We do not expect it to be a bigpage. */
		assert(!(pt->pt_dir[pde] & ARCH_VM_BIGPAGE));
#if defined(__riscv64__)
		assert(!(pt->pt_dir[pde] & RISCV_PTE_LEAF));
#endif

		/* Make sure page directory entry for this page table
		 * is marked present and page table entry is available.
		 */
		assert(pt->pt_pt[pde]);

#if defined(__riscv64__)
		pte_ptr = pt_get_pte(pt, v);
		if (!pte_ptr) {
			if (verify) {
				ret = EFAULT;
				goto resume_exit;
			}
			if ((ret = pt_l0alloc(pt, pde, pte1, verify)) != OK)
				goto resume_exit;
			pte_ptr = pt_get_pte(pt, v);
			if (!pte_ptr) {
				ret = EFAULT;
				goto resume_exit;
			}
		}
#endif

		if(writemapflags & (WMF_WRITEFLAGSONLY|WMF_FREE)) {
#if defined(__i386__)
			physaddr = pt->pt_pt[pde][pte] & ARCH_VM_ADDR_MASK;
#elif defined(__riscv64__)
			physaddr = RISCV_PTE_TO_PA(*pte_ptr);
#elif defined(__arm__)
			physaddr = pt->pt_pt[pde][pte] & ARM_VM_PTE_MASK;
#endif
		}

		if(writemapflags & WMF_FREE) {
			free_mem(ABS2CLICK(physaddr), 1);
		}

		/* Entry we will write. */
#if defined(__i386__)
		entry = (physaddr & ARCH_VM_ADDR_MASK) | flags;
#elif defined(__riscv64__)
		if (physaddr == MAP_NONE)
			entry = 0;
		else {
			u64_t ad = RISCV_PTE_A;
			if (flags & ARCH_VM_PTE_RW)
				ad |= RISCV_PTE_D;
			entry = RISCV_PA_TO_PTE(physaddr) | flags | ad;
		}
#elif defined(__arm__)
		entry = (physaddr & ARM_VM_PTE_MASK) | flags;
#endif

		if(verify) {
			u64_t maskedentry;
#if defined(__riscv64__)
			maskedentry = *pte_ptr;
#else
			maskedentry = pt->pt_pt[pde][pte];
#endif
#if defined(__i386__)
			maskedentry &= ~(I386_VM_ACC|I386_VM_DIRTY);
#endif
			/* Verify pagetable entry. */
#if defined(__i386__) || defined(__riscv64__)
		if(entry & ARCH_VM_PTE_RW) {
				/* If we expect a writable page, allow a readonly page. */
				maskedentry |= ARCH_VM_PTE_RW;
			}
#elif defined(__arm__)
			if(!(entry & ARCH_VM_PTE_RO)) {
				/* If we expect a writable page, allow a readonly page. */
				maskedentry &= ~ARCH_VM_PTE_RO;
			}
			maskedentry &= ~(ARM_VM_PTE_WB|ARM_VM_PTE_WT);
#endif
			if(maskedentry != entry) {
				printf("pt_writemap: mismatch: ");
#if defined(__i386__)
				if((entry & ARCH_VM_ADDR_MASK) !=
					(maskedentry & ARCH_VM_ADDR_MASK)) {
#elif defined(__riscv64__)
				if(RISCV_PTE_TO_PA(entry) !=
					RISCV_PTE_TO_PA(maskedentry)) {
#elif defined(__arm__)
				if((entry & ARM_VM_PTE_MASK) !=
					(maskedentry & ARM_VM_PTE_MASK)) {
#endif
					printf("pt_writemap: physaddr mismatch (0x%llx, 0x%llx); ",
						(unsigned long long)entry,
						(unsigned long long)maskedentry);
				} else printf("phys ok; ");
				printf(" flags: found %s; ",
#if defined(__riscv64__)
					ptestr(*pte_ptr));
#else
					ptestr(pt->pt_pt[pde][pte]));
#endif
				printf(" masked %s; ",
					ptestr(maskedentry));
				printf(" expected %s\n", ptestr(entry));
				printf("found 0x%llx, wanted 0x%llx\n",
#if defined(__riscv64__)
					(unsigned long long)*pte_ptr,
#else
					(unsigned long long)pt->pt_pt[pde][pte],
#endif
					(unsigned long long)entry);
				ret = EFAULT;
				goto resume_exit;
			}
		} else {
			/* Write pagetable entry. */
#if defined(__riscv64__)
			*pte_ptr = entry;
#else
			pt->pt_pt[pde][pte] = entry;
#endif
		}

		physaddr += VM_PAGE_SIZE;
		v += VM_PAGE_SIZE;
	}

resume_exit:

#ifdef CONFIG_SMP
	if (vminhibit_clear) {
		assert(vmp && vmp->vm_endpoint != NONE && vmp->vm_endpoint != VM_PROC_NR &&
			!(vmp->vm_flags & VMF_EXITING));
		sys_vmctl(vmp->vm_endpoint, VMCTL_VMINHIBIT_CLEAR, 0);
	}
#endif

	return ret;
}

/*===========================================================================*
 *				pt_checkrange		     		     *
 *===========================================================================*/
int pt_checkrange(pt_t *pt, vir_bytes v,  size_t bytes,
	int write)
{
	int p, pages;

	assert(!(bytes % VM_PAGE_SIZE));

	pages = bytes / VM_PAGE_SIZE;

	for(p = 0; p < pages; p++) {
		int pde = ARCH_VM_PDE(v);
		int pte = ARCH_VM_PTE(v);
		assert(!(v % VM_PAGE_SIZE));
		assert(pte >= 0 && pte < ARCH_VM_PT_ENTRIES);
		assert(pde >= 0 && pde < ARCH_VM_DIR_ENTRIES);

#if defined(__riscv64__)
		u64_t *entry;
		entry = pt_get_pte(pt, v);
		if (!entry)
			return EFAULT;
		if(!(*entry & ARCH_VM_PTE_PRESENT))
			return EFAULT;
#else
		/* Page table has to be there. */
		if(!(pt->pt_dir[pde] & ARCH_VM_PDE_PRESENT))
			return EFAULT;

		/* Make sure page directory entry for this page table
		 * is marked present and page table entry is available.
		 */
		assert((pt->pt_dir[pde] & ARCH_VM_PDE_PRESENT) && pt->pt_pt[pde]);

		if(!(pt->pt_pt[pde][pte] & ARCH_VM_PTE_PRESENT)) {
			return EFAULT;
		}
#endif

#if defined(__i386__) || defined(__riscv64__)
	if(write &&
#if defined(__riscv64__)
		!(*entry & ARCH_VM_PTE_RW)) {
#else
		!(pt->pt_pt[pde][pte] & ARCH_VM_PTE_RW)) {
#endif
#elif defined(__arm__)
		if(write && (pt->pt_pt[pde][pte] & ARCH_VM_PTE_RO)) {
#endif
			return EFAULT;
		}

		v += VM_PAGE_SIZE;
	}

	return OK;
}

/*===========================================================================*
 *				pt_new			     		     *
 *===========================================================================*/
int pt_new(pt_t *pt)
{
/* Allocate a pagetable root. Allocate a page-aligned page directory
 * and set them to 0 (indicating no page tables are allocated). Lookup
 * its physical address as we'll need that in the future. Verify it's
 * page-aligned.
 */
	int i, r;

	/* Don't ever re-allocate/re-move a certain process slot's
	 * page directory once it's been created. This is a fraction
	 * faster, but also avoids having to invalidate the page
	 * mappings from in-kernel page tables pointing to
	 * the page directories (the page_directories data).
	 */
        if(!pt->pt_dir &&
          !(pt->pt_dir = vm_allocpages((phys_bytes *)&pt->pt_dir_phys,
	  	VMP_PAGEDIR, ARCH_PAGEDIR_SIZE/VM_PAGE_SIZE))) {
		return ENOMEM;
	}

	if (pt->pt_dir_phys % ARCH_PAGEDIR_SIZE) {
		char buf[128];
		int len;

		len = snprintf(buf, sizeof(buf),
			"VM: pt_new misaligned pagedir phys=0x%llx dir=%p pt=%p\n",
			(unsigned long long)pt->pt_dir_phys,
			pt->pt_dir, (void *)pt);
		if (len < 0)
			len = 0;
		if (len > (int)sizeof(buf))
			len = (int)sizeof(buf);
		sys_diagctl_diag(buf, len);
	}
	assert(!(pt->pt_dir_phys % ARCH_PAGEDIR_SIZE));

	for(i = 0; i < ARCH_VM_DIR_ENTRIES; i++) {
		pt->pt_dir[i] = 0; /* invalid entry (PRESENT bit = 0) */
		pt->pt_pt[i] = NULL;
#if defined(__riscv64__)
		pt->pt_pt_l0[i] = NULL;
#endif
	}

	/* Where to start looking for free virtual address space? */
	pt->pt_virtop = 0;

        /* Map in kernel. */
        if((r=pt_mapkernel(pt)) != OK)
		return r;

	return OK;
}

static int freepde(void)
{
	int p = kernel_boot_info.freepde_start++;
	assert(kernel_boot_info.freepde_start < ARCH_VM_DIR_ENTRIES);
	return p;
}

void pt_allocate_kernel_mapped_pagetables(void)
{
	/* Reserve PDEs available for mapping in the page directories. */
	int pd;
	for(pd = 0; pd < MAX_PAGEDIR_PDES; pd++) {
		struct pdm *pdm = &pagedir_mappings[pd];
		if(!pdm->pdeno)  {
			pdm->pdeno = freepde();
			assert(pdm->pdeno);
		}
		phys_bytes ph;

		/* Allocate us a page table in which to
		 * remember page directory pointers.
		 */
		if(!(pdm->page_directories =
			vm_allocpage(&ph, VMP_PAGETABLE))) {
			panic("no virt addr for vm mappings");
		}
		memset(pdm->page_directories, 0, VM_PAGE_SIZE);
		pdm->phys = ph;

#if defined(__riscv64__)
		{
			phys_bytes l0_phys;
			u64_t *l0;

			l0 = vm_allocpage(&l0_phys, VMP_PAGETABLE);
			if (!l0)
				panic("no virt addr for vm mappings l0");
			memset(l0, 0, VM_PAGE_SIZE);
			pdm->page_directories_l0 = l0;
			pdm->page_directories[0] = RISCV_PA_TO_PTE(l0_phys);
			pdm->val = RISCV_PA_TO_PTE(ph);
		}
#elif defined(__i386__)
		pdm->val = (ph & ARCH_VM_ADDR_MASK) |
			ARCH_VM_PDE_PRESENT | ARCH_VM_PTE_RW;
#elif defined(__arm__)
		pdm->val = (ph & ARCH_VM_PDE_MASK)
			| ARCH_VM_PDE_PRESENT
			| ARM_VM_PTE_CACHED
			| ARM_VM_PDE_DOMAIN; //LSC FIXME
#endif
	}
}

static void pt_copy(pt_t *dst, pt_t *src)
{
	int pde;
	for(pde=0; pde < kern_start_pde; pde++) {
#if defined(__riscv64__)
		int pd;
		int skip = 0;

		for (pd = 0; pd < MAX_PAGEDIR_PDES; pd++) {
			if (pagedir_mappings[pd].pdeno == pde) {
				skip = 1;
				break;
			}
		}
		if (skip)
			continue;
#endif
		if(!(src->pt_dir[pde] & ARCH_VM_PDE_PRESENT)) {
			continue;
		}
#if defined(__riscv64__)
		if(src->pt_dir[pde] & RISCV_PTE_LEAF) {
			dst->pt_dir[pde] = src->pt_dir[pde];
			continue;
		}
#else
		assert(!(src->pt_dir[pde] & ARCH_VM_BIGPAGE));
#endif
		if(!src->pt_pt[pde]) { panic("pde %d empty\n", pde); }
#if defined(__riscv64__)
		if(!(dst->pt_dir[pde] & ARCH_VM_PDE_PRESENT)) {
			if(pt_ptalloc(dst, pde, 0) != OK)
				panic("pt_ptalloc failed");
		} else if (dst->pt_dir[pde] & RISCV_PTE_LEAF) {
			continue;
		}
		if(!dst->pt_pt[pde]) { panic("dst pde %d empty\n", pde); }
		if(!dst->pt_pt_l0[pde]) {
			phys_bytes l0_phys;
			u64_t **l0 = vm_allocpage(&l0_phys, VMP_PAGETABLE);
			if (!l0)
				panic("pt_copy l0 root alloc failed");
			memset(l0, 0, VM_PAGE_SIZE);
			dst->pt_pt_l0[pde] = l0;
		}
		{
			int pte1;
			for(pte1 = 0; pte1 < ARCH_VM_PT_ENTRIES; pte1++) {
				u64_t entry = src->pt_pt[pde][pte1];
				if(!(entry & ARCH_VM_PDE_PRESENT))
					continue;
				if(entry & RISCV_PTE_LEAF) {
					dst->pt_pt[pde][pte1] = entry;
					continue;
				}
				if(!src->pt_pt_l0[pde] || !src->pt_pt_l0[pde][pte1])
					panic("pt_copy missing l0 pde %d pte1 %d\n", pde, pte1);
				phys_bytes l0_phys;
				u64_t *dst_l0 = vm_allocpage(&l0_phys, VMP_PAGETABLE);
				if (!dst_l0)
					panic("pt_copy l0 alloc failed");
				memcpy(dst_l0, src->pt_pt_l0[pde][pte1],
					ARCH_VM_PT_ENTRIES * sizeof(*dst_l0));
				dst->pt_pt_l0[pde][pte1] = dst_l0;
				dst->pt_pt[pde][pte1] = RISCV_PA_TO_PTE(l0_phys);
			}
		}
#else
		if(pt_ptalloc(dst, pde, 0) != OK)
			panic("pt_ptalloc failed");
		memcpy(dst->pt_pt[pde], src->pt_pt[pde],
			ARCH_VM_PT_ENTRIES * sizeof(*dst->pt_pt[pde]));
#endif
	}
}

/*===========================================================================*
 *                              pt_init                                      *
 *===========================================================================*/
void pt_init(void)
{
        pt_t *newpt, newpt_dyn;
        int s, r, p;
	phys_bytes phys;
	vir_bytes sparepages_mem;
#if defined(__arm__)
	vir_bytes sparepagedirs_mem;
#endif
	#if defined(__riscv64__)
	static u64_t currentpagedir[ARCH_VM_DIR_ENTRIES];
	#else
	static u32_t currentpagedir[ARCH_VM_DIR_ENTRIES];
#endif
	static int bad_pt_copy_logged;
	int m = kernel_boot_info.kern_mod;
#if defined(__i386__)
	int global_bit_ok = 0;
	u32_t mypdbr; /* Page Directory Base Register (cr3) value */
#elif defined(__arm__)
	u32_t myttbr;
#elif defined(__riscv64__)
	phys_bytes mypdbr;
#endif

	/* Find what the physical location of the kernel is. */
	assert(m >= 0);
	assert(m < kernel_boot_info.mods_with_kernel);
	assert(kernel_boot_info.mods_with_kernel < MULTIBOOT_MAX_MODS);
	kern_mb_mod = &kernel_boot_info.module_list[m];
	kern_size = kern_mb_mod->mod_end - kern_mb_mod->mod_start;
#if defined(__riscv64__)
	kern_phys_base = kern_mb_mod->mod_start & ~(ARCH_BIG_PAGE_SIZE - 1);
#else
	assert(!(kern_mb_mod->mod_start % ARCH_BIG_PAGE_SIZE));
#endif
	assert(!(kernel_boot_info.vir_kern_start % ARCH_BIG_PAGE_SIZE));
#if defined(__riscv64__)
	kern_start_pde = ARCH_VM_PDE(kernel_boot_info.vir_kern_start);
#else
	kern_start_pde = kernel_boot_info.vir_kern_start / ARCH_BIG_PAGE_SIZE;
#endif
        /* Get ourselves spare pages. */
        sparepages_mem = (vir_bytes) static_sparepages;
	assert(!(sparepages_mem % VM_PAGE_SIZE));

#if defined(__arm__)
        /* Get ourselves spare pagedirs. */
	sparepagedirs_mem = (vir_bytes) static_sparepagedirs;
	assert(!(sparepagedirs_mem % ARCH_PAGEDIR_SIZE));
#endif

	/* Spare pages are used to allocate memory before VM has its own page
	 * table that things (i.e. arbitrary physical memory) can be mapped into.
	 * We get it by pre-allocating it in our bss (allocated and mapped in by
	 * the kernel) in static_sparepages. We also need the physical addresses
	 * though; we look them up now so they are ready for use.
	 */
#if defined(__arm__)
        missing_sparedirs = 0;
        assert(STATIC_SPAREPAGEDIRS <= SPAREPAGEDIRS);
        for(s = 0; s < SPAREPAGEDIRS; s++) {
		vir_bytes v = (sparepagedirs_mem + s*ARCH_PAGEDIR_SIZE);;
		phys_bytes ph;
        	if((r=sys_umap(SELF, VM_D, (vir_bytes) v,
	                ARCH_PAGEDIR_SIZE, &ph)) != OK)
				panic("pt_init: sys_umap failed: %d", r);
        	if(s >= STATIC_SPAREPAGEDIRS) {
        		sparepagedirs[s].pagedir = NULL;
        		missing_sparedirs++;
        		continue;
        	}
        	sparepagedirs[s].pagedir = (void *) v;
        	sparepagedirs[s].phys = ph;
        }
#endif

	if(!(spare_pagequeue = reservedqueue_new(SPAREPAGES, 1, 1, 0)))
		panic("reservedqueue_new for single pages failed");

	assert(STATIC_SPAREPAGES < SPAREPAGES);
#if defined(__riscv64__)
	printf("VM: mapping %d static spare pages\n", STATIC_SPAREPAGES);
#endif
	for(s = 0; s < STATIC_SPAREPAGES; s++) {
		void *v = (void *) (sparepages_mem + s*VM_PAGE_SIZE);
		phys_bytes ph;
		if((r=sys_umap(SELF, VM_D, (vir_bytes) v,
	                VM_PAGE_SIZE, &ph)) != OK)
				panic("pt_init: sys_umap failed: %d", r);
		reservedqueue_add(spare_pagequeue, v, ph);
	}
#if defined(__riscv64__)
	printf("VM: static spare pages mapped\n");
#endif

#if defined(__i386__)
	/* global bit and 4MB pages available? */
	global_bit_ok = _cpufeature(_CPUF_I386_PGE);
	bigpage_ok = _cpufeature(_CPUF_I386_PSE);

	/* Set bit for PTE's and PDE's if available. */
	if(global_bit_ok)
		global_bit = I386_VM_GLOBAL;
#endif

	/* Now reserve another pde for kernel's own mappings. */
	{
		int kernmap_pde;
		phys_bytes addr, len;
		int flags, pindex = 0;
		vir_bytes offset = 0;

		kernmap_pde = freepde();
		offset = kernmap_pde * ARCH_BIG_PAGE_SIZE;

		while(sys_vmctl_get_mapping(pindex, &addr, &len,
			&flags) == OK)  {
			int usedpde;
			vir_bytes vir;
			if(pindex >= MAX_KERNMAPPINGS)
                		panic("VM: too many kernel mappings: %d", pindex);
			kern_mappings[pindex].phys_addr = addr;
			kern_mappings[pindex].len = len;
			kern_mappings[pindex].flags = flags;
			kern_mappings[pindex].vir_addr = offset;
			kern_mappings[pindex].flags =
				ARCH_VM_PTE_PRESENT;
#if defined(__riscv64__)
			kern_mappings[pindex].flags |= RISCV_PTE_R;
#endif
#if defined(__arm__)
			if(flags & VMMF_UNCACHED)
				kern_mappings[pindex].flags |= ARM_VM_PTE_DEVICE;
			else
				kern_mappings[pindex].flags |= ARM_VM_PTE_CACHED;
#else
			if(flags & VMMF_UNCACHED) {
#if defined(__i386__)
				kern_mappings[pindex].flags |= PTF_NOCACHE;
#endif
			}
#endif
			if(flags & VMMF_USER)
				kern_mappings[pindex].flags |= ARCH_VM_PTE_USER;
#if defined(__arm__)
			else
				kern_mappings[pindex].flags |= ARM_VM_PTE_SUPER;
#endif
			if(flags & VMMF_WRITE)
				kern_mappings[pindex].flags |= ARCH_VM_PTE_RW;
#if defined(__arm__)
			else 
				kern_mappings[pindex].flags |= ARCH_VM_PTE_RO;
#endif
			if (pindex == 0) {
				printf("VM: kernmap set flags=0x%x userflag=0x%x\n",
				    kern_mappings[pindex].flags,
				    ARCH_VM_PTE_USER);
			}

#if defined(__i386__)
			if(flags & VMMF_GLO)
				kern_mappings[pindex].flags |= I386_VM_GLOBAL;
#endif

			if(addr % VM_PAGE_SIZE)
                		panic("VM: addr unaligned: %lu", addr);
			if(len % VM_PAGE_SIZE)
                		panic("VM: len unaligned: %lu", len);
			vir = offset;
			if (flags & VMMF_USER) {
				printf("VM: kernmap user pindex=%d phys=0x%llx len=0x%llx flags=0x%x vir=0x%lx\n",
				    pindex, (unsigned long long)addr,
				    (unsigned long long)len, flags,
				    (unsigned long)vir);
			}
			if(sys_vmctl_reply_mapping(pindex, vir) != OK)
                		panic("VM: reply failed");
			offset += len;
			pindex++;
			kernmappings++;

			usedpde = ARCH_VM_PDE(offset);
			while(usedpde > kernmap_pde) {
				int newpde = freepde();
				assert(newpde == kernmap_pde+1);
				kernmap_pde = newpde;
			}
		}
	}

	pt_allocate_kernel_mapped_pagetables();
#if defined(__riscv64__)
	printf("VM: pt_init kernel mapped page tables allocated\n");
#endif

	/* Allright. Now. We have to make our own page directory and page tables,
	 * that the kernel has already set up, accessible to us. It's easier to
	 * understand if we just copy all the required pages (i.e. page directory
	 * and page tables), and set up the pointers as if VM had done it itself.
	 *
	 * This allocation will happen without using any page table, and just
	 * uses spare pages.
	 */
        newpt = &vmprocess->vm_pt;
#if defined(__riscv64__)
	printf("VM: pt_init newpt alloc\n");
#endif
	if(pt_new(newpt) != OK)
		panic("vm pt_new failed");
#if defined(__riscv64__)
	printf("VM: pt_init newpt ready\n");
#endif

	/* Get our current pagedir so we can see it. */
#if defined(__i386__)
	if(sys_vmctl_get_pdbr(SELF, &mypdbr) != OK)
#elif defined(__arm__)
	if(sys_vmctl_get_pdbr(SELF, &myttbr) != OK)
#elif defined(__riscv64__)
	if(sys_vmctl_get_pdbr(SELF, &mypdbr) != OK)
#endif

		panic("VM: sys_vmctl_get_pdbr failed");
#if defined(__i386__)
	if(sys_vircopy(NONE, mypdbr, SELF,
		(vir_bytes) currentpagedir, VM_PAGE_SIZE, 0) != OK)
#elif defined(__arm__)
	if(sys_vircopy(NONE, myttbr, SELF,
		(vir_bytes) currentpagedir, ARCH_PAGEDIR_SIZE, 0) != OK)
#elif defined(__riscv64__)
	if(sys_vircopy(NONE, mypdbr, SELF,
		(vir_bytes) currentpagedir, ARCH_PAGEDIR_SIZE, 0) != OK)
#endif
		panic("VM: sys_vircopy failed");

	/* We have mapped in kernel ourselves; now copy mappings for VM
	 * that kernel made, including allocations for BSS. Skip identity
	 * mapping bits; just map in VM.
	 */
#if defined(__riscv64__)
	printf("VM: pt_init copy pagetables start\n");
#endif
	for(p = 0; p < ARCH_VM_DIR_ENTRIES; p++) {
		u64_t entry = currentpagedir[p];
		phys_bytes ptaddr_kern, ptaddr_us;

#if defined(__riscv64__)
		if ((p % 128) == 0)
			printf("VM: pt_init copy pde %d\n", p);
#endif

		/* BIGPAGEs are kernel mapping (do ourselves) or boot
		 * identity mapping (don't want).
		 */
		if(!(entry & ARCH_VM_PDE_PRESENT)) continue;
#if defined(__riscv64__)
		if(entry & RISCV_PTE_LEAF) continue;
#else
		if((entry & ARCH_VM_BIGPAGE)) continue;
#endif

		#if defined(__riscv64__)
		if(!(newpt->pt_dir[p] & ARCH_VM_PDE_PRESENT)) {
			if(pt_ptalloc(newpt, p, 0) != OK)
				panic("pt_ptalloc failed");
		}
		#else
		if(pt_ptalloc(newpt, p, 0) != OK)
			panic("pt_ptalloc failed");
		#endif
		assert(newpt->pt_dir[p] & ARCH_VM_PDE_PRESENT);

#if defined(__i386__)
		ptaddr_kern = entry & ARCH_VM_ADDR_MASK;
		ptaddr_us = newpt->pt_dir[p] & ARCH_VM_ADDR_MASK;
#elif defined(__arm__)
		ptaddr_kern = entry & ARCH_VM_PDE_MASK;
		ptaddr_us = newpt->pt_dir[p] & ARCH_VM_PDE_MASK;
#elif defined(__riscv64__)
		ptaddr_kern = RISCV_PTE_TO_PA(entry);
		ptaddr_us = RISCV_PTE_TO_PA(((u64_t *)newpt->pt_dir)[p]);
#endif

		if (!bad_pt_copy_logged &&
			(ptaddr_kern >= 0x100000000ULL ||
			 ptaddr_us >= 0x100000000ULL)) {
			char buf[160];
			int len = snprintf(buf, sizeof(buf),
			    "VM: pt_init bad pt copy p=%d entry=0x%llx kern=0x%llx us=0x%llx\n",
			    p, (unsigned long long)entry,
			    (unsigned long long)ptaddr_kern,
			    (unsigned long long)ptaddr_us);
			if (len < 0)
				len = 0;
			if (len > (int)sizeof(buf))
				len = (int)sizeof(buf);
			sys_diagctl_diag(buf, len);
			bad_pt_copy_logged = 1;
		}

		/* Copy kernel-initialized pagetable contents into our
		 * normally accessible pagetable.
		 */
                if(sys_abscopy(ptaddr_kern, ptaddr_us, VM_PAGE_SIZE) != OK)
			panic("pt_init: abscopy failed");
#if defined(__riscv64__)
		{
			int pte1;
			for(pte1 = 0; pte1 < ARCH_VM_PT_ENTRIES; pte1++) {
				u64_t l1_entry = newpt->pt_pt[p][pte1];
				phys_bytes l0_phys;
				u64_t *l0;

				if(!(l1_entry & ARCH_VM_PDE_PRESENT))
					continue;
				if(l1_entry & RISCV_PTE_LEAF)
					continue;
				if(!newpt->pt_pt_l0[p])
					panic("pt_init: missing l0 map\n");
				l0 = vm_allocpage(&l0_phys, VMP_PAGETABLE);
				if(!l0)
					panic("pt_init: l0 alloc failed");
				if(sys_abscopy(RISCV_PTE_TO_PA(l1_entry), l0_phys,
				    VM_PAGE_SIZE) != OK)
					panic("pt_init: l0 abscopy failed");
				newpt->pt_pt_l0[p][pte1] = l0;
				newpt->pt_pt[p][pte1] = RISCV_PA_TO_PTE(l0_phys);
			}
		}
#endif
	}
#if defined(__riscv64__)
	printf("VM: pt_init copy pagetables end\n");
	printf("VM: pt_init pte2=0x%llx kern_pde=%d val=0x%llx\n",
		(unsigned long long)newpt->pt_dir[2],
		kern_start_pde,
		(unsigned long long)newpt->pt_dir[kern_start_pde]);
	if (sys_vircopy(NONE, newpt->pt_dir_phys, SELF,
	    (vir_bytes)currentpagedir, ARCH_PAGEDIR_SIZE, 0) == OK) {
		printf("VM: pt_init pte2_phys=0x%llx\n",
			(unsigned long long)currentpagedir[2]);
	}
	{
		phys_bytes ph_check;
		if (sys_umap(SELF, VM_D, (vir_bytes)newpt->pt_dir,
		    VM_PAGE_SIZE, &ph_check) == OK) {
			printf("VM: pt_init pd_phys=0x%llx check=0x%llx\n",
				(unsigned long long)newpt->pt_dir_phys,
				(unsigned long long)ph_check);
		}
	}
#endif

	/* Inform kernel vm has a newly built page table. */
	assert(vmproc[VM_PROC_NR].vm_endpoint == VM_PROC_NR);
#if defined(__riscv64__)
	printf("VM: pt_init before pt_bind\n");
	pt_verify_kernel_mappings(newpt, "pt_init bind");
#endif
	pt_bind(newpt, &vmproc[VM_PROC_NR]);
#if defined(__riscv64__)
	printf("VM: pt_init bound newpt\n");
#endif

	pt_init_done = 1;
#if defined(__riscv64__)
	{
		u64_t *entry = pt_get_pte(&vmprocess->vm_pt, VM_OWN_DMAPBASE);
		if (entry &&
		    ((*entry & (ARCH_VM_PTE_PRESENT | ARCH_VM_PTE_USER |
		    ARCH_VM_PTE_RW)) ==
		    (ARCH_VM_PTE_PRESENT | ARCH_VM_PTE_USER |
		    ARCH_VM_PTE_RW)))
			pt_dmap_ready = 1;
	}
#endif

	/* VM is now fully functional in that it can dynamically allocate memory
	 * for itself.
	 *
	 * We don't want to keep using the bootstrap statically allocated spare
	 * pages though, as the physical addresses will change on liveupdate. So we
	 * re-do part of the initialization now with purely dynamically allocated
	 * memory. First throw out the static pool.
	 *
	 * Then allocate the kernel-shared-pagetables and VM pagetables with dynamic
	 * memory.
	 */

	alloc_cycle();                          /* Make sure allocating works */
	while(vm_getsparepage(&phys)) ;		/* Use up all static pages */
	alloc_cycle();                          /* Refill spares with dynamic */
	pt_allocate_kernel_mapped_pagetables(); /* Reallocate in-kernel pages */
	pt_bind(newpt, &vmproc[VM_PROC_NR]);    /* Recalculate */
	pt_mapkernel(newpt);                    /* Rewrite pagetable info */
#if defined(__riscv64__)
	pt_verify_kernel_mappings(newpt, "pt_init remap");
#endif

	/* Flush TLB just in case any of those mappings have been touched */
	if((sys_vmctl(SELF, VMCTL_FLUSHTLB, 0)) != OK) {
		panic("VMCTL_FLUSHTLB failed");
	}

	/* Recreate VM page table with dynamic-only allocations */
	memset(&newpt_dyn, 0, sizeof(newpt_dyn));
	pt_new(&newpt_dyn);
#if defined(__riscv64__)
	pt_copying = 1;
#endif
	pt_copy(&newpt_dyn, newpt);
#if defined(__riscv64__)
	pt_copying = 0;
	pt_verify_kernel_mappings(&newpt_dyn, "pt_copy");
#endif
	memcpy(newpt, &newpt_dyn, sizeof(*newpt));

	pt_bind(newpt, &vmproc[VM_PROC_NR]);    /* Recalculate */
	pt_mapkernel(newpt);                    /* Rewrite pagetable info */
#if defined(__riscv64__)
	pt_verify_kernel_mappings(newpt, "pt_init dyn");
#endif

	/* Flush TLB just in case any of those mappings have been touched */
	if((sys_vmctl(SELF, VMCTL_FLUSHTLB, 0)) != OK) {
		panic("VMCTL_FLUSHTLB failed");
	}

#if defined(__riscv64__)
	{
		int i;
		for (i = 0; i < kernmappings; i++) {
			u64_t *entry;
			printf("VM: kernmap check i=%d vir=0x%lx flags=0x%x\n",
			    i, (unsigned long)kern_mappings[i].vir_addr,
			    kern_mappings[i].flags);
			entry = pt_get_pte(&vmprocess->vm_pt, kern_mappings[i].vir_addr);
			if (entry) {
				printf("VM: kernmap pte vir=0x%lx entry=0x%llx\n",
				    (unsigned long)kern_mappings[i].vir_addr,
				    (unsigned long long)*entry);
			} else {
				printf("VM: kernmap pte missing vir=0x%lx\n",
				    (unsigned long)kern_mappings[i].vir_addr);
			}
		}
	}
#endif
	printf("VM: pt_init end\n");

        /* All OK. */
        return;
}

/*===========================================================================*
 *				pt_bind			     		     *
 *===========================================================================*/
int pt_bind(pt_t *pt, struct vmproc *who)
{
	int procslot, pdeslot;
	phys_bytes phys;
	void *pdes;
	int pagedir_pde;
	int slots_per_pde;
	int pages_per_pagedir = ARCH_PAGEDIR_SIZE/VM_PAGE_SIZE;
	struct pdm *pdm;

	slots_per_pde = ARCH_VM_PT_ENTRIES / pages_per_pagedir;

	/* Basic sanity checks. */
	assert(who);
	assert(who->vm_flags & VMF_INUSE);
	assert(pt);

	procslot = who->vm_slot;
	pdm = &pagedir_mappings[procslot/slots_per_pde];
	pdeslot = procslot%slots_per_pde;
	pagedir_pde = pdm->pdeno;
	assert(pdeslot >= 0);
	assert(procslot < ELEMENTS(vmproc));
	assert(pdeslot < ARCH_VM_PT_ENTRIES / pages_per_pagedir);
	assert(pagedir_pde >= 0);

#if defined(__i386__) || defined(__riscv64__)
	phys = pt->pt_dir_phys & ARCH_VM_ADDR_MASK;
#elif defined(__arm__)
	phys = pt->pt_dir_phys & ARM_VM_PTE_MASK;
#endif
	assert(pt->pt_dir_phys == phys);
	assert(!(pt->pt_dir_phys % ARCH_PAGEDIR_SIZE));

	/* Update "page directory pagetable." */
#if defined(__i386__)
	pdm->page_directories[pdeslot] =
		phys | ARCH_VM_PDE_PRESENT|ARCH_VM_PTE_RW;
#elif defined(__riscv64__)
	if (!pdm->page_directories_l0)
		panic("pt_bind: missing pagedir l0");
	pdm->page_directories_l0[pdeslot] =
		RISCV_PA_TO_PTE(phys) | ARCH_VM_PTE_RW |
		RISCV_PTE_A | RISCV_PTE_D;
#elif defined(__arm__)
{
	int i;
	for (i = 0; i < pages_per_pagedir; i++) {
		pdm->page_directories[pdeslot*pages_per_pagedir+i] =
			(phys+i*VM_PAGE_SIZE)
			| ARCH_VM_PTE_PRESENT
			| ARCH_VM_PTE_RW
			| ARM_VM_PTE_CACHED
			| ARCH_VM_PTE_USER; //LSC FIXME
	}
}
#endif

	/* This is where the PDE's will be visible to the kernel
	 * in its address space.
	 */
	pdes = (void *) (pagedir_pde*ARCH_BIG_PAGE_SIZE + 
#if defined(__i386__) || defined(__riscv64__)
			pdeslot * VM_PAGE_SIZE);
#elif defined(__arm__)
			pdeslot * ARCH_PAGEDIR_SIZE);
#endif

	/* Tell kernel about new page table root. */
	{
		int r = sys_vmctl_set_addrspace(who->vm_endpoint, pt->pt_dir_phys, pdes);
#if defined(__riscv64__)
		printf("VM: pt_bind set_addrspace r=%d\n", r);
		if (r == OK && who->vm_endpoint == VM_PROC_NR) {
			phys_bytes pdbr = 0;
			int r2 = sys_vmctl_get_pdbr(SELF, &pdbr);
			if (r2 == OK) {
				if (pdbr != pt->pt_dir_phys) {
					printf("VM: pt_bind pdbr mismatch pdbr=0x%llx want=0x%llx\n",
					    (unsigned long long)pdbr,
					    (unsigned long long)pt->pt_dir_phys);
					r = sys_vmctl_set_addrspace(who->vm_endpoint,
					    pt->pt_dir_phys, pdes);
				}
			} else {
				printf("VM: pt_bind get_pdbr failed r=%d\n", r2);
			}
			pt_verify_kernel_mappings(pt, "pt_bind");
		}
#endif
		return r;
	}
}

/*===========================================================================*
 *				pt_free			     		     *
 *===========================================================================*/
void pt_free(pt_t *pt)
{
/* Free memory associated with this pagetable. */
	int i;

#if defined(__riscv64__)
	for(i = 0; i < ARCH_VM_DIR_ENTRIES; i++) {
		if(pt->pt_pt_l0[i]) {
			int j;
			for(j = 0; j < ARCH_VM_PT_ENTRIES; j++) {
				if(pt->pt_pt_l0[i][j])
					vm_freepages((vir_bytes) pt->pt_pt_l0[i][j], 1);
			}
			vm_freepages((vir_bytes) pt->pt_pt_l0[i], 1);
			pt->pt_pt_l0[i] = NULL;
		}
		if(pt->pt_pt[i]) {
			vm_freepages((vir_bytes) pt->pt_pt[i], 1);
			pt->pt_pt[i] = NULL;
		}
	}
	return;
#else
	for(i = 0; i < ARCH_VM_DIR_ENTRIES; i++)
		if(pt->pt_pt[i])
			vm_freepages((vir_bytes) pt->pt_pt[i], 1);

	return;
#endif
}

/*===========================================================================*
 *				pt_mapkernel		     		     *
 *===========================================================================*/
int pt_mapkernel(pt_t *pt)
{
	int i;
	int kern_pde = kern_start_pde;
	phys_bytes addr, mapped = 0;

        /* Any page table needs to map in the kernel address space. */
	assert(bigpage_ok);
	assert(kern_pde >= 0);

	/* pt_init() has made sure this is ok. */
#if defined(__riscv64__)
	addr = kern_phys_base;
#else
	addr = kern_mb_mod->mod_start;
#endif

	/* Actually mapping in kernel */
	while(mapped < kern_size) {
#if defined(__i386__)
		pt->pt_dir[kern_pde] = addr | ARCH_VM_PDE_PRESENT |
			ARCH_VM_BIGPAGE | ARCH_VM_PTE_RW | global_bit;
#elif defined(__riscv64__)
	u64_t kflags = RISCV_PTE_R | RISCV_PTE_W | RISCV_PTE_X |
		RISCV_PTE_G | RISCV_PTE_A | RISCV_PTE_D;
	u64_t idflags = kflags;

	if (mapped == 0) {
		/* Identity map DRAM window at 0x80000000. */
		pt->pt_dir[2] = RISCV_PA_TO_PTE(0x80000000UL) | idflags;
	}

	/* Map kernel in the high half. */
	pt->pt_dir[kern_pde] = RISCV_PA_TO_PTE(addr) | kflags;
	if (pt == &vmprocess->vm_pt) {
		int r;
		u32_t dmap_flags = ARCH_VM_PTE_PRESENT |
			ARCH_VM_PTE_RW | ARCH_VM_PTE_USER;

		r = pt_writemap(NULL, pt, VM_OWN_DMAPBASE,
			VM_OWN_DMAP_PHYS_BASE,
			VM_OWN_DMAPTOP - VM_OWN_DMAPBASE, dmap_flags, 0);
		if (r != OK)
			return r;
	}
#elif defined(__arm__)
		pt->pt_dir[kern_pde] = (addr & ARM_VM_SECTION_MASK)
			| ARM_VM_SECTION
			| ARM_VM_SECTION_DOMAIN
			| ARM_VM_SECTION_CACHED
			| ARM_VM_SECTION_SUPER;
#endif
		kern_pde++;
		mapped += ARCH_BIG_PAGE_SIZE;
		addr += ARCH_BIG_PAGE_SIZE;
	}

#if defined(__riscv64__)
	{
		struct {
			phys_bytes base;
			phys_bytes len;
		} devs[] = {
			{ 0x02000000UL, 0x10000UL },   /* CLINT */
			{ 0x0C000000UL, 0x4000000UL }, /* PLIC */
			{ 0x10000000UL, 0x1000UL },    /* UART */
			{ 0x10001000UL, 0x1000UL },    /* VirtIO MMIO */
		};
		int r;

		for(i = 0; i < (int)(sizeof(devs)/sizeof(devs[0])); i++) {
			r = pt_writemap(NULL, pt, devs[i].base, devs[i].base,
				devs[i].len, ARCH_VM_PTE_PRESENT | ARCH_VM_PTE_RW, 0);
			if(r != OK)
				return r;
		}
	}
#endif

	/* Kernel also wants to know about all page directories. */
	{
		int pd;
		for(pd = 0; pd < MAX_PAGEDIR_PDES; pd++) {
			struct pdm *pdm = &pagedir_mappings[pd];
			
			assert(pdm->pdeno > 0);
#if defined(__riscv64__)
			assert(pdm->pdeno < kern_start_pde);
#else
			assert(pdm->pdeno > kern_pde);
#endif
			pt->pt_dir[pdm->pdeno] = pdm->val;
		}
	}

	/* Kernel also wants various mappings of its own. */
	for(i = 0; i < kernmappings; i++) {
		int r;
		if((r=pt_writemap(NULL, pt,
			kern_mappings[i].vir_addr,
			kern_mappings[i].phys_addr,
			kern_mappings[i].len,
			kern_mappings[i].flags, 0)) != OK) {
			return r;
		}

	}

	return OK;
}

int get_vm_self_pages(void) { return vm_self_pages; }

int pt_kern_mapping_lookup(vir_bytes addr, vir_bytes *vbase,
	phys_bytes *pbase, phys_bytes *len, u32_t *flags)
{
	int i;

	for (i = 0; i < kernmappings; i++) {
		vir_bytes base = kern_mappings[i].vir_addr;
		phys_bytes mlen = kern_mappings[i].len;

		if (!(kern_mappings[i].flags & ARCH_VM_PTE_USER))
			continue;
		if (addr < base || addr >= base + mlen)
			continue;

		if (vbase)
			*vbase = base;
		if (pbase)
			*pbase = kern_mappings[i].phys_addr;
		if (len)
			*len = mlen;
		if (flags)
			*flags = kern_mappings[i].flags;
		return 1;
	}

	return 0;
}
