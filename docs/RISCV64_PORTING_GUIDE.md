# MINIX 3 RISC-V 64 Porting Guide / MINIX 3 RISC-V 64 位移植指南

本文件是面向工程实践的“移植过程文档化”手册，强调**读代码—写文档**的迭代节奏。
This document is a practical, code-driven porting guide that emphasizes an iterative
**read-code → write-doc** workflow.

**Document Info / 文档信息**
- Last updated / 最后更新: 2026-01-07
- Version / 版本: 1.1
- Scope / 范围: evbriscv64 on QEMU virt; kernel + VM + UART + build pipeline
- Update rule / 更新规则: append notes after code review; file issues in `issue.md`
- Change note / 变更说明: 2026-01-07 文档同步，未重新构建或测试。

---

## 0. Scope & Reading Coverage / 范围与阅读覆盖

**中文**
- 已阅读并纳入分析的核心代码区域（代表性文件）：
  - `minix/kernel/arch/riscv64/*`（启动、异常、中断、SBI、时钟、内存）
  - `minix/servers/vm/*`（页表管理、缺页处理、sys_vmctl 交互）
  - `minix/drivers/tty/ns16550/*`（UART 驱动）
  - `minix/lib/libsys/sys_vmctl.c` 与 `minix/include/minix/com.h`
  - 现有文档：`README-RISCV64.md`, `docs/RISCV64_PORT_PLAN.md`, `docs/RISCV64_KERNEL_BUILD_LOG.md`

**English**
- Code areas reviewed and included in this documentation (representative files):
  - `minix/kernel/arch/riscv64/*` (boot, exceptions, interrupts, SBI, clock, memory)
  - `minix/servers/vm/*` (page tables, page faults, sys_vmctl path)
  - `minix/drivers/tty/ns16550/*` (UART)
  - `minix/lib/libsys/sys_vmctl.c` and `minix/include/minix/com.h`
  - Existing docs: `README-RISCV64.md`, `docs/RISCV64_PORT_PLAN.md`, `docs/RISCV64_KERNEL_BUILD_LOG.md`

> 建议实践方式：读完一个模块（例如 arch/riscv64/exception.c），立即在对应章节补充“流程+约束+已知问题”。
> Suggested practice: after finishing a module (e.g., arch/riscv64/exception.c), immediately
> update the matching section with “flow + constraints + known issues”.

---

## 1. Porting Phases Overview / 移植阶段总览

**中文**
1) 工具链与构建系统：确保 evbriscv64 可构建并产出内核/用户态。
2) 内核最小启动：OpenSBI → head.S → 启用 Sv39 → kernel_main。
3) 中断与时钟：异常入口、PLIC、SBI timer、系统时钟回调。
4) VM 服务联动：pt_bind / sys_vmctl / 缺页处理链路打通。
5) 驱动与用户态：UART 最先稳定，然后引入 VirtIO。
6) 测试与回归：QEMU 启动、riscv64 测试脚本、日志回归。

**English**
1) Toolchain & build: make evbriscv64 buildable for kernel/userspace.
2) Minimal kernel boot: OpenSBI → head.S → Sv39 → kernel_main.
3) Interrupts & clock: trap entry, PLIC, SBI timer, clock handler integration.
4) VM integration: pt_bind / sys_vmctl / pagefault pipeline.
5) Drivers & userspace: stabilize UART first, then VirtIO.
6) Testing & regression: QEMU boot, riscv64 test scripts, log checks.

---

## 2. Build & Toolchain / 构建与工具链

**中文**
构建流程已集中在 `README-RISCV64.md`，以下是可用的最小命令集合（避免 LLVM、C++ 等尚未稳定组件）：

```bash
MKPCI=no HOST_CFLAGS="-O -fcommon" HAVE_GOLD=no HAVE_LLVM=no MKLLVM=no \
./build.sh -j$(nproc) -m evbriscv64 \
  -V AVAILABLE_COMPILER=gcc -V ACTIVE_CC=gcc -V ACTIVE_CPP=gcc -V ACTIVE_CXX=gcc -V ACTIVE_OBJC=gcc \
  -V RISCV_ARCH_FLAGS='-march=RV64IMAFD -mcmodel=medany' \
  -V NOGCCERROR=yes \
  -V MKPIC=no -V MKPICLIB=no -V MKPICINSTALL=no \
  -V MKCXX=no -V MKLIBSTDCXX=no -V MKATF=no \
  -V USE_PCI=no \
  -V CHECKFLIST_FLAGS='-m -e' \
  distribution
```

**English**
The minimal working build recipe is in `README-RISCV64.md`. The following is the
known-good baseline (skipping LLVM/C++):

```bash
MKPCI=no HOST_CFLAGS="-O -fcommon" HAVE_GOLD=no HAVE_LLVM=no MKLLVM=no \
./build.sh -j$(nproc) -m evbriscv64 \
  -V AVAILABLE_COMPILER=gcc -V ACTIVE_CC=gcc -V ACTIVE_CPP=gcc -V ACTIVE_CXX=gcc -V ACTIVE_OBJC=gcc \
  -V RISCV_ARCH_FLAGS='-march=RV64IMAFD -mcmodel=medany' \
  -V NOGCCERROR=yes \
  -V MKPIC=no -V MKPICLIB=no -V MKPICINSTALL=no \
  -V MKCXX=no -V MKLIBSTDCXX=no -V MKATF=no \
  -V USE_PCI=no \
  -V CHECKFLIST_FLAGS='-m -e' \
  distribution
```

---

## 3. Boot Flow / 启动流程

**中文（读取文件：`minix/kernel/arch/riscv64/head.S`, `pg_utils.c`, `arch_system.c`）**
1) OpenSBI 进入 `_start`：关闭中断、保存 hart id、清 BSS、设置初始栈。
2) `pg_early_init` 构造 Sv39 初始页表：
   - 0x80000000 1GB identity map
   - 高地址内核映射（0xFFFFFFFF80000000）
3) 写 SATP 并 `sfence.vma`，跳转到高地址 `kernel_main`。
4) `arch_system_init` 初始化异常、PLIC、中断与时钟。

**English (files read: `minix/kernel/arch/riscv64/head.S`, `pg_utils.c`, `arch_system.c`)**
1) OpenSBI enters `_start`: interrupts off, hart id saved, BSS cleared, stack set.
2) `pg_early_init` builds initial Sv39 tables:
   - 1GB identity map at 0x80000000
   - high kernel mapping at 0xFFFFFFFF80000000
3) Write SATP + `sfence.vma`, jump to high `kernel_main`.
4) `arch_system_init` brings up traps, PLIC, interrupts, and clock.

---

## 4. Memory Management (Sv39) / 内存管理（Sv39）

**中文（读取文件：`pg_utils.c`, `memory.c`, `servers/vm/pagetable.c`）**
- 内核早期页表由 `pg_early_init` 构建，后续映射由 VM 服务接管。
- VM 通过 `pt_bind` + `sys_vmctl_set_addrspace` 把新页表根交给内核。
- RISC-V 三层页表：L2 (VPN2) → L1 (VPN1) → L0 (VPN0)。

**English (files read: `pg_utils.c`, `memory.c`, `servers/vm/pagetable.c`)**
- Early kernel mappings are created in `pg_early_init`, later managed by VM.
- VM uses `pt_bind` + `sys_vmctl_set_addrspace` to update the kernel SATP root.
- Sv39 uses 3 levels: L2 (VPN2) → L1 (VPN1) → L0 (VPN0).

**关键建议 / Key recommendations**
- 修正 PTROOT 传递为 64-bit，避免 SATP 根地址截断。
- 使用 `ptroot_v` 或稳定内核映射，避免物理地址被当作虚拟地址解引用。

---

## 5. Interrupts & Timer / 中断与时钟

**中文（读取文件：`exception.c`, `hw_intr.c`, `plic.c`, `arch_clock.c`, `sbi.c`）**
- 异常入口在 `trap_entry`，`exception_handler` 负责区分中断/异常。
- 外部中断：PLIC → `hw_intr_handler` → `irq_handle`。
- 定时器：`arch_clock_handler` 设置下一次 SBI timer。当前缺少与系统时钟的对接（见 issue.md）。
- IPI 使用 SBI 0.1 legacy 接口，需注意地址与模式差异。

**English (files read: `exception.c`, `hw_intr.c`, `plic.c`, `arch_clock.c`, `sbi.c`)**
- Trap entry is `trap_entry`, with `exception_handler` splitting IRQ vs exception.
- External IRQs: PLIC → `hw_intr_handler` → `irq_handle`.
- Timer: `arch_clock_handler` re-arms SBI timer. Kernel clock integration is still missing.
- IPIs use SBI v0.1 legacy calls and require correct addressing.

---

## 6. Syscall & IPC Path / 系统调用与 IPC 路径

**中文**
- Syscall 由 `exception.c:handle_syscall` 分发，`a7` 为系统调用编号。
- `IPCVEC_INTR` → `do_ipc`，`KERVEC_INTR` → `kernel_call`。
- `arch_set_secondary_ipc_return` 在 a1 返回额外值。

**English**
- `exception.c:handle_syscall` dispatches syscalls using `a7`.
- `IPCVEC_INTR` → `do_ipc`, `KERVEC_INTR` → `kernel_call`.
- `arch_set_secondary_ipc_return` places the secondary result in `a1`.

---

## 7. VM Server Integration / VM 服务集成

**中文（读取文件：`servers/vm/pagetable.c`, `servers/vm/pagefaults.c`）**
- VM 负责维护进程页表和内核映射，关键路径在 `pt_bind`。
- 缺页处理由内核发送 `VM_PAGEFAULT` 消息，VM 处理后用 `VMCTL_CLEAR_PAGEFAULT` 解阻塞。
- 注意 TLB 刷新与 leaf→non-leaf 转换的正确性。

**English (files read: `servers/vm/pagetable.c`, `servers/vm/pagefaults.c`)**
- VM owns page table construction; `pt_bind` is the critical handoff to the kernel.
- Kernel sends `VM_PAGEFAULT`, VM resolves and clears via `VMCTL_CLEAR_PAGEFAULT`.
- TLB flushes must be correct for leaf→non-leaf transitions.

---

## 8. Drivers (UART) / 驱动（UART）

**中文（读取文件：`drivers/tty/ns16550/ns16550.c`）**
- QEMU virt UART: MMIO 0x10000000，IRQ 10。
- 当前驱动支持基本读写与中断，但阻塞式 read 仍需补齐“延迟回复”逻辑。

**English (file read: `drivers/tty/ns16550/ns16550.c`)**
- QEMU virt UART at 0x10000000, IRQ 10.
- Basic RX/TX works; blocking read still needs deferred reply handling.

---

## 9. Testing & Debug / 测试与调试

**中文**
- RISC-V 测试：`./minix/tests/riscv64/run_tests.sh all`
- QEMU 启动：`./minix/scripts/qemu-riscv64.sh -k minix/kernel/obj/kernel -B obj/destdir.evbriscv64`
- GDB：`./minix/scripts/gdb-riscv64.sh /path/to/kernel`
- 关键日志参考：`README-RISCV64.md` 与 `docs/RISCV64_KERNEL_BUILD_LOG.md`

**English**
- Tests: `./minix/tests/riscv64/run_tests.sh all`
- QEMU boot: `./minix/scripts/qemu-riscv64.sh -k minix/kernel/obj/kernel -B obj/destdir.evbriscv64`
- GDB: `./minix/scripts/gdb-riscv64.sh /path/to/kernel`
- Logs: `README-RISCV64.md`, `docs/RISCV64_KERNEL_BUILD_LOG.md`

---

## 10. Known Issues / 已知问题

**中文**
详见 `issue.md` 与 `RISC64-STATUS.md`，其中包含 PTROOT 传参截断、TLB 刷新缺失、
UART 延迟回复缺口、SBI legacy IPI/fence 传参问题等关键问题与状态摘要。

**English**
See `issue.md` and `RISC64-STATUS.md` for detailed issues and status snapshots, including
PTROOT truncation, missing TLB flushes, UART deferred reply gaps, SBI legacy IPI/fence issues,
and others.

---

## 11. Documentation Workflow / 文档更新流程

**中文**
推荐固定节奏：
1) 读完一个模块（如 exception.c 或 pagetable.c）。
2) 在对应章节新增：
   - 流程图或文字流程（关键函数与数据结构）
   - 约束与假设（地址范围、寄存器语义）
   - 已知问题与测试方法

**English**
Recommended cadence:
1) Finish reading a module (e.g., exception.c or pagetable.c).
2) Update the matching section with:
   - flow description (key functions/data structures)
   - constraints & assumptions (address ranges, CSR semantics)
   - known issues and test methods

---

## 12. Quick Checklist for Changes / 变更检查清单

**中文**
- sys_vmctl PTROOT 传参是否全 64-bit？
- leaf→non-leaf 转换后是否刷新 TLB？
- UART 阻塞读是否有延迟回复路径？
- SBI legacy IPI/fence 是否改为 v0.2+ 或传递 PA？
- IPC 返回寄存器是否一致（a0/a1）？

**English**
- Are sys_vmctl PTROOT parameters 64-bit end-to-end?
- Is TLB flushed after leaf→non-leaf splits?
- Does UART blocking read have a deferred reply path?
- Are SBI legacy IPI/fence calls on v0.2+ or passing PA?
- Are IPC return registers consistent (a0/a1)?
