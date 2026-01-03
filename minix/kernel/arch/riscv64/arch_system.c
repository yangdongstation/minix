/*
 * RISC-V 64 system management functions
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"

#include <assert.h>
#include <string.h>

extern u64_t _boot_pgdir[];

/* Shutdown types */
#define RBT_HALT        0
#define RBT_REBOOT      1
#define RBT_PANIC       2
#define RBT_POWEROFF    3

/*
 * Initialize architecture-specific system components
 */
void arch_system_init(void)
{
    /* Initialize exception handling */
    exception_init();

    /* Initialize hardware interrupts (PLIC) */
    hw_intr_init();

    /* Initialize clock */
    arch_init_clock();

    /* TODO: Initialize other subsystems */
}

/*
 * System shutdown/reboot
 */
void arch_shutdown(int how)
{
    /* Disable interrupts */
    intr_disable();

    switch (how) {
    case RBT_HALT:
    case RBT_PANIC:
        direct_print("System halted.\n");
        sbi_system_reset(0, 0);  /* Shutdown */
        break;

    case RBT_REBOOT:
        direct_print("Rebooting...\n");
        sbi_system_reset(1, 0);  /* Cold reboot */
        break;

    case RBT_POWEROFF:
        direct_print("Power off.\n");
        sbi_system_reset(0, 0);  /* Shutdown */
        break;

    default:
        direct_print("Unknown shutdown type, halting.\n");
        sbi_system_reset(0, 0);
    }

    /* Should not reach here */
    for (;;) {
        wfi();
    }
}

/*
 * Return to monitor (not applicable on RISC-V)
 */
void arch_monitor(void)
{
    /* No monitor on RISC-V, just halt */
    arch_shutdown(RBT_HALT);
}

/*
 * Get a.out headers (for binary loading)
 */
void arch_get_aout_headers(int i, struct exec *h)
{
    /* TODO: Implement if needed for a.out binary support */
    (void)i;
    (void)h;
}

int intr_init(int auto_eoi)
{
	(void)auto_eoi;
	return 0;
}

void do_ser_debug(void)
{
}

void cpu_identify(void)
{
	unsigned cpu = cpuid;
	u64_t freq = CLOCK_FREQ;

#ifdef CONFIG_SMP
	cpu_info[cpu].hartid = cpu_number();
#else
	cpu_info[cpu].hartid = 0;
#endif
	cpu_info[cpu].freq = (u32_t)(freq / 1000000);
	cpu_set_freq(cpu, freq);
}

void fpu_init(void)
{
	get_cpulocal_var(fpu_presence) = 1;
	get_cpulocal_var(fpu_owner) = NULL;
}

void save_local_fpu(struct proc *pr, int retain)
{
	(void)retain;
	if (pr == NULL)
		return;
	save_fpu(pr);
}

void enable_fpu_exception(void)
{
}

void disable_fpu_exception(void)
{
}

void fpu_sigcontext(struct proc *pr, struct sigframe_sigcontext *fr,
	struct sigcontext *sc)
{
	(void)pr;
	(void)fr;
	(void)sc;
}

reg_t arch_get_sp(struct proc *p)
{
	return p->p_reg.sp;
}

void arch_set_secondary_ipc_return(struct proc *p, u32_t val)
{
	p->p_reg.a1 = val;
}

void arch_proc_reset(struct proc *pr)
{
	assert(pr->p_nr < NR_PROCS);

	memset(&pr->p_reg, 0, sizeof(pr->p_reg));
	pr->p_reg.psr = SSTATUS_SPIE;
	if (iskerneln(pr->p_nr))
		pr->p_reg.psr |= SSTATUS_SPP;

	pr->p_seg.p_satp = 0;
	pr->p_seg.p_satp_v = NULL;
	pr->p_seg.fpu_state = NULL;
}

void riscv64_switch_address_space(struct proc *p)
{
	phys_bytes pgdir;

	if (p && p->p_seg.p_satp != 0) {
		pgdir = p->p_seg.p_satp;
		set_pgdir(pgdir);
		pg_flush_tlb();
		if (p->p_endpoint == VM_PROC_NR)
			csr_set_sstatus(SSTATUS_SUM);
		else
			csr_clear_sstatus(SSTATUS_SUM);
		get_cpulocal_var(ptproc) = p;
	} else {
		set_pgdir((phys_bytes)_boot_pgdir);
		pg_flush_tlb();
		csr_clear_sstatus(SSTATUS_SUM);
	}
}

void arch_proc_setcontext(struct proc *p, struct stackframe_s *state, int user,
	int trapstyle)
{
	(void)user;
	(void)trapstyle;

	assert(sizeof(p->p_reg) == sizeof(*state));
	if (state != &p->p_reg)
		memcpy(&p->p_reg, state, sizeof(*state));

	p->p_misc_flags |= MF_CONTEXT_SET;
}

void arch_do_syscall(struct proc *proc)
{
	assert(proc == get_cpulocal_var(proc_ptr));
	assert(proc->p_misc_flags & MF_SC_DEFER);
	proc->p_reg.retreg =
		do_ipc(proc->p_defer.r1, proc->p_defer.r2, proc->p_defer.r3);
}

struct proc *arch_finish_switch_to_user(void)
{
	struct proc *p;

	p = get_cpulocal_var(proc_ptr);
	p->p_reg.psr |= SSTATUS_SPIE;
	p->p_reg.psr &= ~SSTATUS_SIE;
	if (iskerneln(p->p_nr))
		p->p_reg.psr |= SSTATUS_SPP;
	else
		p->p_reg.psr &= ~SSTATUS_SPP;

	return p;
}

void arch_init(void)
{
	arch_system_init();
	cpu_identify();
	fpu_init();
}

void arch_pause(void)
{
	__asm__ __volatile__("nop");
}
