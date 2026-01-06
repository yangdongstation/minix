/*
 * RISC-V 64 memory management
 *
 * Implements Sv39 paging with 4KB pages
 * Virtual address space: 512 GB (39 bits)
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"
#include "kernel/vm.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

#define PAGE_SIZE RISCV_PAGE_SIZE

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

static inline void *pt_phys_to_virt(phys_bytes pa)
{
    if (pa < VIRT_DRAM_BASE)
        return (void *)(vir_bytes)pa;
    return (void *)(vir_bytes)(KERNEL_BASE + (pa - VIRT_DRAM_BASE));
}

static u64_t *get_pgdir(struct proc *p)
{
    if (p && p->p_seg.p_satp_v)
        return (u64_t *)p->p_seg.p_satp_v;
    return _boot_pgdir;
}

/*
 * Initialize memory management
 */
void mem_init(phys_bytes usedlimit)
{
    phys_bytes mem_start, mem_size;

    bsp_get_memory(&mem_start, &mem_size);

    /* Set up memory bounds */
    mem_low = usedlimit;
    mem_high = mem_start + mem_size;

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
        free_page_list = *(phys_bytes *)pt_phys_to_virt(page);
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
    *(phys_bytes *)pt_phys_to_virt(page) = free_page_list;
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
            memset(pt_phys_to_virt(new_pt), 0, PAGE_SIZE);

            /* Install in parent */
            pt[idx] = PA_TO_PTE(new_pt);
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
            new_pt = page_alloc();
            if (new_pt == 0)
                return NULL;

            memset(pt_phys_to_virt(new_pt), 0, PAGE_SIZE);

            base = PTE_TO_PA(pte);
            flags = pte & (PTE_R | PTE_W | PTE_X | PTE_U |
                PTE_G | PTE_A | PTE_D);
            child_size = (phys_bytes)1ULL <<
                (RISCV_PAGE_SHIFT + (level - 1) * RISCV_PTE_SHIFT);

            for (i = 0; i < PT_ENTRIES; i++) {
                ((u64_t *)pt_phys_to_virt(new_pt))[i] =
                    PA_TO_PTE(base + (phys_bytes)i * child_size) |
                    flags;
            }

            pt[idx] = PA_TO_PTE(new_pt);
            pte = pt[idx];
        }

        /* Get next level page table */
        pt = (u64_t *)pt_phys_to_virt(PTE_TO_PA(pte));
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
    pgdir = get_pgdir(p);

    /* Convert MINIX VMMF_* flags to RISC-V PTE flags */
    pte_flags = PTE_V | PTE_A | PTE_D | PTE_R | PTE_X;
    if (flags & VMMF_WRITE)
        pte_flags |= PTE_W;
    if (flags & VMMF_USER)
        pte_flags |= PTE_U;
    if (flags & VMMF_GLO)
        pte_flags |= PTE_G;

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

    pgdir = get_pgdir(p);

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
	u64_t *pt;
	u64_t pte;
	int level;
#if defined(__riscv64__)
	static int umap_user_faults;
#endif

	(void)seg;
	(void)bytes;

	pgdir = get_pgdir(p);

    pt = pgdir;
    for (level = 2; level >= 0; level--) {
        int idx;
        phys_bytes page_size;

        switch (level) {
        case 2: idx = VPN2(vir); break;
        case 1: idx = VPN1(vir); break;
        default: idx = VPN0(vir); break;
        }

		pte = pt[idx];
		if (!(pte & PTE_V))
			return 0;

		if (pte & (PTE_R | PTE_W | PTE_X)) {
			if (p && !iskerneln(p->p_nr) && !(pte & PTE_U)) {
#if defined(__riscv64__)
				if (umap_user_faults < 8) {
					direct_print("rv64: umap_local no PTE_U ep=");
					direct_print_hex((u64_t)p->p_endpoint);
					direct_print(" va=");
					direct_print_hex((u64_t)vir);
					direct_print(" pte=");
					direct_print_hex((u64_t)pte);
					direct_print("\n");
					umap_user_faults++;
				}
#endif
				return 0;
			}
			/* Leaf PTE: compute physical address for large pages too. */
			page_size = (phys_bytes)1ULL <<
				(RISCV_PAGE_SHIFT + level * RISCV_PTE_SHIFT);
			return PTE_TO_PA(pte) | (vir & (page_size - 1));
        }

        if (level == 0)
            return 0;

        pt = (u64_t *)pt_phys_to_virt(PTE_TO_PA(pte));
    }

    return 0;
}

/*===========================================================================*
 *                              vm_lookup                                    *
 *===========================================================================*/
int vm_lookup(const struct proc *proc, const vir_bytes virtual,
	phys_bytes *physical, u32_t *ptent)
{
	phys_bytes phys;

	if (!proc || !physical)
		return EFAULT;

	phys = umap_local((struct proc *)proc, 0, virtual, 1);
	if (!phys)
		return EFAULT;

	*physical = phys;
	if (ptent)
		*ptent = 0;

	return OK;
}

/*===========================================================================*
 *				vm_lookup_range				     *
 *===========================================================================*/
size_t vm_lookup_range(const struct proc *proc, vir_bytes vir_addr,
	phys_bytes *phys_addr, size_t bytes)
{
	phys_bytes phys, next_phys;
	size_t len;

	if (!proc || bytes == 0)
		return 0;

	if (vm_lookup(proc, vir_addr, &phys, NULL) != OK)
		return 0;

	if (phys_addr != NULL)
		*phys_addr = phys;

	len = PAGE_SIZE - (vir_addr % PAGE_SIZE);
	vir_addr += len;
	next_phys = phys + len;

	while (len < bytes) {
		if (vm_lookup(proc, vir_addr, &phys, NULL) != OK)
			break;
		if (next_phys != phys)
			break;

		len += PAGE_SIZE;
		vir_addr += PAGE_SIZE;
		next_phys += PAGE_SIZE;
	}

	return MIN(bytes, len);
}

/*===========================================================================*
 *				vm_check_range				     *
 *===========================================================================*/
int vm_check_range(struct proc *caller, struct proc *target,
	vir_bytes vir_addr, size_t bytes, int writeflag)
{
	int r;

	if ((caller->p_misc_flags & MF_KCALL_RESUME) &&
		(r = caller->p_vmrequest.vmresult) != OK)
		return r;

	vm_suspend(caller, target, vir_addr, bytes, VMSTYPE_KERNELCALL,
		writeflag);

	return VMSUSPEND;
}

/*===========================================================================*
 *                           check_resumed_caller                            *
 *===========================================================================*/
static int check_resumed_caller(struct proc *caller)
{
    if (caller && (caller->p_misc_flags & MF_KCALL_RESUME)) {
        assert(caller->p_vmrequest.vmresult != VMSUSPEND);
        return caller->p_vmrequest.vmresult;
    }

    return OK;
}

/*===========================================================================*
 *                              vm_memset                                    *
 *===========================================================================*/
int vm_memset(struct proc *caller, endpoint_t who, phys_bytes ph, int c,
    phys_bytes count)
{
    struct proc *whoptr = NULL;
    phys_bytes cur_ph = ph;
    phys_bytes left = count;
    phys_bytes phys;
    phys_bytes chunk;
    int r;

    if ((r = check_resumed_caller(caller)) != OK)
        return r;

    /* NONE means physical address, otherwise virtual in target process. */
    if (who != NONE && !(whoptr = endpoint_lookup(who)))
        return ESRCH;

    c &= 0xFF;

    while (left > 0) {
        chunk = left;
        if (whoptr) {
            chunk = MIN(chunk, PAGE_SIZE - (cur_ph & (PAGE_SIZE - 1)));
            phys = umap_local(whoptr, 0, cur_ph, chunk);
            if (!phys) {
                if (caller) {
                    vm_suspend(caller, whoptr, cur_ph, count,
                        VMSTYPE_KERNELCALL, 1);
                    return VMSUSPEND;
                }
                return EFAULT;
            }
        } else {
            phys = cur_ph;
        }

        phys_memset(phys, (unsigned long)c, chunk);
        cur_ph += chunk;
        left -= chunk;
    }

    return OK;
}

/*===========================================================================*
 *				virtual_copy_f				     *
 *===========================================================================*/
int virtual_copy_f(struct proc *caller, struct vir_addr *src_addr,
    struct vir_addr *dst_addr, vir_bytes bytes, int vmcheck)
{
    struct proc *procs[2];
    struct vir_addr *vir_addr[2];
    int i, r;
#ifdef __riscv64
    static int vcopy_trace_once;
    static int vcopy_fault_count;
    int trace = 0;
#endif

    if (bytes <= 0)
        return EDOM;

    vir_addr[_SRC_] = src_addr;
    vir_addr[_DST_] = dst_addr;

    for (i = _SRC_; i <= _DST_; i++) {
        endpoint_t proc_e = vir_addr[i]->proc_nr_e;
        int proc_nr;
        struct proc *p;

        if (proc_e == NONE) {
            p = NULL;
        } else {
            if (!isokendpt(proc_e, &proc_nr))
                return ESRCH;
            p = proc_addr(proc_nr);
        }
        procs[i] = p;
    }

    if ((r = check_resumed_caller(caller)) != OK)
        return r;

#ifdef __riscv64
    if (caller && caller->p_endpoint == VM_PROC_NR &&
        src_addr->proc_nr_e == KERNEL &&
        dst_addr->proc_nr_e == caller->p_endpoint &&
        bytes == 0x400 && vcopy_trace_once == 0) {
        trace = 1;
        direct_print("rv64: vcopy start dst=");
        direct_print_hex((u64_t)dst_addr->offset);
        direct_print(" src=");
        direct_print_hex((u64_t)src_addr->offset);
        direct_print(" bytes=");
        direct_print_hex((u64_t)bytes);
        direct_print(" vmcheck=");
        direct_print_hex((u64_t)vmcheck);
        direct_print("\n");
    }
#endif

    vir_bytes src_off = src_addr->offset;
    vir_bytes dst_off = dst_addr->offset;
    vir_bytes left = bytes;

    while (left > 0) {
        phys_bytes src_phys, dst_phys;
        vir_bytes chunk = left;

        if (procs[_SRC_])
            chunk = MIN(chunk, PAGE_SIZE - (src_off & (PAGE_SIZE - 1)));
        if (procs[_DST_])
            chunk = MIN(chunk, PAGE_SIZE - (dst_off & (PAGE_SIZE - 1)));

        src_phys = procs[_SRC_] ?
            umap_local(procs[_SRC_], 0, src_off, chunk) : src_off;
#ifdef __riscv64
        if (trace) {
            direct_print("rv64: vcopy src_phys=");
            direct_print_hex((u64_t)src_phys);
            direct_print(" src_off=");
            direct_print_hex((u64_t)src_off);
            direct_print(" chunk=");
            direct_print_hex((u64_t)chunk);
            direct_print("\n");
        }
#endif
        if (!src_phys) {
            if (vmcheck && caller) {
                vm_suspend(caller, procs[_SRC_], src_off, bytes,
                    VMSTYPE_KERNELCALL, 0);
                return VMSUSPEND;
            }
            return EFAULT_SRC;
        }

        dst_phys = procs[_DST_] ?
            umap_local(procs[_DST_], 0, dst_off, chunk) : dst_off;
#ifdef __riscv64
        if (trace) {
            direct_print("rv64: vcopy dst_phys=");
            direct_print_hex((u64_t)dst_phys);
            direct_print(" dst_off=");
            direct_print_hex((u64_t)dst_off);
            direct_print("\n");
        }
#endif
        if (!dst_phys) {
            if (vmcheck && caller) {
                vm_suspend(caller, procs[_DST_], dst_off, bytes,
                    VMSTYPE_KERNELCALL, 1);
                return VMSUSPEND;
            }
            return EFAULT_DST;
        }

        if (phys_copy(src_phys, dst_phys, chunk) != 0) {
#ifdef __riscv64
            if (trace) {
                direct_print("rv64: vcopy phys_copy fault\n");
            }
            if (vcopy_fault_count < 8) {
                direct_print("rv64: vcopy phys_copy fault caller=");
                if (caller) {
                    direct_print_hex((u64_t)caller->p_endpoint);
                } else {
                    direct_print("0");
                }
                direct_print(" src_e=");
                direct_print_hex((u64_t)vir_addr[_SRC_]->proc_nr_e);
                direct_print(" dst_e=");
                direct_print_hex((u64_t)vir_addr[_DST_]->proc_nr_e);
                direct_print(" src_phys=");
                direct_print_hex((u64_t)src_phys);
                direct_print(" dst_phys=");
                direct_print_hex((u64_t)dst_phys);
                direct_print(" src_off=");
                direct_print_hex((u64_t)src_off);
                direct_print(" dst_off=");
                direct_print_hex((u64_t)dst_off);
                direct_print(" bytes=");
                direct_print_hex((u64_t)bytes);
                direct_print(" chunk=");
                direct_print_hex((u64_t)chunk);
                direct_print("\n");
                vcopy_fault_count++;
            }
#endif
            if (vmcheck && caller) {
                vm_suspend(caller, procs[_DST_], dst_off, bytes,
                    VMSTYPE_KERNELCALL, 1);
                return VMSUSPEND;
            }
            return EFAULT;
        }

        left -= chunk;
        src_off += chunk;
        dst_off += chunk;
    }

#ifdef __riscv64
    if (trace) {
        direct_print("rv64: vcopy done\n");
        vcopy_trace_once = 1;
    }
#endif

    return OK;
}

/*===========================================================================*
 *				data_copy				     *
 *===========================================================================*/
int data_copy(const endpoint_t from_proc, const vir_bytes from_addr,
    const endpoint_t to_proc, const vir_bytes to_addr, size_t bytes)
{
    struct vir_addr src, dst;

    src.offset = from_addr;
    dst.offset = to_addr;
    src.proc_nr_e = from_proc;
    dst.proc_nr_e = to_proc;

    return virtual_copy(&src, &dst, bytes);
}

/*===========================================================================*
 *				data_copy_vmcheck			     *
 *===========================================================================*/
int data_copy_vmcheck(struct proc *caller, const endpoint_t from_proc,
    const vir_bytes from_addr, const endpoint_t to_proc,
    const vir_bytes to_addr, size_t bytes)
{
    struct vir_addr src, dst;

    src.offset = from_addr;
    dst.offset = to_addr;
    src.proc_nr_e = from_proc;
    dst.proc_nr_e = to_proc;

    return virtual_copy_vmcheck(caller, &src, &dst, bytes);
}

void memory_init(void)
{
}

/*===========================================================================*
 *				arch_proc_init				     *
 *===========================================================================*/
void arch_proc_init(struct proc *pr, const u32_t ip, const u32_t sp,
	const u32_t ps_str, char *name)
{
	reg_t saved_satp = pr->p_seg.p_satp;
	reg_t *saved_satp_v = pr->p_seg.p_satp_v;

	arch_proc_reset(pr);
	/* Preserve address space root set by VM before exec. */
	pr->p_seg.p_satp = saved_satp;
	pr->p_seg.p_satp_v = saved_satp_v;
	strlcpy(pr->p_name, name, sizeof(pr->p_name));

	pr->p_reg.pc = ip;
	pr->p_reg.sp = sp;
	/* __start(cleanup, obj, ps_strings): a0/a1 = NULL, a2 = ps_strings. */
	pr->p_reg.a0 = 0;
	pr->p_reg.a1 = 0;
	pr->p_reg.a2 = ps_str;
}

static int usermapped_glo_index = -1,
	usermapped_index = -1, first_um_idx = -1;

extern char usermapped_start, usermapped_end, usermapped_nonglo_start;

int arch_phys_map(const int index, phys_bytes *addr, phys_bytes *len,
	int *flags)
{
	static int first = 1;
	phys_bytes glo_len;
	phys_bytes phys;
	int freeidx = 0;

	glo_len = (phys_bytes)((vir_bytes)&usermapped_nonglo_start -
		(vir_bytes)&usermapped_start);

	if (first) {
		memset(&minix_kerninfo, 0, sizeof(minix_kerninfo));
		if (glo_len > 0)
			usermapped_glo_index = freeidx++;

		usermapped_index = freeidx++;
		first_um_idx = usermapped_index;
		if (usermapped_glo_index != -1)
			first_um_idx = usermapped_glo_index;
		first = 0;
	}

	if (index == usermapped_glo_index) {
		phys = umap_local(NULL, 0, (vir_bytes)&usermapped_start, 1);
		if (!phys)
			return EFAULT;
		*addr = phys;
		*len = glo_len;
		*flags = VMMF_USER | VMMF_GLO;
		return OK;
	} else if (index == usermapped_index) {
		phys = umap_local(NULL, 0, (vir_bytes)&usermapped_nonglo_start, 1);
		if (!phys)
			return EFAULT;
		*addr = phys;
		*len = (phys_bytes)((vir_bytes)&usermapped_end -
			(vir_bytes)&usermapped_nonglo_start);
		*flags = VMMF_USER;
		return OK;
	}

	return EINVAL;
}

int arch_phys_map_reply(const int index, const vir_bytes addr)
{
	if (index == first_um_idx) {
		intptr_t usermapped_offset;

		assert(addr != 0);
		direct_print("rv64: phys_map_reply idx=");
		direct_print_hex((u64_t)index);
		direct_print(" addr=");
		direct_print_hex((u64_t)addr);
		direct_print(" usermapped_start=");
		direct_print_hex((u64_t)(vir_bytes)&usermapped_start);
		direct_print("\n");
		usermapped_offset =
			(intptr_t)addr - (intptr_t)(vir_bytes)&usermapped_start;
#define FIXEDPTR(ptr) (void *)((intptr_t)(ptr) + usermapped_offset)
#define ASSIGN(minixstruct) minix_kerninfo.minixstruct = FIXEDPTR(&minixstruct)
		ASSIGN(kinfo);
		ASSIGN(machine);
		ASSIGN(kmessages);
		ASSIGN(loadinfo);
		ASSIGN(kuserinfo);
		ASSIGN(arm_frclock);
		ASSIGN(kclockinfo);

		minix_kerninfo.kerninfo_magic = KERNINFO_MAGIC;
		minix_kerninfo.minix_feature_flags = minix_feature_flags;
		minix_kerninfo_user = (vir_bytes)FIXEDPTR(&minix_kerninfo);
		direct_print("rv64: phys_map_reply offset=");
		direct_print_hex((u64_t)(uintptr_t)usermapped_offset);
		direct_print(" kerninfo_user=");
		direct_print_hex((u64_t)minix_kerninfo_user);
		direct_print("\n");

		minix_kerninfo.ki_flags |= MINIX_KIF_USERINFO;

		return OK;
	}

	if (index == usermapped_index) {
		direct_print("rv64: phys_map_reply idx=");
		direct_print_hex((u64_t)index);
		direct_print(" addr=");
		direct_print_hex((u64_t)addr);
		direct_print(" (nonglo)\n");
		return OK;
	}

	return EINVAL;
}

void mem_clear_mapcache(void)
{
}

void release_address_space(struct proc *pr)
{
	if (!pr)
		return;

	pr->p_seg.p_satp = 0;
	pr->p_seg.p_satp_v = NULL;
}

int copy_msg_from_user(message *user_mbuf, message *dst)
{
	struct proc *caller;
	struct vir_addr src, dst_addr;

	caller = get_cpulocal_var(proc_ptr);
	if (caller == NULL)
		return 1;

	src.proc_nr_e = caller->p_endpoint;
	src.offset = (vir_bytes)user_mbuf;
	dst_addr.proc_nr_e = KERNEL;
	dst_addr.offset = (vir_bytes)dst;

	return (virtual_copy(&src, &dst_addr, sizeof(message)) != OK);
}

int copy_msg_to_user(message *src, message *user_mbuf)
{
	struct proc *caller;
	struct vir_addr src_addr, dst;

	caller = get_cpulocal_var(proc_ptr);
	if (caller == NULL)
		return 1;

	src_addr.proc_nr_e = KERNEL;
	src_addr.offset = (vir_bytes)src;
	dst.proc_nr_e = caller->p_endpoint;
	dst.offset = (vir_bytes)user_mbuf;

	return (virtual_copy(&src_addr, &dst, sizeof(message)) != OK);
}
