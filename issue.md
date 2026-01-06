# MINIX RISC-V Port Issues / MINIX RISC-V 移植问题清单

**Date / 日期**: 2026-01-07  
**Version / 版本**: 1.2  
**Scope / 范围**: RISC-V 64-bit port, evidence includes file/line references.

本文件记录 RISC-V 64 位移植的具体问题与证据（含文件/行号），并给出修复建议。  
This file records concrete issues in the RISC-V 64-bit port with evidence and suggested fixes.

**复核说明**：2026-01-07 补充 2026-01-06 01:00 前代码变更；未重新构建或测试。  
**Review note**: 2026-01-07 update after reviewing pre-2026-01-06 01:00 changes; no new build/test run.

## Active Investigation / 当前主问题跟踪

### A1) Userland services stuck in user pagefault loop during boot / 用户态服务启动期反复缺页
- Evidence / 证据:
  - VFS: `sef_receive_status` at `minix/lib/libsys/sef.c:150`, `mthread_trampoline` at `minix/lib/libmthread/allocate.c:531`,
    `malloc_bytes` at `lib/libc/stdlib/malloc.c:875`, `memset` at `common/lib/libc/string/memset.c:156`
  - MFS: `extend_pgdir` at `lib/libc/stdlib/malloc.c:534`, `imalloc` at `lib/libc/stdlib/malloc.c:934`,
    `kdoprnt` at `sys/lib/libsa/subr_prf.c:189`
  - DS: `_regcomp` at `lib/libc/regex/regcomp.c:333`, `extend_pgdir` at `lib/libc/stdlib/malloc.c:534`,
    `imalloc` at `lib/libc/stdlib/malloc.c:934`, `kdoprnt` at `sys/lib/libsa/subr_prf.c:189`
- Impact / 影响:
  - Boot does not reach stable userland; init dies and services spin on faults. / 启动无法进入稳定用户态；init 退出且服务反复缺页。
- Hypothesis / 假设:
  - RV64 heap growth or page table extension path (`extend_pgdir` / `malloc_pages`) maps invalid VA. / RV64 堆扩展或页表扩展路径可能映射了非法 VA。
  - May be related to address-space handoff or missing TLB flush after leaf splits (Major #4). / 可能与地址空间切换或叶子拆分页后的 TLB 刷新缺失（Major #4）相关。
- Next steps / 下一步:
  - Capture faulting `addr` with matching `pc` to confirm heap boundaries. / 采集 fault addr 与 pc 对应关系以确认堆边界。
  - Audit `malloc.c` + VM mappings on RV64; verify `brk`/`sbrk` flow and VM map permissions. / 审核 RV64 的 `malloc.c` 与 VM 映射；核对 `brk`/`sbrk` 路径与权限。

### A2) RV64 process support exists in loader/CPU mode but is not stable end-to-end / RV64 进程支持具备基础但端到端不稳定
- Evidence / 证据:
  - U-mode is configured by clearing `SSTATUS_SPP` in `prot_init`. / 内核已配置 U-mode 入口。  
    Evidence: `minix/kernel/arch/riscv64/protect.c:48`
  - VM boot loader expects ELF64/EM_RISCV for the VM image. / VM 引导加载器按 ELF64/EM_RISCV 校验。  
    Evidence: `minix/kernel/arch/riscv64/protect.c:138`
  - Exec loader is built as 64-bit on `__riscv64__`, and ELF target class is 64 for RISC-V. / exec 加载器按 RV64 构建并期望 ELF64。  
    Evidence: `minix/lib/libexec/exec_elf.c:4`, `sys/arch/riscv/include/elf_machdep.h:24`
  - Current status explicitly says userland is not yet stable. / 现状明确用户态仍不稳定。  
    Evidence: `README-RISCV64.md:27`
- Impact / 影响:
  - Kernel has RV64 U-mode + ELF64 exec plumbing, but user processes are not reliably runnable yet. / 内核具备基础通路，但 RV64 进程尚不可稳定运行。
- Suggested fix / 修复建议:
  - Resolve A1 and top Major issues (VM/PT/TLB/IPI), then validate exec with a minimal ELF64 user binary + ld.so. / 先修复 A1 与主要问题（VM/PT/TLB/IPI），再用最小 ELF64 用户程序验证 exec/ld.so。
  - Validation checklist (doc-only): / 验证清单（文档）:
    1) Confirm target ELF64/EM_RISCV via `readelf -h` on the test binary. / 通过 `readelf -h` 确认 ELF64/EM_RISCV。
    2) Prefer a minimal static executable if available; otherwise verify `PT_INTERP` points to `/libexec/ld.elf_so`. / 尽量使用静态可执行文件；否则确认 `PT_INTERP` 指向 `/libexec/ld.elf_so`。  
       Evidence: `minix/drivers/storage/ramdisk/proto.common.dynamic:2`, `minix/servers/vfs/exec.c:282`
    3) Ensure dynamic loader is mapped below stack as per VFS logic. / 确认动态加载器按 VFS 逻辑映射到栈下。  
       Evidence: `minix/servers/vfs/exec.c:306`
    4) Keep the test binary minimal (single `main`, no threads) to isolate VM/exec issues. / 测试程序保持最小化以隔离 VM/exec 问题。

## Critical / 严重

- None confirmed in current workspace; former Critical #1 moved to Fixed. / 当前工作区未确认有严重问题，原 Critical #1 已移至 Fixed。

## Major / 重要

### 4) Leaf → non-leaf page table split lacks TLB flush / 叶子页拆分后未刷新 TLB
- Evidence / 证据:
  - `minix/servers/vm/pagetable.c:50-147` (`pt_l0alloc`) and `150-193` (`pt_l1alloc`) convert
    leaf mappings to non-leaf PTEs without a TLB invalidation.
- Impact / 影响:
  - Stale large-page TLB entries may persist, causing wrong mappings. / 旧的大页 TLB 项可能继续生效，导致映射错误。
- Suggested fix / 修复建议:
  - Issue `VMCTL_FLUSHTLB` or targeted `sfence.vma` after the split. / 拆分后执行 `VMCTL_FLUSHTLB` 或定向 `sfence.vma`。

### 5) SBI legacy IPI/fence calls pass virtual addresses / SBI v0.1 旧接口传递 VA
- Evidence / 证据:
  - `minix/kernel/arch/riscv64/sbi.c:131-151` passes `&hart_mask` for legacy SBI v0.1 calls
- Impact / 影响:
  - SBI v0.1 expects PA; VA breaks once paging is enabled. / SBI v0.1 期望 PA，启用分页后传 VA 会失败。
  - IPI and remote fence may silently fail. / IPI 与远程指令缓存刷新可能失效。
- Suggested fix / 修复建议:
  - Use SBI v0.2+ extensions or translate to PA. / 切换到 SBI v0.2+ 扩展或传递物理地址。

### 6) UART driver blocks reads without replying / UART 阻塞读无延迟回复
- Evidence / 证据:
  - `minix/drivers/tty/ns16550/ns16550.c:171-195` returns `EDONTREPLY` when no data
  - `minix/drivers/tty/ns16550/ns16550.c:256-298` interrupt handler does not issue deferred replies
- Impact / 影响:
  - Blocking reads hang indefinitely; userland console read stalls. / 阻塞读无限期挂起，用户态无法读取。
- Suggested fix / 修复建议:
  - Track pending reads and reply on RX interrupts. / 保存挂起请求并在 RX 中断时回复。

### 7) vm_map_range marks pages executable unconditionally / vm_map_range 无条件设置可执行
- Evidence / 证据:
  - `minix/kernel/arch/riscv64/memory.c:198-205` sets `PTE_X` in all mappings
- Impact / 影响:
  - W^X is violated; data pages become executable. / 违反 W^X，数据页被标记为可执行。
- Suggested fix / 修复建议:
  - Set `PTE_X` only for executable mappings (use VMMF flags). / 仅在可执行映射时设置 `PTE_X`。

### 8) Breakpoint exception always advances PC by 4 bytes / 断点异常固定前移 4 字节
- Evidence / 证据:
  - `minix/kernel/arch/riscv64/exception.c:162-168`
- Impact / 影响:
  - Compressed `ebreak` is 2 bytes; advancing by 4 skips the next instruction. / 压缩 `ebreak` 为 2 字节，前移 4 字节会跳过下一条指令。
- Suggested fix / 修复建议:
  - Decode instruction length before advancing `sepc`. / 根据指令长度推进 `sepc`。

### 9) Software interrupts (IPI) not enabled in SIE / SIE 中未开启 SSIE
- Evidence / 证据:
  - `minix/kernel/arch/riscv64/exception.c:60-61` enables only STIE/SEIE
- Impact / 影响:
  - SMP IPIs will not be delivered even if SBI works. / 即使 SBI 正常，SMP IPI 也不会送达。
- Suggested fix / 修复建议:
  - Enable `SIE_SSIE` when SMP is configured. / SMP 场景下开启 `SIE_SSIE`。

### 15) RISC-V SMP core missing (arch_smp + smp.c not implemented) / RISC-V SMP 核心缺失
- Evidence / 证据:
  - `minix/kernel/arch/riscv64/include/arch_proto.h:143-151` declares SMP entrypoints with no riscv64 definitions
  - `minix/kernel/arch/riscv64/head.S:149-152` calls `smp_ap_entry` when `CONFIG_SMP` is enabled
  - `docs/RISCV64_PORT_PLAN.md:1618-1621` and `README-RISCV64.md:213-218` state SMP core is planned/skip-marked
- Impact / 影响:
  - CONFIG_SMP builds will not link or will only boot BSP; AP bring-up and IPI paths are missing. / 打开 CONFIG_SMP 时无法链接或仅能启动 BSP；从核启动与 IPI 路径缺失。
- Suggested fix / 修复建议:
  - Add `minix/kernel/arch/riscv64/smp.c` with `smp_init`, `smp_ap_entry`, `smp_send_ipi`, `smp_broadcast_ipi`, `smp_ipi_handler`, `arch_send_smp_schedule_ipi`, `arch_smp_halt_cpu`. / 增加 riscv64 `smp.c` 并实现上述入口。
  - Add `minix/kernel/arch/riscv64/include/arch_smp.h` with SMP `cpuid` definition, plus per-CPU PLIC/timer init and `SIE_SSIE` enablement. / 增加 riscv64 `arch_smp.h`，定义 SMP `cpuid`，并接入每 CPU 的 PLIC/定时器初始化及 `SIE_SSIE` 使能。

## Moderate / 中等

### 11) Minimal kernel build is not RISC-V-ready / minimal_kernel 未支持 RISC-V
- Evidence / 证据:
  - `minimal_kernel/include/minix/const.h:83-87` rejects non-i386/arm
  - Build fails due to incomplete header/machine wiring (see build logs)
- Impact / 影响:
  - `minimal_kernel/` cannot be used for RISC-V minimal boot tests. / `minimal_kernel/` 无法用于 RISC-V 最小化测试。
- Suggested fix / 修复建议:
  - Add RISC-V support in minimal kernel headers or exclude it from RISC-V builds. / 在 minimal kernel 增加 RISC-V 支持或在 RISC-V 构建中排除。

### 13) phys_copy fault handler not registered / phys_copy 缺少故障处理注册
- Evidence / 证据:
  - `minix/kernel/arch/riscv64/phys_copy.S:71-74` leaves a TODO to register the fault handler
- Impact / 影响:
  - Faulting physical copies can trap instead of returning an error. / 物理复制缺页可能直接陷入异常而非返回错误。
- Suggested fix / 修复建议:
  - Hook into the arch fault-handling mechanism (as in other architectures) for safe phys_copy. / 接入架构故障处理机制以安全执行 phys_copy。

### 14) Device tree parsing is minimal (single region, no reserved areas) / 设备树解析较简化（单一内存段、无保留区）
- Evidence / 证据:
  - `minix/kernel/arch/riscv64/bsp/virt/bsp_init.c:80-201` parses memory/CPUs/timebase but only uses a single `memory` node and ignores reserved areas
  - `minix/kernel/arch/riscv64/memory.c:69-83` still has a TODO for richer DT-driven memory layout
- Impact / 影响:
  - Multi-region or reserved-memory layouts may be ignored, leading to overlaps or wrong sizing. / 多段或保留内存布局可能被忽略，导致覆盖或尺寸错误。
- Suggested fix / 修复建议:
  - Extend FDT parsing to handle reserved regions and multiple memory ranges, then plumb into `add_memmap`. / 扩展 FDT 解析以处理保留区与多段内存，并接入 `add_memmap`。

## Technical Debt / 技术债务

### TD1) Static RISC-V links require per-binary __global_pointer$ workaround / 静态链接需要每个二进制打补丁
- Evidence / 证据:
  - Link failure without workaround: `crt0.o: undefined reference to '__global_pointer$'`
  - `lib/csu/arch/riscv/crt0.S` initializes `gp` for dynamic start.
  - Per-binary stubs in `minix/servers/vfs/gp.c`, `minix/servers/rs/gp.c`,
    `minix/drivers/tty/tty/gp.c`, `minix/commands/minix-service/gp.c` (and others).
- Impact / 影响:
  - Workaround is widespread but still per-binary; new binaries can miss it. / 仍需逐个二进制打补丁，新增组件易遗漏。
- Suggested fix / 修复建议:
- Define `__global_pointer$` in crt0 or linker script globally, then remove per-binary gp.c/LDFLAGS. / 在 crt0 或链接脚本中全局定义 `__global_pointer$`，再移除各二进制 gp.c/LDFLAGS。

### TD2) SMP support is scaffolded but not fully tested / SMP 支持尚未完整验证
- Evidence / 证据:
  - `minix/kernel/arch/riscv64/README.md:156` lists SMP as not fully tested
- Impact / 影响:
  - SMP-related paths may regress silently without coverage. / SMP 路径可能在无覆盖情况下回归。
- Suggested fix / 修复建议:
  - Add SMP boot/interrupt/regression tests and document known-good configurations. / 增加 SMP 启动/中断/回归测试并记录可用配置。

### TD3) Debug registers (hardware breakpoints) not supported / 调试寄存器（硬件断点）未支持
- Evidence / 证据:
  - `minix/kernel/arch/riscv64/README.md:158` notes missing debug register support
- Impact / 影响:
  - Kernel/user debugging via hardware breakpoints is unavailable. / 无法使用硬件断点调试内核/用户态。
- Suggested fix / 修复建议:
  - Implement RISC-V debug CSR handling and integrate with exception flow. / 实现 RISC-V 调试 CSR 支持并接入异常流程。

## Enhancement / 增强提案

### E2) LiteOS-M + LiteOS-A Emulation (QEMU-based) / LiteOS-M 与 LiteOS-A 全系统仿真（QEMU）
- Tags / 标签: `enhancement`, `emulation`, `qemu`, `epic`
- Priority / 优先级: P0
- Owner / 负责人: @yangdongstation
- Target / 目标版本: Emulation platform (host OS) / 仿真平台（宿主系统）
- Summary / 简述:
  - Build a host-based full emulation stack using QEMU + HAL + kernel logic + API mapping for LiteOS-M and LiteOS-A. / 采用 QEMU + HAL + 内核功能层 + API 映射构建全系统仿真。
- Feasibility / 可行性评估:
  - Upstream LiteOS-M dynlink expects ELF `ET_DYN`; RISC-V dynlink is ELF32 and arch tree is RV32-only. / 上游 LiteOS-M 动态加载为 ELF32 且 RISC-V 仅 RV32。  
    Evidence: `https://gitee.com/openharmony/kernel_liteos_m/blob/master/components/dynlink/los_dynlink.c#L103-L112`, `https://gitee.com/openharmony/kernel_liteos_m/blob/master/components/dynlink/los_dynlink.h#L44-L64`, `https://gitee.com/openharmony/kernel_liteos_m/blob/master/README.md#L23-L40`
  - QEMU riscv32_virt uses `gcc_riscv32` and outputs `OHOS_Image`, making RV32 the practical starting point. / QEMU riscv32_virt 工具链与产物指向 RV32 起步。  
    Evidence: `https://gitee.com/openharmony/device_qemu/blob/master/riscv32_virt/README_zh.md#L15-L18`, `https://gitee.com/openharmony/device_qemu/blob/master/riscv32_virt/README_zh.md#L41-L46`
- Architecture doc / 架构文档:
  - `docs/liteos-emulation-architecture.md`

## Fixed in Current Working Tree / 已在当前工作区修复

- `minimal_kernel/proto.h:175` uses `reg_t` for `arch_set_secondary_ipc_return` to avoid RV64 truncation
  (matches `minix/kernel/proto.h` and arch implementations).  
  `minimal_kernel/proto.h:175` 已改为 `reg_t`，避免 RV64 截断（与 `minix/kernel/proto.h` 及架构实现一致）。
- `minix/include/minix/com.h:776` uses a 64-bit VPF_ADDR field; VM pagefault address transport is now 64-bit.  
  `minix/include/minix/com.h:776` 使用 64 位 VPF_ADDR，缺页地址传递已改为 64-bit。
- `minix/lib/libc/arch/riscv64/sys/_ipc.S:101` argument order for `senda` now matches kernel expectations.  
  `minix/lib/libc/arch/riscv64/sys/_ipc.S:101` 修正 `senda` 参数顺序以匹配内核。
- `minix/lib/libc/arch/riscv64/sys/ucontext.S:7` uses generated offsets and sets `MCF_MAGIC` for RV64 ucontext.  
  `minix/lib/libc/arch/riscv64/sys/ucontext.S:7` 使用偏移头与 `MCF_MAGIC`，统一 RV64 ucontext 约定。
- Former Critical #2: SATP root VA pointer is now passed via `SVMCTL_PTROOT_V` and used by the kernel (`minix/lib/libsys/sys_vmctl.c:30-40`,
  `minix/servers/vm/pagetable.c:2199-2208`, `minix/kernel/arch/riscv64/arch_do_vmctl.c:75-90`,
  `minix/kernel/arch/riscv64/memory.c:60-63`).  
  SATP 根地址 VA 指针已通过 `SVMCTL_PTROOT_V` 传递并被内核使用。
- Former Major #3: Timer interrupts now drive the kernel clock path (`minix/kernel/arch/riscv64/exception.c:110-112`,
  `minix/kernel/arch/riscv64/arch_clock.c:82-85`).  
  时钟中断已接入内核时钟路径。
- Former Critical #1: VMCTL transport is 64-bit on riscv64 (`minix/include/minix/com.h:370-395`,
  `minix/lib/libsys/sys_vmctl.c:3-52`, `minix/kernel/arch/riscv64/arch_do_vmctl.c:62-90`).  
  VMCTL 在 riscv64 上已使用 64-bit 字段传递 PTROOT。
- Former Major #12: FPU save/restore is implemented (`minix/kernel/arch/riscv64/klib.S:90-189`).  
  FPU 保存/恢复已实现（f0-f31 + fcsr）。
- Former Moderate #10: pagefault message is stack-local (`minix/kernel/arch/riscv64/exception.c:232-290`).  
  缺页消息已改为栈上局部变量。

## Vision / 愿景: pkgsrc on MINIX RV64

Goal / 目标: enable pkgsrc source builds on MINIX RV64 (not NetBSD binaries).  
在 MINIX RV64 上可源码构建 pkgsrc（非直接运行 NetBSD 二进制）。

Milestones / 里程碑:
1) Boot stability: QEMU boots to shell, PM/VM/RS/VFS stable, fork/exec/wait OK.  
   启动稳定：QEMU 进 shell，PM/VM/RS/VFS 稳定，fork/exec/wait 正常。
2) Userland ABI: RV64 ELF loading, ld.so relocations, syscalls/errno/signals OK.  
   用户态 ABI：RV64 ELF 装载、ld.so 重定位、syscall/errno/信号链路正确。
3) Toolchain & base tools: build tools + sysroot ready, bmake/sh/awk/sed/tar/gzip usable.  
   工具链与基础工具：交叉/本地工具链与 sysroot 就绪，bmake/sh/awk/sed/tar/gzip 可用。
4) pkgsrc port: MINIX/riscv64 mk files, bootstrap works, pkg_install/pkgconf builds.  
   pkgsrc 端口化：MINIX/riscv64 平台文件完善，bootstrap 完成，pkg_install/pkgconf 可构建。
5) Core libs: zlib/libarchive/libiconv/ncurses/libutil OK; networking if needed.  
   常用库：zlib/libarchive/libiconv/ncurses/libutil 可用；需要时补齐网络栈。
6) Regression & tests: smoke set + POSIX/VM/signals, QEMU automation.  
   回归与测试：smoke 套件 + POSIX/VM/信号测试，QEMU 自动化。
