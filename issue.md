# MINIX RISC-V Port Issues / MINIX RISC-V 移植问题清单

**Date / 日期**: 2026-01-04  
**Scope / 范围**: RISC-V 64-bit port, evidence includes file/line references.

本文件记录 RISC-V 64 位移植的具体问题与证据（含文件/行号），并给出修复建议。  
This file records concrete issues in the RISC-V 64-bit port with evidence and suggested fixes.

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

## Fixed in Current Working Tree / 已在当前工作区修复

- `minimal_kernel/proto.h:175` uses `reg_t` for `arch_set_secondary_ipc_return` to avoid RV64 truncation
  (matches `minix/kernel/proto.h` and arch implementations).  
  `minimal_kernel/proto.h:175` 已改为 `reg_t`，避免 RV64 截断（与 `minix/kernel/proto.h` 及架构实现一致）。
