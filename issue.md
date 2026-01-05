# MINIX RISC-V Port Issues / MINIX RISC-V 移植问题清单

**Date / 日期**: 2026-01-06  
**Version / 版本**: 1.0  
**Scope / 范围**: RISC-V 64-bit port, evidence includes file/line references.

本文件记录 RISC-V 64 位移植的具体问题与证据（含文件/行号），并给出修复建议。  
This file records concrete issues in the RISC-V 64-bit port with evidence and suggested fixes.

## Active Investigation / 当前主问题跟踪

### A1) Userland services stuck in user pagefault loop during boot / 用户态服务启动期反复缺页
- Evidence / 证据:
  - VFS: `sef_receive_status` at `minix/lib/libsys/sef.c:151`, `mthread_trampoline` at `minix/lib/libmthread/allocate.c:532`,
    `malloc_bytes` at `lib/libc/stdlib/malloc.c:792`, `memset` at `common/lib/libc/string/memset.c:156`
  - MFS: `extend_pgdir` at `lib/libc/stdlib/malloc.c:464`, `imalloc` at `lib/libc/stdlib/malloc.c:831`,
    `kdoprnt` at `sys/lib/libsa/subr_prf.c:328`
  - DS: `_regcomp` at `lib/libc/regex/regcomp.c:333`, `extend_pgdir` at `lib/libc/stdlib/malloc.c:464`,
    `imalloc` at `lib/libc/stdlib/malloc.c:831`, `kdoprnt` at `sys/lib/libsa/subr_prf.c:224`
- Impact / 影响:
  - Boot does not reach stable userland; init dies and services spin on faults. / 启动无法进入稳定用户态；init 退出且服务反复缺页。
- Hypothesis / 假设:
  - RV64 heap growth or page table extension path (`extend_pgdir` / `malloc_pages`) maps invalid VA. / RV64 堆扩展或页表扩展路径可能映射了非法 VA。
  - May be related to PTROOT truncation issue (#1). / 可能与 PTROOT 截断问题（#1）相关。
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
  - Resolve A1 + Critical items (PTROOT truncation, SATP VA/PA mismatch, clock/IRQ paths), then validate exec with a minimal ELF64 user binary + ld.so. / 先修复 A1 与严重问题，再用最小 ELF64 用户程序验证 exec/ld.so。
  - Validation checklist (doc-only): / 验证清单（文档）:
    1) Confirm target ELF64/EM_RISCV via `readelf -h` on the test binary. / 通过 `readelf -h` 确认 ELF64/EM_RISCV。
    2) Prefer a minimal static executable if available; otherwise verify `PT_INTERP` points to `/libexec/ld.elf_so`. / 尽量使用静态可执行文件；否则确认 `PT_INTERP` 指向 `/libexec/ld.elf_so`。  
       Evidence: `minix/drivers/storage/ramdisk/proto.common.dynamic:2`, `minix/servers/vfs/exec.c:282`
    3) Ensure dynamic loader is mapped below stack as per VFS logic. / 确认动态加载器按 VFS 逻辑映射到栈下。  
       Evidence: `minix/servers/vfs/exec.c:306`
    4) Keep the test binary minimal (single `main`, no threads) to isolate VM/exec issues. / 测试程序保持最小化以隔离 VM/exec 问题。

## Critical / 严重

### 1) 64-bit page table root truncated to 32-bit in sys_vmctl path / sys_vmctl 路径 PTROOT 32 位截断
- Evidence / 证据:
  - `minix/include/minix/com.h:381` defines `SVMCTL_PTROOT` as `m1_i3` (32-bit)
  - `minix/lib/libsys/sys_vmctl.c:30-40` packs `phys_bytes ptroot` into `m.SVMCTL_PTROOT`
  - `minix/kernel/arch/riscv64/arch_do_vmctl.c:75` casts `m_ptr->SVMCTL_PTROOT` to `u32_t`
  - `minix/servers/vm/pagetable.c:2186` passes `pt->pt_dir_phys` to `sys_vmctl_set_addrspace`
- Impact / 影响:
  - Page table roots above 4GB are truncated, causing wrong SATP and hangs. / 4GB 以上页表根被截断，SATP 错误并导致切换挂起。
  - Matches observed “pt_bind then SATP switch timeout.” / 与“pt_bind 后 SATP 切换超时”现象一致。
- Suggested fix / 修复建议:
  - Make PTROOT transport 64-bit end-to-end (split hi/lo or new message). / 端到端改为 64-bit 传参（拆分高低位或新增消息字段）。
  - Update `sys_vmctl_set_addrspace`, kernel handler, and `sys_vmctl_get_pdbr`. / 同步更新 `sys_vmctl_set_addrspace`、内核处理与 `sys_vmctl_get_pdbr`。

### 2) SATP root virtual pointer stored as physical address / SATP 根地址以物理指针保存
- Evidence / 证据:
  - `minix/kernel/arch/riscv64/arch_do_vmctl.c:19-21` sets `p->p_seg.p_satp_v = (reg_t *)root`
  - `minix/kernel/arch/riscv64/memory.c:52-56` uses `p_seg.p_satp_v` as a virtual pointer
  - `minix/servers/vm/pagetable.c:2174-2186` computes `ptroot_v` but kernel ignores it
- Impact / 影响:
  - Kernel dereferences a physical address as VA after paging is enabled. / 启用分页后内核将物理地址当作 VA 解引用。
  - Can crash or hang during address-space handoff. / 地址空间切换可能崩溃或挂起。
- Suggested fix / 修复建议:
  - Store and use `ptroot_v`, or establish a stable KVA mapping for PT roots. / 保存并使用 `ptroot_v`，或建立稳定 KVA 映射。

## Major / 重要

### 3) Timer interrupt does not drive kernel clock/scheduler / 时钟中断未驱动内核时钟
- Evidence / 证据:
  - `minix/kernel/arch/riscv64/arch_clock.c:82-102` increments `total_ticks` but does not call the
    generic kernel clock handler (commented TODO).
- Impact / 影响:
  - No periodic scheduling or timekeeping after boot. / 启动后无周期调度与时间推进。
- Suggested fix / 修复建议:
  - Call the kernel clock handler (e.g., `clock_handler()` or MINIX clock entry). / 接入内核时钟处理函数。

### 4) Leaf → non-leaf page table split lacks TLB flush / 叶子页拆分后未刷新 TLB
- Evidence / 证据:
  - `minix/servers/vm/pagetable.c:50-125` (`pt_l0alloc`) and `129-170` (`pt_l1alloc`) convert
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

### 8) Breakpoint exception always advances PC by 2 bytes / 断点异常固定前移 2 字节
- Evidence / 证据:
  - `minix/kernel/arch/riscv64/exception.c:164-166`
- Impact / 影响:
  - Non-compressed `ebreak` is 4 bytes; advancing by 2 breaks instruction flow. / 非压缩 `ebreak` 为 4 字节，前移 2 字节会跳入指令中间。
- Suggested fix / 修复建议:
  - Decode instruction length before advancing `sepc`. / 根据指令长度推进 `sepc`。

### 9) Software interrupts (IPI) not enabled in SIE / SIE 中未开启 SSIE
- Evidence / 证据:
  - `minix/kernel/arch/riscv64/exception.c:61-62` enables only STIE/SEIE
- Impact / 影响:
  - SMP IPIs will not be delivered even if SBI works. / 即使 SBI 正常，SMP IPI 也不会送达。
- Suggested fix / 修复建议:
  - Enable `SIE_SSIE` when SMP is configured. / SMP 场景下开启 `SIE_SSIE`。

## Moderate / 中等

### 10) Static page fault message is not per-CPU / 缺页消息为静态共享
- Evidence / 证据:
  - `minix/kernel/arch/riscv64/exception.c:11` uses static `m_pagefault`
- Impact / 影响:
  - SMP can race and corrupt page fault notifications. / SMP 下可能竞态并破坏缺页通知。
- Suggested fix / 修复建议:
  - Use per-CPU storage or allocate on stack. / 使用每 CPU 存储或栈上分配。

### 11) Minimal kernel build is not RISC-V-ready / minimal_kernel 未支持 RISC-V
- Evidence / 证据:
  - `minimal_kernel/include/minix/const.h:83-87` rejects non-i386/arm
  - Build fails due to incomplete header/machine wiring (see build logs)
- Impact / 影响:
  - `minimal_kernel/` cannot be used for RISC-V minimal boot tests. / `minimal_kernel/` 无法用于 RISC-V 最小化测试。
- Suggested fix / 修复建议:
  - Add RISC-V support in minimal kernel headers or exclude it from RISC-V builds. / 在 minimal kernel 增加 RISC-V 支持或在 RISC-V 构建中排除。

## Technical Debt / 技术债务

### TD1) Static RISC-V links require per-binary __global_pointer$ workaround / 静态链接需要每个二进制打补丁
- Evidence / 证据:
  - Link failure without workaround: `crt0.o: undefined reference to '__global_pointer$'`
  - Workarounds in `minix/servers/vfs/gp.c` + `minix/servers/vfs/Makefile` (and similarly in DS/MFS).
- Impact / 影响:
  - Requires per-binary patches; easy to miss. / 需要逐个二进制打补丁，容易遗漏。
- Suggested fix / 修复建议:
- Define `__global_pointer$` in crt0 or linker script globally, then remove per-binary gp.c/LDFLAGS. / 在 crt0 或链接脚本中全局定义 `__global_pointer$`，再移除各二进制 gp.c/LDFLAGS。

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
