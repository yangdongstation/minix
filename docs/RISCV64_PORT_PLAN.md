# MINIX 3 RISC-V 64 Port Plan / MINIX 3 RISC-V 64 移植项目计划（可执行版）

**Document Info / 文档信息**
- Last updated / 最后更新: 2026-01-07
- Version / 版本: 1.1
- Scope / 范围: evbriscv64 on QEMU virt
- Purpose / 用途: actionable checklist; do not re-apply steps already present in tree

**Status Legend / 状态标识**
- Done / 已完成：已在代码树中观察到
- Partial / 部分完成：已实现但运行不稳定或仍有缺口
- Planned / 计划中：尚未实现
- Blocked / 受阻：已知阻塞原因

## Status Summary / 状态概览

**中文**
- Phase 0/1：构建系统与目录结构已在代码树中观察到（见 `build.sh`、`sys/arch/*`、`share/mk/bsd.own.mk`）。
- Phase 2：`minix/kernel/arch/riscv64` 已存在，内核基础具备但运行不稳定（见 `RISC64-STATUS.md`）。
- 运行时关键问题与修复建议见 `issue.md`。
- 2026-01-07 文档同步：计划内容未变更，状态沿用 2026-01-06。

**English**
- Phase 0/1: build-system and arch directories are present in tree (`build.sh`, `sys/arch/*`, `share/mk/bsd.own.mk`).
- Phase 2: `minix/kernel/arch/riscv64` exists; runtime remains unstable (see `RISC64-STATUS.md`).
- Runtime issues and fixes are tracked in `issue.md`.
- 2026-01-07 doc sync: plan unchanged; status carried forward from 2026-01-06.

## Phase Overview / 阶段概览

**中文**
- Phase 0：依赖安装、QEMU virt 设备树与硬件地址确认
- Phase 1：构建系统识别 evbriscv64、RISC-V CPU 家族与编译标志
- Phase 2：内核架构层文件结构与基础实现
- Phase 3：异常/中断/时钟/VM 等核心链路
- Phase 4：VirtIO 设备驱动
- Phase 5：SMP 多核支持
- Phase 6：完整系统集成与回归

**English**
- Phase 0: dependencies, QEMU virt device tree, and hardware addresses
- Phase 1: build system recognition of evbriscv64, CPU family, and flags
- Phase 2: kernel architecture layer files and baseline implementation
- Phase 3: core paths (traps, IRQ, clock, VM)
- Phase 4: VirtIO device drivers
- Phase 5: SMP support
- Phase 6: full system integration and regression

**Note / 说明**
- 详细步骤以中文为主；命令与代码块为语言无关内容。  
  Detailed steps below are Chinese-first; commands and code blocks are language-neutral.

## 项目目标 / Goal

将 MINIX 3 移植到 RISC-V 64 位架构，达到与官方 x86_64 ISO 相同完成度。  
Port MINIX 3 to RISC-V 64-bit with parity to the official x86_64 ISO.

---

## 阶段 0: 环境准备与验证 / Phase 0: Environment Setup and Validation

### 0.1 安装依赖工具 / Install Dependencies

**Status / 状态**: Planned  
**Evidence / 证据**: N/A (host environment)

```bash
# Ubuntu/Debian
sudo apt-get install qemu-system-riscv64 device-tree-compiler \
    gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu

# 验证
qemu-system-riscv64 --version
riscv64-linux-gnu-gcc --version
```

### 0.2 获取 QEMU virt 平台设备树 / Get QEMU virt Device Tree

**Status / 状态**: Planned  
**Evidence / 证据**: N/A (host environment)

```bash
# 导出设备树供参考
qemu-system-riscv64 -M virt -machine dumpdtb=qemu-virt.dtb
dtc -I dtb -O dts qemu-virt.dtb > qemu-virt.dts

# 关键信息提取
grep -A5 "plic@" qemu-virt.dts     # PLIC 地址: 0x0c000000
grep -A5 "serial@" qemu-virt.dts   # UART 地址: 0x10000000
grep -A5 "virtio_mmio@" qemu-virt.dts  # VirtIO MMIO 区域
```

**检查点 / Checkpoint**: 设备树文件生成成功，记录关键硬件地址

---

## 阶段 1: 构建系统适配 / Phase 1: Build System Integration

### 1.1 修改 build.sh 添加 RISC-V 目标 / Add RISC-V Target to build.sh

**Status / 状态**: Done  
**Evidence / 证据**: `build.sh` includes evbriscv64 entries

**文件 / File**: `/path/to/minix/build.sh`
**位置 / Location**: 行 615-720 附近 (MACHINE/MACHINE_ARCH 定义区域)

```bash
# 找到添加位置
grep -n "evbarm" build.sh | head -5
```

**添加内容 / Add** (在 evbarm 定义后):
```bash
# 在约 653 行后添加
evbriscv64)
    MACHINE=evbriscv64
    MACHINE_ARCH=riscv64
    TOOLCHAIN_MISSING=yes  # 初始阶段使用外部工具链
    ;;
```

**验证命令 / Verify**:
```bash
./build.sh -m evbriscv64 list-arch
```

### 1.2 修改 bsd.own.mk 添加 RISC-V CPU 族 / Add RISC-V CPU Family in bsd.own.mk

**Status / 状态**: Done  
**Evidence / 证据**: `share/mk/bsd.own.mk` defines `RISCV_ARCH_FLAGS` and CFLAGS

**文件 / File**: `/path/to/minix/share/mk/bsd.own.mk`
**位置 / Location**: 行 188-189 (MACHINE_CPU 定义)

**当前代码 / Current**:
```makefile
MACHINE_CPU= ${MACHINE_ARCH:C/mipse[bl]/mips/:C/earm.*/arm/:C/riscv../riscv/}
```

**修改为 / Change to** (已支持，无需修改，确认即可):
```makefile
# riscv32 -> riscv, riscv64 -> riscv
# 规则 :C/riscv../riscv/ 已覆盖
```

**新增 RISC-V 编译标志 / Add RISC-V CFLAGS** (约行 120 后):
```makefile
.if ${MACHINE_ARCH} == "riscv64"
CFLAGS+= -march=rv64gc -mabi=lp64d
.endif
```

**验证命令 / Verify**:
```bash
grep -n "riscv" share/mk/bsd.own.mk
```

### 1.3 修改 sys/arch/Makefile / Update sys/arch/Makefile

**Status / 状态**: Done  
**Evidence / 证据**: `sys/arch/Makefile` contains riscv SUBDIR branch

**文件 / File**: `/path/to/minix/sys/arch/Makefile`
**位置 / Location**: 行 19-35 (SUBDIR 定义)

**添加内容 / Add**:
```makefile
.elif ${MACHINE_CPU} == "riscv"
.if defined(__MINIX)
SUBDIR= evbriscv64
.else
SUBDIR= riscv
.endif
```

### 1.4 创建 sys/arch/riscv 头文件目录 / Create sys/arch/riscv Headers

**Status / 状态**: Done  
**Evidence / 证据**: `sys/arch/riscv/include/` exists with headers

```bash
# 创建目录结构
mkdir -p sys/arch/riscv/include

# 从 NetBSD 或 Linux 参考创建基础头文件
```

**需要创建的头文件清单**:

| 文件 | 来源/参考 | 说明 |
|------|-----------|------|
| `asm.h` | arm/include/asm.h | 汇编宏定义 |
| `atomic.h` | common/lib/libc/arch/riscv/atomic/ | 原子操作 |
| `cdefs.h` | arm/include/cdefs.h | C 定义 |
| `cpu.h` | arm/include/cpu.h | CPU 定义 |
| `elf_machdep.h` | 新建 | ELF 机器类型 |
| `endian.h` | arm/include/endian.h | 字节序 |
| `float.h` | arm/include/float.h | 浮点定义 |
| `frame.h` | 新建 | 栈帧结构 |
| `int_types.h` | arm/include/int_types.h | 整数类型 |
| `limits.h` | arm/include/limits.h | 类型限制 |
| `param.h` | arm/include/param.h | 系统参数 |
| `pmap.h` | arm/include/pmap.h | 物理映射 |
| `ptrace.h` | arm/include/ptrace.h | 进程跟踪 |
| `reg.h` | 新建 | 寄存器定义 |
| `setjmp.h` | lib/libc/arch/riscv/gen/ | setjmp 定义 |
| `signal.h` | arm/include/signal.h | 信号定义 |
| `types.h` | arm/include/types.h | 基本类型 |
| `vmparam.h` | arm/include/vmparam.h | VM 参数 |

**示例: sys/arch/riscv/include/elf_machdep.h**:
```c
#ifndef _RISCV_ELF_MACHDEP_H_
#define _RISCV_ELF_MACHDEP_H_

#define ELF64_MACHDEP_ENDIANNESS    ELFDATA2LSB
#define ELF64_MACHDEP_ID_CASES      case EM_RISCV:
#define ELF64_MACHDEP_ID            EM_RISCV
#define ARCH_ELFSIZE                64

#define ELF_TARG_CLASS  ELFCLASS64
#define ELF_TARG_DATA   ELFDATA2LSB
#define ELF_TARG_MACH   EM_RISCV

#endif /* _RISCV_ELF_MACHDEP_H_ */
```

**示例: sys/arch/riscv/include/frame.h**:
```c
#ifndef _RISCV_FRAME_H_
#define _RISCV_FRAME_H_

struct trapframe {
    register_t tf_ra;      /* x1  - return address */
    register_t tf_sp;      /* x2  - stack pointer */
    register_t tf_gp;      /* x3  - global pointer */
    register_t tf_tp;      /* x4  - thread pointer */
    register_t tf_t[7];    /* x5-x7, x28-x31 - temporaries */
    register_t tf_s[12];   /* x8-x9, x18-x27 - saved registers */
    register_t tf_a[8];    /* x10-x17 - arguments */
    register_t tf_sepc;    /* exception PC */
    register_t tf_sstatus; /* status register */
    register_t tf_stval;   /* trap value */
    register_t tf_scause;  /* trap cause */
};

#endif /* _RISCV_FRAME_H_ */
```

**示例: sys/arch/riscv/include/reg.h**:
```c
#ifndef _RISCV_REG_H_
#define _RISCV_REG_H_

/* General purpose registers */
struct reg {
    register_t r_ra;
    register_t r_sp;
    register_t r_gp;
    register_t r_tp;
    register_t r_t0;
    register_t r_t1;
    register_t r_t2;
    register_t r_s0;
    register_t r_s1;
    register_t r_a0;
    register_t r_a1;
    register_t r_a2;
    register_t r_a3;
    register_t r_a4;
    register_t r_a5;
    register_t r_a6;
    register_t r_a7;
    register_t r_s2;
    register_t r_s3;
    register_t r_s4;
    register_t r_s5;
    register_t r_s6;
    register_t r_s7;
    register_t r_s8;
    register_t r_s9;
    register_t r_s10;
    register_t r_s11;
    register_t r_t3;
    register_t r_t4;
    register_t r_t5;
    register_t r_t6;
    register_t r_pc;
};

/* Floating point registers */
struct fpreg {
    __uint64_t fp_f[32];
    __uint64_t fp_fcsr;
};

#endif /* _RISCV_REG_H_ */
```

### 1.5 创建 sys/arch/evbriscv64 评估板目录 / Create sys/arch/evbriscv64 Board Dir

**Status / 状态**: Done  
**Evidence / 证据**: `sys/arch/evbriscv64/Makefile` and `sys/arch/evbriscv64/include/`

```bash
mkdir -p sys/arch/evbriscv64/include
```

**文件 / File**: `sys/arch/evbriscv64/include/Makefile`
```makefile
INCSDIR= /usr/include/evbriscv64

.include "../../riscv/include/Makefile.common"
```

**文件 / File**: `sys/arch/evbriscv64/Makefile`
```makefile
.include <bsd.own.mk>

# evbriscv64 specific subdirs
SUBDIR= include

.include <bsd.subdir.mk>
```

**阶段 1 检查点 / Phase 1 Checkpoint**:
```bash
# 1. build.sh 识别新架构
./build.sh -m evbriscv64 list-arch

# 2. 头文件目录存在
ls sys/arch/riscv/include/
ls sys/arch/evbriscv64/include/
```

---

## 阶段 2: MINIX 内核架构层 / Phase 2: Kernel Architecture Layer

### 2.1 创建 minix/kernel/arch/riscv64 目录结构 / Create minix/kernel/arch/riscv64 Layout

**Status / 状态**: Done  
**Evidence / 证据**: `minix/kernel/arch/riscv64/` directory exists

```bash
mkdir -p minix/kernel/arch/riscv64/{include,bsp/virt}
```

### 2.2 创建 Makefile.inc / Create Makefile.inc

**Status / 状态**: Done  
**Evidence / 证据**: `minix/kernel/arch/riscv64/Makefile.inc`

**文件 / File**: `minix/kernel/arch/riscv64/Makefile.inc`

```makefile
# MINIX kernel RISC-V 64 architecture Makefile

HERE=${.CURDIR}/arch/${MACHINE_ARCH}
.PATH: ${HERE}

# 未分页对象 (物理内存驻留)
MINLIB_OBJS_UNPAGED=
SYS_OBJS_UNPAGED= assert.o stacktrace.o
MINC_OBJS_UNPAGED= atoi.o printf.o strcmp.o strlen.o

# 核心源文件
SRCS+= head.S
SRCS+= mpx.S
SRCS+= klib.S
SRCS+= exception.c
SRCS+= hw_intr.c
SRCS+= arch_clock.c
SRCS+= arch_system.c
SRCS+= arch_do_vmctl.c
SRCS+= arch_reset.c
SRCS+= memory.c
SRCS+= protect.c
SRCS+= pg_utils.c
SRCS+= phys_copy.S
SRCS+= phys_memset.S
SRCS+= direct_tty_utils.c
SRCS+= sbi.c
SRCS+= plic.c

# Board Support Package
.include "bsp/virt/Makefile.inc"

# 生成处理器偏移
PROCOFFSETSCF= ${HERE}/procoffsets.cf
procoffsets.h: ${PROCOFFSETSCF}
	${_MKTARGET_CREATE}
	${TOOL_GENASSYM} -- ${CC} ${CFLAGS} ${CPPFLAGS} ${PROF} \
	    < ${PROCOFFSETSCF} > procoffsets.h.tmp && \
	mv -f procoffsets.h.tmp procoffsets.h

# 链接脚本
EXTRA_OBJS+= ${HERE}/kernel.lds
LDFLAGS+= -T ${HERE}/kernel.lds
```

### 2.3 创建头文件 / Create Headers

**Status / 状态**: Done  
**Evidence / 证据**: `minix/kernel/arch/riscv64/include/archconst.h`, `arch_proto.h`, `hw_intr.h`

**文件 / File**: `minix/kernel/arch/riscv64/include/archconst.h`

```c
#ifndef _RISCV64_ARCHCONST_H
#define _RISCV64_ARCHCONST_H

/* 系统时钟频率 */
#define DEFAULT_HZ          1000

/* 页大小 */
#define RISCV_PAGE_SIZE     4096
#define RISCV_PAGE_SHIFT    12

/* Sv39 分页模式 (3级页表, 39位虚拟地址) */
#define RISCV_LEVELS        3
#define RISCV_PTES_PER_PT   512

/* 虚拟地址空间布局 */
#define KERNEL_BASE         0xFFFFFFC000000000UL  /* -256GB */
#define USER_MAX            0x0000003FFFFFFFFFUL  /* 256GB */

/* QEMU virt 平台地址 */
#define VIRT_UART0_BASE     0x10000000UL
#define VIRT_PLIC_BASE      0x0C000000UL
#define VIRT_CLINT_BASE     0x02000000UL

/* PLIC 常量 */
#define PLIC_NUM_SOURCES    1024
#define PLIC_NUM_PRIORITIES 7

/* 中断号 */
#define IRQ_UART0           10
#define IRQ_VIRTIO_START    1   /* VirtIO 设备从 IRQ 1 开始 */

#endif /* _RISCV64_ARCHCONST_H */
```

**文件 / File**: `minix/kernel/arch/riscv64/include/arch_proto.h`

```c
#ifndef _RISCV64_ARCH_PROTO_H
#define _RISCV64_ARCH_PROTO_H

#include <machine/types.h>

/* head.S */
void _C_LABEL(kernel_entry)(void);

/* exception.c */
void exception_init(void);
void exception_handler(struct trapframe *tf);

/* hw_intr.c */
void hw_intr_init(void);
void hw_intr_mask(int irq);
void hw_intr_unmask(int irq);

/* plic.c */
void plic_init(void);
void plic_enable_irq(int irq, int cpu);
void plic_disable_irq(int irq);
int  plic_claim(int cpu);
void plic_complete(int cpu, int irq);
void plic_set_priority(int irq, int priority);

/* sbi.c */
void sbi_console_putchar(int ch);
int  sbi_console_getchar(void);
void sbi_set_timer(uint64_t stime_value);
void sbi_send_ipi(unsigned long hart_mask);
void sbi_remote_fence_i(unsigned long hart_mask);
void sbi_shutdown(void);
void sbi_system_reset(uint32_t reset_type, uint32_t reset_reason);

/* arch_clock.c */
void arch_init_clock(void);
void arch_stop_clock(void);
int  arch_clock_handler(struct trapframe *tf);

/* arch_system.c */
void arch_system_init(void);
void arch_shutdown(int how);

/* memory.c */
void mem_init(void);
void vm_init(void);
phys_bytes alloc_lowest(phys_bytes size);

/* protect.c */
void prot_init(void);

/* pg_utils.c */
void pg_map(phys_bytes phys, vir_bytes virt, vir_bytes size, uint64_t flags);
void pg_unmap(vir_bytes virt, vir_bytes size);
void pg_flush_tlb(void);

/* phys_copy.S */
int  phys_copy(phys_bytes src, phys_bytes dst, phys_bytes size);
void phys_memset(phys_bytes dst, unsigned long c, phys_bytes size);

/* direct_tty_utils.c */
void direct_print(const char *str);
void direct_putc(int c);

/* arch_do_vmctl.c */
int arch_do_vmctl(message *m_ptr, struct proc *p);

/* arch_reset.c */
void arch_reset(void);

/* CSR 操作 (内联汇编) */
static inline uint64_t csr_read_sstatus(void) {
    uint64_t val;
    __asm__ __volatile__("csrr %0, sstatus" : "=r"(val));
    return val;
}

static inline void csr_write_sstatus(uint64_t val) {
    __asm__ __volatile__("csrw sstatus, %0" :: "r"(val));
}

static inline uint64_t csr_read_sie(void) {
    uint64_t val;
    __asm__ __volatile__("csrr %0, sie" : "=r"(val));
    return val;
}

static inline void csr_write_sie(uint64_t val) {
    __asm__ __volatile__("csrw sie, %0" :: "r"(val));
}

static inline void csr_set_sie(uint64_t bits) {
    __asm__ __volatile__("csrs sie, %0" :: "r"(bits));
}

static inline void csr_clear_sie(uint64_t bits) {
    __asm__ __volatile__("csrc sie, %0" :: "r"(bits));
}

static inline uint64_t csr_read_sip(void) {
    uint64_t val;
    __asm__ __volatile__("csrr %0, sip" : "=r"(val));
    return val;
}

static inline uint64_t csr_read_scause(void) {
    uint64_t val;
    __asm__ __volatile__("csrr %0, scause" : "=r"(val));
    return val;
}

static inline uint64_t csr_read_stval(void) {
    uint64_t val;
    __asm__ __volatile__("csrr %0, stval" : "=r"(val));
    return val;
}

static inline uint64_t csr_read_sepc(void) {
    uint64_t val;
    __asm__ __volatile__("csrr %0, sepc" : "=r"(val));
    return val;
}

static inline void csr_write_sepc(uint64_t val) {
    __asm__ __volatile__("csrw sepc, %0" :: "r"(val));
}

static inline void csr_write_stvec(uint64_t val) {
    __asm__ __volatile__("csrw stvec, %0" :: "r"(val));
}

static inline void csr_write_sscratch(uint64_t val) {
    __asm__ __volatile__("csrw sscratch, %0" :: "r"(val));
}

static inline void csr_write_satp(uint64_t val) {
    __asm__ __volatile__("csrw satp, %0" :: "r"(val));
    __asm__ __volatile__("sfence.vma" ::: "memory");
}

static inline uint64_t csr_read_time(void) {
    uint64_t val;
    __asm__ __volatile__("csrr %0, time" : "=r"(val));
    return val;
}

#endif /* _RISCV64_ARCH_PROTO_H */
```

**文件 / File**: `minix/kernel/arch/riscv64/include/hw_intr.h`

```c
#ifndef _RISCV64_HW_INTR_H
#define _RISCV64_HW_INTR_H

/* 中断类型 */
#define INTR_TYPE_PLIC      0   /* 外部中断 (通过 PLIC) */
#define INTR_TYPE_TIMER     1   /* 定时器中断 */
#define INTR_TYPE_SOFTWARE  2   /* 软件中断 (IPI) */

/* PLIC 寄存器偏移 */
#define PLIC_PRIORITY_BASE      0x000000
#define PLIC_PENDING_BASE       0x001000
#define PLIC_ENABLE_BASE        0x002000
#define PLIC_THRESHOLD_BASE     0x200000
#define PLIC_CLAIM_BASE         0x200004

/* 每个 hart 的 context 大小 */
#define PLIC_CONTEXT_STRIDE     0x1000
#define PLIC_ENABLE_STRIDE      0x80

#endif /* _RISCV64_HW_INTR_H */
```

### 2.4 创建 SBI 接口 / Create SBI Interface

**Status / 状态**: Partial  
**Evidence / 证据**: `minix/kernel/arch/riscv64/sbi.c` present; legacy IPI/fence uses VA (see `issue.md`)

**文件 / File**: `minix/kernel/arch/riscv64/sbi.c` (~100 行新代码)

```c
/*
 * RISC-V SBI (Supervisor Binary Interface) 接口
 * 用于与 OpenSBI/BBL 固件通信
 */

#include "kernel/kernel.h"
#include "arch_proto.h"

/* SBI 扩展 ID */
#define SBI_EXT_0_1_CONSOLE_PUTCHAR   0x01
#define SBI_EXT_0_1_CONSOLE_GETCHAR   0x02
#define SBI_EXT_0_1_SET_TIMER         0x00
#define SBI_EXT_0_1_SEND_IPI          0x04
#define SBI_EXT_0_1_REMOTE_FENCE_I    0x05
#define SBI_EXT_0_1_SHUTDOWN          0x08

/* SBI v0.2+ 扩展 */
#define SBI_EXT_BASE                  0x10
#define SBI_EXT_TIME                  0x54494D45  /* "TIME" */
#define SBI_EXT_IPI                   0x735049    /* "sPI" */
#define SBI_EXT_RFENCE                0x52464E43  /* "RFNC" */
#define SBI_EXT_HSM                   0x48534D    /* "HSM" */
#define SBI_EXT_SRST                  0x53525354  /* "SRST" */

/* SBI 返回值 */
struct sbiret {
    long error;
    long value;
};

/* SBI 调用封装 */
static struct sbiret sbi_ecall(int ext, int fid, unsigned long arg0,
    unsigned long arg1, unsigned long arg2, unsigned long arg3,
    unsigned long arg4, unsigned long arg5)
{
    struct sbiret ret;

    register unsigned long a0 __asm__("a0") = arg0;
    register unsigned long a1 __asm__("a1") = arg1;
    register unsigned long a2 __asm__("a2") = arg2;
    register unsigned long a3 __asm__("a3") = arg3;
    register unsigned long a4 __asm__("a4") = arg4;
    register unsigned long a5 __asm__("a5") = arg5;
    register unsigned long a6 __asm__("a6") = fid;
    register unsigned long a7 __asm__("a7") = ext;

    __asm__ __volatile__(
        "ecall"
        : "+r"(a0), "+r"(a1)
        : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
        : "memory"
    );

    ret.error = a0;
    ret.value = a1;
    return ret;
}

/* 控制台输出 (调试用) */
void sbi_console_putchar(int ch)
{
    sbi_ecall(SBI_EXT_0_1_CONSOLE_PUTCHAR, 0, ch, 0, 0, 0, 0, 0);
}

/* 控制台输入 */
int sbi_console_getchar(void)
{
    struct sbiret ret;
    ret = sbi_ecall(SBI_EXT_0_1_CONSOLE_GETCHAR, 0, 0, 0, 0, 0, 0, 0);
    return ret.error;
}

/* 设置定时器 */
void sbi_set_timer(uint64_t stime_value)
{
    sbi_ecall(SBI_EXT_0_1_SET_TIMER, 0, stime_value, 0, 0, 0, 0, 0);
}

/* 发送核间中断 */
void sbi_send_ipi(unsigned long hart_mask)
{
    sbi_ecall(SBI_EXT_0_1_SEND_IPI, 0, (unsigned long)&hart_mask, 0, 0, 0, 0, 0);
}

/* 远程指令缓存刷新 */
void sbi_remote_fence_i(unsigned long hart_mask)
{
    sbi_ecall(SBI_EXT_0_1_REMOTE_FENCE_I, 0, (unsigned long)&hart_mask, 0, 0, 0, 0, 0);
}

/* 关机 (旧版 SBI) */
void sbi_shutdown(void)
{
    sbi_ecall(SBI_EXT_0_1_SHUTDOWN, 0, 0, 0, 0, 0, 0, 0);
}

/*
 * 系统重启 (SBI v0.2+ SRST 扩展)
 * reset_type: 0=shutdown, 1=cold reboot, 2=warm reboot
 * reset_reason: 0=none, 1=system failure
 *
 * 这是约 20 行的新代码
 */
void sbi_system_reset(uint32_t reset_type, uint32_t reset_reason)
{
    struct sbiret ret;

    /* 尝试 SRST 扩展 (SBI v0.2+) */
    ret = sbi_ecall(SBI_EXT_SRST, 0, reset_type, reset_reason, 0, 0, 0, 0);

    /* 如果 SRST 不支持，回退到旧版关机 */
    if (ret.error != 0) {
        sbi_shutdown();
    }

    /* 不应到达这里 */
    while (1) {
        __asm__ __volatile__("wfi");
    }
}
```

### 2.5 创建 PLIC 驱动 / Create PLIC Driver

**Status / 状态**: Done  
**Evidence / 证据**: `minix/kernel/arch/riscv64/plic.c`

**文件 / File**: `minix/kernel/arch/riscv64/plic.c` (~150 行，含关键新代码)

```c
/*
 * RISC-V PLIC (Platform-Level Interrupt Controller) 驱动
 */

#include "kernel/kernel.h"
#include "arch_proto.h"
#include "archconst.h"
#include "hw_intr.h"

static volatile uint32_t *plic_base;

/* PLIC 寄存器访问 */
#define PLIC_PRIORITY(irq)      (plic_base + (irq))
#define PLIC_PENDING(irq)       (plic_base + (0x1000/4) + ((irq)/32))
#define PLIC_ENABLE(ctx, irq)   (plic_base + (0x2000/4) + (ctx)*0x20 + ((irq)/32))
#define PLIC_THRESHOLD(ctx)     (plic_base + (0x200000/4) + (ctx)*0x400)
#define PLIC_CLAIM(ctx)         (plic_base + (0x200004/4) + (ctx)*0x400)

/* 每个 hart 有两个 context: M-mode 和 S-mode */
#define PLIC_CONTEXT_S(hart)    ((hart) * 2 + 1)

void plic_init(void)
{
    int i;

    plic_base = (volatile uint32_t *)VIRT_PLIC_BASE;

    /* 禁用所有中断，设置优先级为 0 */
    for (i = 1; i < PLIC_NUM_SOURCES; i++) {
        *PLIC_PRIORITY(i) = 0;
    }

    /* 设置 hart 0 S-mode 的阈值为 0 (接受所有优先级 > 0 的中断) */
    *PLIC_THRESHOLD(PLIC_CONTEXT_S(0)) = 0;
}

void plic_set_priority(int irq, int priority)
{
    if (irq > 0 && irq < PLIC_NUM_SOURCES) {
        *PLIC_PRIORITY(irq) = priority;
    }
}

void plic_enable_irq(int irq, int cpu)
{
    int ctx = PLIC_CONTEXT_S(cpu);
    uint32_t mask;

    if (irq <= 0 || irq >= PLIC_NUM_SOURCES)
        return;

    mask = *PLIC_ENABLE(ctx, irq);
    mask |= (1U << (irq % 32));
    *PLIC_ENABLE(ctx, irq) = mask;

    /* 设置优先级为 1 (大于阈值 0) */
    plic_set_priority(irq, 1);
}

void plic_disable_irq(int irq)
{
    int cpu, ctx;
    uint32_t mask;

    if (irq <= 0 || irq >= PLIC_NUM_SOURCES)
        return;

    /* 在所有 CPU 上禁用 */
    for (cpu = 0; cpu < CONFIG_MAX_CPUS; cpu++) {
        ctx = PLIC_CONTEXT_S(cpu);
        mask = *PLIC_ENABLE(ctx, irq);
        mask &= ~(1U << (irq % 32));
        *PLIC_ENABLE(ctx, irq) = mask;
    }

    plic_set_priority(irq, 0);
}

/* 声明中断处理权 */
int plic_claim(int cpu)
{
    int ctx = PLIC_CONTEXT_S(cpu);
    return *PLIC_CLAIM(ctx);
}

/* 完成中断处理 */
void plic_complete(int cpu, int irq)
{
    int ctx = PLIC_CONTEXT_S(cpu);
    *PLIC_CLAIM(ctx) = irq;
}

/*
 * 关键新代码: plic_irq_cpu_mask() (~50行)
 * 设置中断路由到指定 CPU 集合
 * 用于 SMP 模式下的中断亲和性
 */
void plic_irq_cpu_mask(int irq, uint32_t cpu_mask)
{
    int cpu, ctx;
    uint32_t enable_mask;

    if (irq <= 0 || irq >= PLIC_NUM_SOURCES)
        return;

    for (cpu = 0; cpu < CONFIG_MAX_CPUS; cpu++) {
        ctx = PLIC_CONTEXT_S(cpu);
        enable_mask = *PLIC_ENABLE(ctx, irq);

        if (cpu_mask & (1U << cpu)) {
            /* 在此 CPU 上启用 */
            enable_mask |= (1U << (irq % 32));
        } else {
            /* 在此 CPU 上禁用 */
            enable_mask &= ~(1U << (irq % 32));
        }

        *PLIC_ENABLE(ctx, irq) = enable_mask;
    }
}

/* 获取 IRQ 当前路由到哪些 CPU */
uint32_t plic_irq_get_cpu_mask(int irq)
{
    int cpu, ctx;
    uint32_t cpu_mask = 0;
    uint32_t enable_mask;

    if (irq <= 0 || irq >= PLIC_NUM_SOURCES)
        return 0;

    for (cpu = 0; cpu < CONFIG_MAX_CPUS; cpu++) {
        ctx = PLIC_CONTEXT_S(cpu);
        enable_mask = *PLIC_ENABLE(ctx, irq);
        if (enable_mask & (1U << (irq % 32))) {
            cpu_mask |= (1U << cpu);
        }
    }

    return cpu_mask;
}
```

### 2.6 创建启动汇编 / Create Boot Assembly

**Status / 状态**: Done  
**Evidence / 证据**: `minix/kernel/arch/riscv64/head.S`, `exc.S`, `mpx.S`

**文件 / File**: `minix/kernel/arch/riscv64/head.S` (~200 行)

```assembly
/*
 * MINIX RISC-V 64 内核入口
 */

#include "archconst.h"
#include "sconst.h"

    .section .text.boot
    .globl _start
    .globl kernel_entry

/*
 * 内核入口点
 * a0 = hart id
 * a1 = device tree pointer
 */
_start:
kernel_entry:
    /* 禁用中断 */
    csrw    sie, zero
    csrw    sip, zero

    /* 设置 gp (全局指针) */
    .option push
    .option norelax
    la      gp, __global_pointer$
    .option pop

    /* 只让 hart 0 继续，其他 hart 进入等待 */
    bnez    a0, secondary_hart_wait

    /* 保存 device tree 指针 */
    la      t0, _bootinfo_dtb
    sd      a1, 0(t0)

    /* 清零 BSS 段 */
    la      t0, __bss_start
    la      t1, __bss_end
1:
    sd      zero, 0(t0)
    addi    t0, t0, 8
    blt     t0, t1, 1b

    /* 设置栈指针 */
    la      sp, _boot_stack_top

    /* 设置异常向量 */
    la      t0, trap_entry
    csrw    stvec, t0

    /* 跳转到 C 代码 */
    call    pre_init
    call    main

    /* 不应到达这里 */
halt:
    wfi
    j       halt

/*
 * 从核等待入口
 */
secondary_hart_wait:
    /* 保存 hart id */
    mv      s0, a0

    /* 等待主核发送启动信号 */
1:
    la      t0, _smp_boot_flag
    ld      t1, 0(t0)
    beqz    t1, 1b

    /* 设置每核栈 */
    la      sp, _percpu_stack_top
    li      t0, STACK_SIZE
    mul     t1, s0, t0
    sub     sp, sp, t1

    /* 设置异常向量 */
    la      t0, trap_entry
    csrw    stvec, t0

    /* 跳转到从核初始化 */
    mv      a0, s0
    call    smp_ap_entry

    j       halt

/*
 * 异常/中断入口
 */
    .align 4
trap_entry:
    /* 保存所有寄存器 */
    addi    sp, sp, -TRAPFRAME_SIZE

    sd      ra,  TF_RA(sp)
    sd      gp,  TF_GP(sp)
    sd      tp,  TF_TP(sp)
    sd      t0,  TF_T0(sp)
    sd      t1,  TF_T1(sp)
    sd      t2,  TF_T2(sp)
    sd      s0,  TF_S0(sp)
    sd      s1,  TF_S1(sp)
    sd      a0,  TF_A0(sp)
    sd      a1,  TF_A1(sp)
    sd      a2,  TF_A2(sp)
    sd      a3,  TF_A3(sp)
    sd      a4,  TF_A4(sp)
    sd      a5,  TF_A5(sp)
    sd      a6,  TF_A6(sp)
    sd      a7,  TF_A7(sp)
    sd      s2,  TF_S2(sp)
    sd      s3,  TF_S3(sp)
    sd      s4,  TF_S4(sp)
    sd      s5,  TF_S5(sp)
    sd      s6,  TF_S6(sp)
    sd      s7,  TF_S7(sp)
    sd      s8,  TF_S8(sp)
    sd      s9,  TF_S9(sp)
    sd      s10, TF_S10(sp)
    sd      s11, TF_S11(sp)
    sd      t3,  TF_T3(sp)
    sd      t4,  TF_T4(sp)
    sd      t5,  TF_T5(sp)
    sd      t6,  TF_T6(sp)

    /* 保存 CSR */
    csrr    t0, sepc
    sd      t0, TF_SEPC(sp)
    csrr    t0, sstatus
    sd      t0, TF_SSTATUS(sp)
    csrr    t0, scause
    sd      t0, TF_SCAUSE(sp)
    csrr    t0, stval
    sd      t0, TF_STVAL(sp)

    /* 调用 C 处理函数 */
    mv      a0, sp
    call    exception_handler

    /* 恢复 CSR */
    ld      t0, TF_SEPC(sp)
    csrw    sepc, t0
    ld      t0, TF_SSTATUS(sp)
    csrw    sstatus, t0

    /* 恢复寄存器 */
    ld      ra,  TF_RA(sp)
    ld      gp,  TF_GP(sp)
    ld      tp,  TF_TP(sp)
    ld      t0,  TF_T0(sp)
    ld      t1,  TF_T1(sp)
    ld      t2,  TF_T2(sp)
    ld      s0,  TF_S0(sp)
    ld      s1,  TF_S1(sp)
    ld      a0,  TF_A0(sp)
    ld      a1,  TF_A1(sp)
    ld      a2,  TF_A2(sp)
    ld      a3,  TF_A3(sp)
    ld      a4,  TF_A4(sp)
    ld      a5,  TF_A5(sp)
    ld      a6,  TF_A6(sp)
    ld      a7,  TF_A7(sp)
    ld      s2,  TF_S2(sp)
    ld      s3,  TF_S3(sp)
    ld      s4,  TF_S4(sp)
    ld      s5,  TF_S5(sp)
    ld      s6,  TF_S6(sp)
    ld      s7,  TF_S7(sp)
    ld      s8,  TF_S8(sp)
    ld      s9,  TF_S9(sp)
    ld      s10, TF_S10(sp)
    ld      s11, TF_S11(sp)
    ld      t3,  TF_T3(sp)
    ld      t4,  TF_T4(sp)
    ld      t5,  TF_T5(sp)
    ld      t6,  TF_T6(sp)

    addi    sp, sp, TRAPFRAME_SIZE
    sret

/*
 * 数据段
 */
    .section .data
    .align 3
_bootinfo_dtb:
    .quad   0

_smp_boot_flag:
    .quad   0

    .section .bss
    .align 12
_boot_stack:
    .space  4096
_boot_stack_top:

_percpu_stack:
    .space  4096 * 16   /* 16 核 */
_percpu_stack_top:
```

### 2.7 创建链接脚本 / Create Linker Script

**Status / 状态**: Done  
**Evidence / 证据**: `minix/kernel/arch/riscv64/kernel.lds`

**文件 / File**: `minix/kernel/arch/riscv64/kernel.lds`

```ld
/*
 * MINIX RISC-V 64 内核链接脚本
 */

OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
    /* QEMU virt 平台 RAM 起始于 0x80000000 */
    ram (rwx) : ORIGIN = 0x80200000, LENGTH = 128M
}

SECTIONS
{
    . = ORIGIN(ram);

    .text : {
        *(.text.boot)
        *(.text .text.*)
    } > ram

    . = ALIGN(4096);
    .rodata : {
        *(.rodata .rodata.*)
    } > ram

    . = ALIGN(4096);
    .data : {
        __data_start = .;
        *(.data .data.*)
        __data_end = .;
    } > ram

    . = ALIGN(8);
    __global_pointer$ = . + 0x800;
    .sdata : {
        *(.sdata .sdata.*)
    } > ram

    . = ALIGN(4096);
    .bss : {
        __bss_start = .;
        *(.sbss .sbss.*)
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    } > ram

    . = ALIGN(4096);
    _end = .;

    /DISCARD/ : {
        *(.comment)
        *(.note.*)
    }
}
```

### 2.8 创建 trapframe 常量定义 / Create trapframe Constants

**Status / 状态**: Done  
**Evidence / 证据**: `minix/kernel/arch/riscv64/include/sconst.h`

**文件 / File**: `minix/kernel/arch/riscv64/sconst.h`

```c
#ifndef _RISCV64_SCONST_H
#define _RISCV64_SCONST_H

/* Trapframe 寄存器偏移 */
#define TF_RA       (0 * 8)
#define TF_SP       (1 * 8)
#define TF_GP       (2 * 8)
#define TF_TP       (3 * 8)
#define TF_T0       (4 * 8)
#define TF_T1       (5 * 8)
#define TF_T2       (6 * 8)
#define TF_S0       (7 * 8)
#define TF_S1       (8 * 8)
#define TF_A0       (9 * 8)
#define TF_A1       (10 * 8)
#define TF_A2       (11 * 8)
#define TF_A3       (12 * 8)
#define TF_A4       (13 * 8)
#define TF_A5       (14 * 8)
#define TF_A6       (15 * 8)
#define TF_A7       (16 * 8)
#define TF_S2       (17 * 8)
#define TF_S3       (18 * 8)
#define TF_S4       (19 * 8)
#define TF_S5       (20 * 8)
#define TF_S6       (21 * 8)
#define TF_S7       (22 * 8)
#define TF_S8       (23 * 8)
#define TF_S9       (24 * 8)
#define TF_S10      (25 * 8)
#define TF_S11      (26 * 8)
#define TF_T3       (27 * 8)
#define TF_T4       (28 * 8)
#define TF_T5       (29 * 8)
#define TF_T6       (30 * 8)
#define TF_SEPC     (31 * 8)
#define TF_SSTATUS  (32 * 8)
#define TF_SCAUSE   (33 * 8)
#define TF_STVAL    (34 * 8)

#define TRAPFRAME_SIZE  (35 * 8)

/* 栈大小 */
#define STACK_SIZE  4096

#endif /* _RISCV64_SCONST_H */
```

**阶段 2 检查点 / Phase 2 Checkpoint**:
```bash
# 所有文件创建完成
ls minix/kernel/arch/riscv64/

# 尝试编译 (会有缺失文件错误，逐步补全)
./build.sh -m evbriscv64 kernel 2>&1 | head -50
```

---

## 阶段 3: 核心内核功能实现 / Phase 3: Core Kernel Functionality

### 3.1 异常处理 / Trap and Exception Handling

**Status / 状态**: Partial  
**Evidence / 证据**: `minix/kernel/arch/riscv64/exception.c`; issues in `issue.md` (breakpoint length, SSIE, pagefault msg)

**文件 / File**: `minix/kernel/arch/riscv64/exception.c`

```c
/*
 * RISC-V 异常处理
 */

#include "kernel/kernel.h"
#include "arch_proto.h"
#include "archconst.h"

/* RISC-V 异常原因码 */
#define CAUSE_MISALIGNED_FETCH      0
#define CAUSE_FETCH_ACCESS          1
#define CAUSE_ILLEGAL_INSTRUCTION   2
#define CAUSE_BREAKPOINT            3
#define CAUSE_MISALIGNED_LOAD       4
#define CAUSE_LOAD_ACCESS           5
#define CAUSE_MISALIGNED_STORE      6
#define CAUSE_STORE_ACCESS          7
#define CAUSE_USER_ECALL            8
#define CAUSE_SUPERVISOR_ECALL      9
#define CAUSE_MACHINE_ECALL         11
#define CAUSE_FETCH_PAGE_FAULT      12
#define CAUSE_LOAD_PAGE_FAULT       13
#define CAUSE_STORE_PAGE_FAULT      15

/* 中断位 (scause 最高位) */
#define CAUSE_INTERRUPT             (1UL << 63)

/* 中断类型 */
#define IRQ_S_SOFT                  1
#define IRQ_S_TIMER                 5
#define IRQ_S_EXT                   9

void exception_init(void)
{
    /* 启用 S 模式中断 */
    csr_set_sie(SIE_SEIE | SIE_STIE | SIE_SSIE);
}

void exception_handler(struct trapframe *tf)
{
    uint64_t cause = tf->tf_scause;
    uint64_t is_interrupt = cause & CAUSE_INTERRUPT;

    cause &= ~CAUSE_INTERRUPT;

    if (is_interrupt) {
        /* 中断处理 */
        switch (cause) {
        case IRQ_S_TIMER:
            /* 定时器中断 */
            arch_clock_handler(tf);
            break;

        case IRQ_S_EXT:
            /* 外部中断 (PLIC) */
            hw_intr_handler(tf);
            break;

        case IRQ_S_SOFT:
            /* 软件中断 (IPI) */
            smp_ipi_handler(tf);
            break;

        default:
            panic("Unknown interrupt: %ld", cause);
        }
    } else {
        /* 异常处理 */
        switch (cause) {
        case CAUSE_USER_ECALL:
            /* 用户态系统调用 */
            tf->tf_sepc += 4;  /* 跳过 ecall 指令 */
            do_ipc(tf);
            break;

        case CAUSE_FETCH_PAGE_FAULT:
        case CAUSE_LOAD_PAGE_FAULT:
        case CAUSE_STORE_PAGE_FAULT:
            /* 页错误，转发到 VM */
            do_pagefault(tf);
            break;

        case CAUSE_ILLEGAL_INSTRUCTION:
            /* 非法指令 */
            do_signal(current_proc, SIGILL);
            break;

        default:
            panic("Unhandled exception: %ld at %lx",
                  cause, tf->tf_sepc);
        }
    }
}
```

### 3.2 时钟驱动 / Timer and Clock Driver

**Status / 状态**: Partial  
**Evidence / 证据**: `minix/kernel/arch/riscv64/arch_clock.c` wires `arch_clock_handler()` to `timer_int_handler()`;
timer IRQ dispatches through `minix/kernel/arch/riscv64/exception.c` (runtime validation pending).

**文件 / File**: `minix/kernel/arch/riscv64/arch_clock.c`

```c
/*
 * RISC-V 时钟驱动
 */

#include "kernel/kernel.h"
#include "arch_proto.h"
#include "archconst.h"

/* QEMU virt 平台时钟频率 (10 MHz) */
#define CLOCK_FREQ      10000000UL

static uint64_t ticks_per_hz;

void arch_init_clock(void)
{
    ticks_per_interrupt = timer_freq / system_hz;
    next_timer_deadline = csr_read_time() + ticks_per_interrupt;
    sbi_set_timer(next_timer_deadline);
    csr_set_sie(SIE_STIE);
}

int arch_clock_handler(void)
{
    return timer_int_handler();
}

void arch_timer_int_handler(void)
{
    u64_t now = csr_read_time();
    next_timer_deadline += ticks_per_interrupt;
    if (next_timer_deadline <= now)
        next_timer_deadline = now + ticks_per_interrupt;
    sbi_set_timer(next_timer_deadline);
    total_ticks++;
}
```

### 3.3 VirtIO 公共层 (关键: virtio_fence) / VirtIO Common Layer (key: virtio_fence)

**Status / 状态**: Planned  
**Evidence / 证据**: no `virtio_fence` symbol found; no `minix/drivers/virtio/` directory

**文件 / File**: `minix/drivers/virtio/virtio_riscv.h`

```c
/*
 * RISC-V VirtIO MMIO 访问封装
 * 关键新代码: virtio_fence() (~30行)
 */

#ifndef _VIRTIO_RISCV_H
#define _VIRTIO_RISCV_H

#include <stdint.h>

/*
 * RISC-V 内存屏障
 * 确保 VirtIO 描述符在设备可见之前完成写入
 */
static inline void virtio_mb(void)
{
    __asm__ __volatile__("fence iorw, iorw" ::: "memory");
}

static inline void virtio_rmb(void)
{
    __asm__ __volatile__("fence ir, ir" ::: "memory");
}

static inline void virtio_wmb(void)
{
    __asm__ __volatile__("fence ow, ow" ::: "memory");
}

/*
 * virtio_fence() - VirtIO 操作屏障
 *
 * 在以下场景调用:
 * 1. 写入描述符后，更新 avail ring 前
 * 2. 读取 used ring 前
 * 3. 通知设备前
 */
static inline void virtio_fence(void)
{
    __asm__ __volatile__("fence iorw, iorw" ::: "memory");
}

/* MMIO 读写 */
static inline uint8_t virtio_read8(volatile void *addr)
{
    uint8_t val;
    __asm__ __volatile__("lb %0, 0(%1)" : "=r"(val) : "r"(addr));
    return val;
}

static inline uint16_t virtio_read16(volatile void *addr)
{
    uint16_t val;
    __asm__ __volatile__("lh %0, 0(%1)" : "=r"(val) : "r"(addr));
    return val;
}

static inline uint32_t virtio_read32(volatile void *addr)
{
    uint32_t val;
    __asm__ __volatile__("lw %0, 0(%1)" : "=r"(val) : "r"(addr));
    return val;
}

static inline uint64_t virtio_read64(volatile void *addr)
{
    uint64_t val;
    __asm__ __volatile__("ld %0, 0(%1)" : "=r"(val) : "r"(addr));
    return val;
}

static inline void virtio_write8(volatile void *addr, uint8_t val)
{
    __asm__ __volatile__("sb %0, 0(%1)" :: "r"(val), "r"(addr));
}

static inline void virtio_write16(volatile void *addr, uint16_t val)
{
    __asm__ __volatile__("sh %0, 0(%1)" :: "r"(val), "r"(addr));
}

static inline void virtio_write32(volatile void *addr, uint32_t val)
{
    __asm__ __volatile__("sw %0, 0(%1)" :: "r"(val), "r"(addr));
}

static inline void virtio_write64(volatile void *addr, uint64_t val)
{
    __asm__ __volatile__("sd %0, 0(%1)" :: "r"(val), "r"(addr));
}

/* VirtIO MMIO 寄存器偏移 (QEMU virt 平台) */
#define VIRTIO_MMIO_MAGIC           0x000
#define VIRTIO_MMIO_VERSION         0x004
#define VIRTIO_MMIO_DEVICE_ID       0x008
#define VIRTIO_MMIO_VENDOR_ID       0x00c
#define VIRTIO_MMIO_DEVICE_FEATURES 0x010
#define VIRTIO_MMIO_DRIVER_FEATURES 0x020
#define VIRTIO_MMIO_QUEUE_SEL       0x030
#define VIRTIO_MMIO_QUEUE_NUM_MAX   0x034
#define VIRTIO_MMIO_QUEUE_NUM       0x038
#define VIRTIO_MMIO_QUEUE_READY     0x044
#define VIRTIO_MMIO_QUEUE_NOTIFY    0x050
#define VIRTIO_MMIO_INTERRUPT_STATUS 0x060
#define VIRTIO_MMIO_INTERRUPT_ACK   0x064
#define VIRTIO_MMIO_STATUS          0x070
#define VIRTIO_MMIO_QUEUE_DESC_LOW  0x080
#define VIRTIO_MMIO_QUEUE_DESC_HIGH 0x084
#define VIRTIO_MMIO_QUEUE_AVAIL_LOW 0x090
#define VIRTIO_MMIO_QUEUE_AVAIL_HIGH 0x094
#define VIRTIO_MMIO_QUEUE_USED_LOW  0x0a0
#define VIRTIO_MMIO_QUEUE_USED_HIGH 0x0a4

/* QEMU virt VirtIO MMIO 基地址 */
#define VIRTIO_MMIO_BASE    0x10001000
#define VIRTIO_MMIO_SIZE    0x1000
#define VIRTIO_MMIO_COUNT   8   /* 8 个 VirtIO 设备槽 */

/* 获取第 n 个 VirtIO 设备地址 */
#define VIRTIO_MMIO_ADDR(n) (VIRTIO_MMIO_BASE + (n) * VIRTIO_MMIO_SIZE)

#endif /* _VIRTIO_RISCV_H */
```

**阶段 3 检查点 / Phase 3 Checkpoint**:
```bash
# 编译内核
./build.sh -m evbriscv64 kernel

# QEMU 测试启动
qemu-system-riscv64 -M virt -m 256M -nographic \
    -kernel obj.evbriscv64/minix/kernel/kernel \
    -bios default

# 预期输出: "MINIX booting on RISC-V 64..."
```

---

## 阶段 4: VirtIO 驱动移植 / Phase 4: VirtIO Driver Porting

### 4.1 virtio-blk 移植步骤 / virtio-blk Porting Steps

**Status / 状态**: Partial  
**Evidence / 证据**: `minix/drivers/storage/virtio_blk` and `virtio_blk_mmio` exist; RISC-V-specific wiring not verified

**源文件 / Source**: `minix/drivers/storage/virtio_blk/`

| 步骤 | 操作 | 预计修改 |
|------|------|----------|
| 1 | 复制 x86 virtio_blk 驱动 | - |
| 2 | 添加 `#include "virtio_riscv.h"` | 1 行 |
| 3 | 替换 `inb/outb` 为 `virtio_read/write` | ~50 处 |
| 4 | 添加 `virtio_fence()` 调用 | ~10 处 |
| 5 | 更新 IRQ 号为 PLIC IRQ | ~5 处 |
| 6 | 修改 MMIO 基地址 | ~3 处 |

**验证命令 / Verify**:
```bash
# 创建测试磁盘镜像
dd if=/dev/zero of=test.img bs=1M count=64

# QEMU 测试
qemu-system-riscv64 -M virt -m 512M -nographic \
    -kernel minix.elf \
    -drive file=test.img,if=virtio,format=raw
```

### 4.2 virtio-net 移植步骤 / virtio-net Porting Steps

**Status / 状态**: Partial  
**Evidence / 证据**: `minix/drivers/net/virtio_net` exists; RISC-V MMIO/PLIC integration not verified

与 virtio-blk 类似，额外注意:
- 中断处理中的 DMA 缓冲区同步
- `virtio_fence()` 在发送/接收路径

### 4.3 virtio-gpu 移植步骤 / virtio-gpu Porting Steps

**Status / 状态**: Planned  
**Evidence / 证据**: no `minix/drivers/video/virtio_gpu` directory

额外注意:
- 帧缓冲区映射
- 2D 渲染命令序列化

### 4.4 virtio-input 移植步骤 / virtio-input Porting Steps

**Status / 状态**: Planned  
**Evidence / 证据**: no `minix/drivers/hid/virtio_input` directory

最简单的驱动，主要是事件队列处理。

**阶段 4 检查点 / Phase 4 Checkpoint**:
```bash
# 完整启动测试
qemu-system-riscv64 -M virt -m 2G -smp 1 \
    -kernel minix.elf \
    -drive file=minix.img,if=virtio,format=raw \
    -netdev user,id=n0 -device virtio-net-device,netdev=n0 \
    -serial mon:stdio

# 预期: 启动到 shell，网络 ping 成功
```

---

## 阶段 5: SMP 多核支持 / Phase 5: SMP Support

### 5.1 SMP 核心代码 / SMP Core Code

**Status / 状态**: Planned  
**Evidence / 证据**: no `minix/kernel/arch/riscv64/smp.c` in tree

**文件 / File**: `minix/kernel/arch/riscv64/smp.c` (~200 行新代码)

```c
/*
 * RISC-V SMP 支持
 */

#include "kernel/kernel.h"
#include "arch_proto.h"
#include "archconst.h"

/* SBI HSM 扩展用于启动从核 */
#define SBI_EXT_HSM             0x48534D
#define SBI_HSM_HART_START      0
#define SBI_HSM_HART_STOP       1
#define SBI_HSM_HART_STATUS     2

/* 外部变量 (head.S 中定义) */
extern volatile uint64_t _smp_boot_flag;

/* 每核数据 */
struct cpu_info cpu_info[CONFIG_MAX_CPUS];
int num_cpus = 1;

static struct sbiret sbi_hsm_hart_start(unsigned long hartid,
    unsigned long start_addr, unsigned long opaque)
{
    return sbi_ecall(SBI_EXT_HSM, SBI_HSM_HART_START,
                     hartid, start_addr, opaque, 0, 0, 0);
}

void smp_init(void)
{
    int i;
    struct sbiret ret;

    /* 主核已在运行 */
    cpu_info[0].cpu_id = 0;
    cpu_info[0].running = 1;

    /* 启动从核 */
    for (i = 1; i < CONFIG_MAX_CPUS; i++) {
        /* 获取从核入口地址 */
        extern void secondary_hart_entry(void);

        ret = sbi_hsm_hart_start(i, (unsigned long)secondary_hart_entry, 0);
        if (ret.error == 0) {
            num_cpus++;
        }
    }

    /* 设置启动标志，让从核继续 */
    __asm__ __volatile__("fence w, w" ::: "memory");
    _smp_boot_flag = 1;
    __asm__ __volatile__("fence w, w" ::: "memory");

    /* 等待所有核启动 */
    while (1) {
        int running = 0;
        for (i = 0; i < num_cpus; i++) {
            if (cpu_info[i].running)
                running++;
        }
        if (running == num_cpus)
            break;
    }

    printf("SMP: %d CPUs online\n", num_cpus);
}

/* 从核入口 */
void smp_ap_entry(int hart_id)
{
    /* 初始化每核数据 */
    cpu_info[hart_id].cpu_id = hart_id;
    cpu_info[hart_id].running = 1;

    /* 设置 PLIC 阈值 */
    plic_threshold_set(hart_id, 0);

    /* 启用中断 */
    csr_set_sie(SIE_SEIE | SIE_STIE | SIE_SSIE);
    csr_set_sstatus(SSTATUS_SIE);

    /* 进入调度循环 */
    idle_loop();
}

/* 发送 IPI */
void smp_send_ipi(int cpu)
{
    unsigned long hart_mask = 1UL << cpu;
    sbi_send_ipi(hart_mask);
}

/* 广播 IPI */
void smp_broadcast_ipi(void)
{
    unsigned long hart_mask = (1UL << num_cpus) - 1;
    sbi_send_ipi(hart_mask);
}

/* IPI 处理 */
void smp_ipi_handler(struct trapframe *tf)
{
    /* 清除软件中断 */
    csr_clear_sip(SIP_SSIP);

    /* 检查是否需要重新调度 */
    if (need_resched())
        schedule();
}
```

### 5.2 SMP 调度器适配 / SMP Scheduler Integration

**Status / 状态**: Planned  
**Evidence / 证据**: no riscv64-specific scheduler changes observed; user space not stable (`RISC64-STATUS.md`)

**文件 / File**: `minix/servers/sched/` (修改现有代码)

主要添加:
- 负载均衡
- CPU 亲和性支持
- 每核运行队列

**阶段 5 检查点 / Phase 5 Checkpoint**:
```bash
# 16 核启动测试
qemu-system-riscv64 -M virt -m 2G -smp 16 \
    -kernel minix.elf \
    -drive file=minix.img,if=virtio,format=raw \
    -serial mon:stdio

# 验证多核
cat /proc/cpuinfo | grep processor | wc -l
# 预期输出: 16
```

---

## 阶段 6: 完整系统集成 / Phase 6: Full System Integration

### 6.1 X11 测试 / X11 Tests

**Status / 状态**: Planned  
**Evidence / 证据**: user space not stable (see `RISC64-STATUS.md`)

```bash
# 带 GPU 启动
qemu-system-riscv64 -M virt -m 2G -smp 16 \
    -kernel minix.elf \
    -drive file=minix.img,if=virtio,format=raw \
    -device virtio-gpu-device \
    -device virtio-keyboard-device \
    -device virtio-mouse-device

# 在系统中
startx
```

### 6.2 完整测试清单 / Full Test Checklist

**Status / 状态**: Planned  
**Evidence / 证据**: user space not stable; full checklist not yet executable (see `RISC64-STATUS.md`)

| 测试项 | 命令 | 预期结果 |
|--------|------|----------|
| 启动 | boot | Shell 提示符 |
| 磁盘 | ls / | 文件列表 |
| 网络 | ping 10.0.2.2 | 响应 |
| 多核 | top | 16 CPU 显示 |
| X11 | startx | 桌面显示 |
| 重启 | reboot | 系统重启 |

---

## 文件清单总结 / File Summary

### 需要新建的文件 (~30 个) / Files to Add (~30)

**Status / 状态**: Partial  
**Evidence / 证据**: most arch files exist; missing `minix/kernel/arch/riscv64/smp.c`,
`minix/drivers/virtio/virtio_riscv.h`, `minix/drivers/video/virtio_gpu/`,
`minix/drivers/hid/virtio_input/`

```
sys/arch/riscv/include/
├── asm.h, atomic.h, cdefs.h, cpu.h
├── elf_machdep.h, endian.h, float.h, frame.h
├── int_types.h, limits.h, param.h, pmap.h
├── ptrace.h, reg.h, setjmp.h, signal.h
├── types.h, vmparam.h
└── Makefile, Makefile.common

sys/arch/evbriscv64/
├── include/Makefile
└── Makefile

minix/kernel/arch/riscv64/
├── Makefile.inc
├── include/ (archconst.h, arch_proto.h, hw_intr.h, ...)
├── head.S, mpx.S, klib.S
├── sbi.c, plic.c, smp.c
├── exception.c, hw_intr.c
├── arch_clock.c, arch_system.c, arch_reset.c
├── memory.c, protect.c, pg_utils.c
├── phys_copy.S, phys_memset.S
├── kernel.lds, procoffsets.cf, sconst.h
└── bsp/virt/

minix/drivers/virtio/
└── virtio_riscv.h
```

**备注 / Note**:
- 已存在：`sys/arch/riscv/include/`, `sys/arch/evbriscv64/`, `minix/kernel/arch/riscv64/` 的大部分文件。  
  Present: most files under `sys/arch/riscv/include/`, `sys/arch/evbriscv64/`, `minix/kernel/arch/riscv64/`.
- 缺失：`minix/kernel/arch/riscv64/smp.c`、`minix/drivers/virtio/virtio_riscv.h`、`virtio_gpu/input` 目录。  
  Missing: `minix/kernel/arch/riscv64/smp.c`, `minix/drivers/virtio/virtio_riscv.h`,
  `virtio_gpu/input` directories.

### 需要修改的文件 (~10 个) / Files to Update (~10)

**Status / 状态**: Partial  
**Evidence / 证据**: `build.sh`, `share/mk/bsd.own.mk`, `sys/arch/Makefile` already updated;
virtio driver MMIO/IRQ wiring not verified

```
build.sh                    # 添加 evbriscv64 目标
share/mk/bsd.own.mk        # 添加 RISC-V 编译标志
sys/arch/Makefile          # 添加 RISC-V 分支
minix/drivers/storage/virtio_blk/  # MMIO 适配
minix/drivers/net/virtio_net/      # MMIO 适配
minix/drivers/video/virtio_gpu/    # MMIO 适配
minix/drivers/hid/virtio_input/    # MMIO 适配
```

---

## 快速开始脚本

```bash
#!/bin/bash
# setup_riscv64.sh - 创建 RISC-V 64 移植基础结构

set -e

# 1. 创建目录
mkdir -p sys/arch/riscv/include
mkdir -p sys/arch/evbriscv64/include
mkdir -p minix/kernel/arch/riscv64/{include,bsp/virt}
mkdir -p minix/include/arch/riscv64/include

# 2. 复制 ARM 模板
cp -r minix/kernel/arch/earm/include/* minix/kernel/arch/riscv64/include/
cp minix/kernel/arch/earm/Makefile.inc minix/kernel/arch/riscv64/

# 3. 复制头文件模板
cp -r sys/arch/arm/include/* sys/arch/riscv/include/

echo "RISC-V 64 目录结构已创建"
echo "下一步: 修改 build.sh 添加 evbriscv64 目标"
```

运行此脚本后，按照各阶段逐步实现具体代码。
