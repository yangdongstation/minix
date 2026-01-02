/*
 * RISC-V 64 protection/privilege management
 *
 * RISC-V uses privilege levels:
 * - M-mode (Machine): Highest privilege, runs OpenSBI
 * - S-mode (Supervisor): Kernel runs here
 * - U-mode (User): User processes
 */

#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/exec_elf.h>

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"

#include <sys/exec.h>

/*
 * Initialize protection
 */
void prot_init(void)
{
    /* On RISC-V, we're already in S-mode after OpenSBI handoff */

    /* Configure sstatus for default state */
    u64_t sstatus = csr_read_sstatus();

    /* Clear SPP (will return to U-mode after sret) */
    sstatus &= ~SSTATUS_SPP;

    /* Enable FPU if present */
    sstatus |= SSTATUS_FS_INITIAL;

    /* Disable SUM (supervisor can't access user memory by default) */
    sstatus &= ~SSTATUS_SUM;

    csr_write_sstatus(sstatus);

    /* Set up PMP if needed (usually done by OpenSBI) */
}

/*
 * Set kernel segment limit (not applicable on RISC-V)
 */
void prot_set_kern_seg_limit(vir_bytes limit)
{
    /* RISC-V uses page tables, not segments */
    (void)limit;
}

/*
 * Check if address is kernel address
 */
int is_kernel_addr(vir_bytes addr)
{
    /* Kernel addresses are in high memory (negative in 2's complement) */
    return (addr >= KERNEL_BASE);
}

/*
 * Check if current mode is kernel
 */
int in_kernel_mode(void)
{
    /* Check SPP bit in sstatus */
    return (csr_read_sstatus() & SSTATUS_SPP) != 0;
}

/*
 * Enable user memory access from supervisor
 */
void enable_user_access(void)
{
    csr_set_sstatus(SSTATUS_SUM);
}

/*
 * Disable user memory access from supervisor
 */
void disable_user_access(void)
{
    csr_clear_sstatus(SSTATUS_SUM);
}

void arch_post_init(void)
{
	struct proc *vm;

	vm = proc_addr(VM_PROC_NR);
	get_cpulocal_var(ptproc) = vm;
}

static int alloc_for_vm = 0;

static int load_vm_elf(multiboot_module_t *mod, vir_bytes stack_high,
	vir_bytes stack_size, vir_bytes *entry)
{
	const char *image;
	size_t image_size;
	Elf64_Ehdr *ehdr;
	Elf64_Phdr *phdr;
	size_t ph_size;
	int i;

	image = (const char *)(vir_bytes)mod->mod_start;
	image_size = mod->mod_end - mod->mod_start;

	if (image_size < sizeof(*ehdr))
		return ENOEXEC;

	ehdr = (Elf64_Ehdr *)image;
	if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
		ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
		ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
		ehdr->e_ident[EI_MAG3] != ELFMAG3)
		return ENOEXEC;

	if (ehdr->e_ident[EI_CLASS] != ELFCLASS64 ||
		ehdr->e_ident[EI_DATA] != ELFDATA2LSB ||
		ehdr->e_machine != EM_RISCV ||
		ehdr->e_phentsize != sizeof(Elf64_Phdr))
		return ENOEXEC;

	ph_size = (size_t)ehdr->e_phnum * sizeof(Elf64_Phdr);
	if (ehdr->e_phoff + ph_size > image_size)
		return ENOEXEC;

	phdr = (Elf64_Phdr *)(image + ehdr->e_phoff);

	for (i = 0; i < ehdr->e_phnum; i++) {
		Elf64_Phdr *ph = &phdr[i];
		u64_t flags = RISCV_PTE_U;

		if (ph->p_type != PT_LOAD || ph->p_memsz == 0)
			continue;
		if (ph->p_offset + ph->p_filesz > image_size)
			return ENOEXEC;
		if ((vir_bytes)ph->p_vaddr >= KERNEL_BASE)
			return ENOEXEC;

		if (ph->p_flags & PF_R)
			flags |= RISCV_PTE_R;
		if (ph->p_flags & PF_W)
			flags |= RISCV_PTE_W;
		if (ph->p_flags & PF_X)
			flags |= RISCV_PTE_X;

		pg_map(PG_ALLOCATEME, (vir_bytes)ph->p_vaddr,
			(size_t)ph->p_memsz, flags);
		alloc_for_vm += roundup((vir_bytes)ph->p_memsz, RISCV_PAGE_SIZE);
	}

	stack_size = roundup(stack_size, RISCV_PAGE_SIZE);
	stack_high = rounddown(stack_high, RISCV_PAGE_SIZE);
	if (stack_size > 0) {
		pg_map(PG_ALLOCATEME, stack_high - stack_size, stack_size,
			RISCV_PTE_R | RISCV_PTE_W | RISCV_PTE_U);
		alloc_for_vm += stack_size;
	}

	pg_load(NULL);

	enable_user_access();
	for (i = 0; i < ehdr->e_phnum; i++) {
		Elf64_Phdr *ph = &phdr[i];
		vir_bytes vaddr;

		if (ph->p_type != PT_LOAD || ph->p_memsz == 0)
			continue;

		vaddr = (vir_bytes)ph->p_vaddr;
		if (ph->p_filesz > 0) {
			memcpy((void *)vaddr,
				image + ph->p_offset,
				(size_t)ph->p_filesz);
		}
		if (ph->p_memsz > ph->p_filesz) {
			memset((void *)(vaddr + ph->p_filesz), 0,
				(size_t)(ph->p_memsz - ph->p_filesz));
		}
	}
	disable_user_access();

	*entry = (vir_bytes)ehdr->e_entry;
	return OK;
}

static multiboot_module_t *bootmod(int pnr)
{
	int i;

	assert(pnr >= 0);

	for (i = NR_TASKS; i < NR_BOOT_PROCS; i++) {
		int p = i - NR_TASKS;

		if (image[i].proc_nr == pnr) {
			assert(p < MULTIBOOT_MAX_MODS);
			assert(p < kinfo.mbi.mi_mods_count);
			return &kinfo.module_list[p];
		}
	}

	panic("boot module %d not found", pnr);
}

void arch_boot_proc(struct boot_image *ip, struct proc *rp)
{
	multiboot_module_t *mod;
	struct ps_strings *psp;
	char *sp;

	if (rp->p_nr < 0)
		return;

	mod = bootmod(rp->p_nr);

	if (rp->p_nr == VM_PROC_NR) {
		vir_bytes pc;
		vir_bytes stack_high = kinfo.user_sp;
		vir_bytes stack_size = 64 * 1024;

		if (load_vm_elf(mod, stack_high, stack_size, &pc) != OK)
			panic("VM loading failed");

		stack_high = rounddown(stack_high, RISCV_PAGE_SIZE);
		sp = (char *)(vir_bytes)stack_high;
		sp -= sizeof(struct ps_strings);
		psp = (struct ps_strings *)sp;
		sp -= (sizeof(void *) + sizeof(void *) + sizeof(int));

		psp->ps_argvstr = (char **)(sp + sizeof(int));
		psp->ps_nargvstr = 0;
		psp->ps_envstr = psp->ps_argvstr + sizeof(void *);
		psp->ps_nenvstr = 0;

		arch_proc_init(rp, pc, (vir_bytes)sp,
			stack_high - sizeof(struct ps_strings),
			ip->proc_name);

		add_memmap(&kinfo, mod->mod_start, mod->mod_end - mod->mod_start);
		mod->mod_end = mod->mod_start = 0;
		kinfo.vm_allocated_bytes = alloc_for_vm;
	}
}
