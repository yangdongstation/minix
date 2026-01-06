/*
 * RISC-V 64 architecture function prototypes for MINIX kernel
 */

#ifndef _RISCV64_ARCH_PROTO_H
#define _RISCV64_ARCH_PROTO_H

#include <machine/types.h>
#include "archconst.h"

#define RISCV64_STRINGIFY_1(x) #x
#define RISCV64_STRINGIFY(x) RISCV64_STRINGIFY_1(x)

struct exec;
struct proc;
struct trapframe;

/* head.S - Entry points */
void _C_LABEL(_start)(void);
void _C_LABEL(kernel_entry)(void);
void _C_LABEL(trap_entry)(void);
void _C_LABEL(secondary_hart_entry)(void);

/* exception.c */
void exception_init(void);
void exception_handler(struct trapframe *tf);

/* hw_intr.c */
void hw_intr_init(void);
void hw_intr_mask(int irq);
void hw_intr_unmask(int irq);
void hw_intr_ack(int irq);
void hw_intr_handler(struct trapframe *tf);

/* plic.c */
void plic_init(void);
void plic_enable_irq(int irq, int cpu);
void plic_disable_irq(int irq);
int  plic_claim(int cpu);
void plic_complete(int cpu, int irq);
void plic_set_priority(int irq, int priority);
void plic_set_threshold(int cpu, int threshold);
void plic_irq_cpu_mask(int irq, u32_t cpu_mask);
u32_t plic_irq_get_cpu_mask(int irq);

/* sbi.c - SBI interface */
void sbi_console_putchar(int ch);
int  sbi_console_getchar(void);
void sbi_set_timer(u64_t stime_value);
void sbi_send_ipi(unsigned long hart_mask);
void sbi_remote_fence_i(unsigned long hart_mask);
void sbi_remote_sfence_vma(unsigned long hart_mask, unsigned long start, unsigned long size);
void sbi_shutdown(void);
void sbi_system_reset(u32_t reset_type, u32_t reset_reason);

/* arch_clock.c */
void arch_init_clock(void);
void arch_stop_clock(void);
int  arch_clock_handler(void);
u64_t arch_get_timestamp(void);
void arch_set_timer_freq(u64_t freq);

/* arch_system.c */
void arch_system_init(void);
void arch_shutdown(int how);
void arch_monitor(void);
void arch_get_aout_headers(int i, struct exec *h);

/* memory.c */
void mem_init(phys_bytes usedlimit);
void vm_init(struct proc *newptproc);
int vm_map_range(struct proc *p, phys_bytes phys, vir_bytes vir, size_t bytes, u32_t flags);
void vm_unmap_range(struct proc *p, vir_bytes vir, size_t bytes);
phys_bytes umap_local(struct proc *p, int seg, vir_bytes vir, vir_bytes bytes);

/* protect.c */
void prot_init(void);
void prot_set_kern_seg_limit(vir_bytes limit);

/* pg_utils.c */
void pg_map(phys_bytes phys, vir_bytes virt, size_t size, u64_t flags);
void pg_protect(vir_bytes virt, size_t size, u64_t flags);
void pg_unmap(vir_bytes virt, size_t size);
void pg_identity_map(phys_bytes start, phys_bytes end);
void pg_flush_tlb(void);
void pg_extend_kernel_map(phys_bytes start, phys_bytes size);
void pg_load(struct proc *p);
void pg_dump_mapping(vir_bytes va);
phys_bytes pg_create(void);
void add_memmap(kinfo_t *cbi, u64_t addr, u64_t len);

/* arch_do_vmctl.c */
void set_pgdir(phys_bytes pgdir);

/* arch_system.c */
void riscv64_switch_address_space(struct proc *p);

/* phys_copy.S */
phys_bytes phys_copy(phys_bytes src, phys_bytes dst, phys_bytes size);
void phys_memset(phys_bytes dst, unsigned long c, phys_bytes size);
void phys_copy_fault(void);
void phys_copy_fault_in_kernel(void);

/* direct_tty_utils.c */
void direct_init(void);
void direct_cls(void);
void direct_print(const char *str);
void direct_print_char(int c);
int  direct_read_char(void);
void direct_print_hex(u64_t val);
void direct_print_dec(u64_t val);
void direct_printf(const char *fmt, ...);

/* console.c - Early console support */
void riscv_cons_init(void);
void riscv_cons_putc(int c);
int  riscv_cons_getc(void);

/* bsp/virt */
void bsp_early_init(void);
void bsp_serial_init(void);
void bsp_get_memory(phys_bytes *start, phys_bytes *size);

/* arch_do_vmctl.c */
int arch_do_vmctl(message *m_ptr, struct proc *p);

/* arch_reset.c */
void arch_reset(void);
__dead void reset(void);

/* klib.S */
void switch_to_user(void);
void restore_user_context(struct proc *p);
void save_fpu(struct proc *p);
int restore_fpu(struct proc *p);

/* mpx.S */
void level0(void (*func)(void));
void read_tsc_64(u64_t *t);
u32_t read_cpu_flags(void);
void write_cpu_flags(u32_t flags);

/* smp.c (if SMP enabled) */
#ifdef CONFIG_SMP
void smp_init(void);
void smp_start_cpu(int cpu);
void smp_ap_entry(int hart_id);
void smp_send_ipi(int cpu);
void smp_broadcast_ipi(void);
void smp_ipi_handler(struct trapframe *tf);
int cpu_number(void);
#endif

/* CSR operations (inline assembly) */
static inline u64_t csr_read_sstatus(void) {
    u64_t val;
    __asm__ __volatile__("csrr %0, " RISCV64_STRINGIFY(CSR_SSTATUS)
        : "=r"(val));
    return val;
}

static inline void csr_write_sstatus(u64_t val) {
    __asm__ __volatile__("csrw " RISCV64_STRINGIFY(CSR_SSTATUS) ", %0"
        :: "r"(val));
}

static inline void csr_set_sstatus(u64_t bits) {
    __asm__ __volatile__("csrs " RISCV64_STRINGIFY(CSR_SSTATUS) ", %0"
        :: "r"(bits));
}

static inline void csr_clear_sstatus(u64_t bits) {
    __asm__ __volatile__("csrc " RISCV64_STRINGIFY(CSR_SSTATUS) ", %0"
        :: "r"(bits));
}

static inline u64_t csr_read_sie(void) {
    u64_t val;
    __asm__ __volatile__("csrr %0, " RISCV64_STRINGIFY(CSR_SIE)
        : "=r"(val));
    return val;
}

static inline void csr_write_sie(u64_t val) {
    __asm__ __volatile__("csrw " RISCV64_STRINGIFY(CSR_SIE) ", %0"
        :: "r"(val));
}

static inline void csr_set_sie(u64_t bits) {
    __asm__ __volatile__("csrs " RISCV64_STRINGIFY(CSR_SIE) ", %0"
        :: "r"(bits));
}

static inline void csr_clear_sie(u64_t bits) {
    __asm__ __volatile__("csrc " RISCV64_STRINGIFY(CSR_SIE) ", %0"
        :: "r"(bits));
}

static inline u64_t csr_read_sip(void) {
    u64_t val;
    __asm__ __volatile__("csrr %0, " RISCV64_STRINGIFY(CSR_SIP)
        : "=r"(val));
    return val;
}

static inline void csr_clear_sip(u64_t bits) {
    __asm__ __volatile__("csrc " RISCV64_STRINGIFY(CSR_SIP) ", %0"
        :: "r"(bits));
}

static inline u64_t csr_read_scause(void) {
    u64_t val;
    __asm__ __volatile__("csrr %0, " RISCV64_STRINGIFY(CSR_SCAUSE)
        : "=r"(val));
    return val;
}

static inline u64_t csr_read_stval(void) {
    u64_t val;
    __asm__ __volatile__("csrr %0, " RISCV64_STRINGIFY(CSR_STVAL)
        : "=r"(val));
    return val;
}

static inline u64_t csr_read_sepc(void) {
    u64_t val;
    __asm__ __volatile__("csrr %0, " RISCV64_STRINGIFY(CSR_SEPC)
        : "=r"(val));
    return val;
}

static inline void csr_write_sepc(u64_t val) {
    __asm__ __volatile__("csrw " RISCV64_STRINGIFY(CSR_SEPC) ", %0"
        :: "r"(val));
}

static inline void csr_write_stvec(u64_t val) {
    __asm__ __volatile__("csrw " RISCV64_STRINGIFY(CSR_STVEC) ", %0"
        :: "r"(val));
}

static inline void csr_write_sscratch(u64_t val) {
    __asm__ __volatile__("csrw " RISCV64_STRINGIFY(CSR_SSCRATCH) ", %0"
        :: "r"(val));
}

static inline u64_t csr_read_satp(void) {
    u64_t val;
    __asm__ __volatile__("csrr %0, " RISCV64_STRINGIFY(CSR_SATP)
        : "=r"(val));
    return val;
}

static inline void sfence_vma_all(void) {
    __asm__ __volatile__(".word " RISCV64_STRINGIFY(RISCV_SFENCE_VMA_INSN)
        ::: "memory");
}

static inline void csr_write_satp(u64_t val) {
    __asm__ __volatile__("csrw " RISCV64_STRINGIFY(CSR_SATP) ", %0"
        :: "r"(val));
    sfence_vma_all();
}

static inline u64_t csr_read_time(void) {
    u64_t val;
    __asm__ __volatile__("rdtime %0" : "=r"(val));
    return val;
}

static inline u64_t csr_read_cycle(void) {
    u64_t val;
    __asm__ __volatile__("rdcycle %0" : "=r"(val));
    return val;
}

static inline int csr_read_hartid(void) {
    /* Hart ID is stored in tp register during boot */
    int hartid;
    __asm__ __volatile__("mv %0, tp" : "=r"(hartid));
    return hartid;
}

/* Memory barriers */
static inline void mb(void) {
    __asm__ __volatile__("fence iorw, iorw" ::: "memory");
}

static inline void rmb(void) {
    __asm__ __volatile__("fence ir, ir" ::: "memory");
}

static inline void wmb(void) {
    __asm__ __volatile__("fence ow, ow" ::: "memory");
}

static inline void fence_i(void) {
    __asm__ __volatile__("fence.i" ::: "memory");
}

/* Interrupt control */
static inline void intr_enable(void) {
    csr_set_sstatus(SSTATUS_SIE);
}

static inline void intr_disable(void) {
    csr_clear_sstatus(SSTATUS_SIE);
}

static inline int intr_disabled(void) {
    return (csr_read_sstatus() & SSTATUS_SIE) == 0;
}

/* WFI (Wait For Interrupt) */
static inline void wfi(void) {
    __asm__ __volatile__(".word " RISCV64_STRINGIFY(RISCV_WFI_INSN));
}

#endif /* _RISCV64_ARCH_PROTO_H */
