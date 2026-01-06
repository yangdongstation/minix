# MINIX RISC-V 64-bit Port Status / MINIX RISC-V 64 位移植状态

**Date / 日期**: 2026-01-07  
**Version / 版本**: 1.1  
**Status / 状态**: Phase 1 in progress — buildable with workarounds, runtime unstable  
**Progress / 进度**: ~60% (core kernel present; VM/IO issues outstanding)

## Summary / 摘要

**中文**
- 构建可通过（需使用绕过项与特定构建变量），详见 `README-RISCV64.md`。
- QEMU 启动可进入早期内核初始化，但用户态未稳定进入。
- 关键风险集中在页表根传参、UART、TLB 刷新与 SBI IPI/fence 路径（见 `issue.md`）。
- 本次仅同步文档，自 2026-01-06 起未重新构建或测试，状态沿用。

**English**
- Build passes with workaround flags; see `README-RISCV64.md` for exact commands.
- QEMU boot reaches early kernel init; user space is not yet stable.
- Key risks are in page-table root handoff, UART, TLB flush, and SBI IPI/fence paths (see `issue.md`).
- Documentation sync only; no new build/test run since 2026-01-06; status unchanged.

## Build Status / 构建状态

**中文**
- 基线命令：使用 GCC、禁用 LLVM/C++、放宽 `checkflist`（见 `README-RISCV64.md`）。
- 产物：`minix/kernel/obj/kernel` 与 `obj/destdir.evbriscv64`。
- 限制：`CHECKFLIST_FLAGS='-m -e'` 为临时绕过，需在 sets 完整后移除。

**English**
- Baseline: GCC, LLVM/C++ disabled, relaxed `checkflist` (see `README-RISCV64.md`).
- Outputs: `minix/kernel/obj/kernel` and `obj/destdir.evbriscv64`.
- Limitation: `CHECKFLIST_FLAGS='-m -e'` is a temporary workaround until sets are complete.

## Runtime Status / 运行状态

**中文**
- 观察到 `rv64: kernel_main` 等早期日志，但随后出现 `System reset...` 并重复 OpenSBI banner。
- 运行时稳定性不足，用户态服务尚未可靠启动。
- 该行为与 `issue.md` 中的 PTROOT 截断等关键问题一致。

**English**
- Logs show early messages like `rv64: kernel_main`, then `System reset...` with repeated OpenSBI banner.
- Runtime stability is insufficient; user space services are not reliably started.
- This matches the PTROOT truncation and other key issues in `issue.md`.

## Key Issues (Snapshot) / 关键问题（摘要）

**Critical / 严重**
- sys_vmctl PTROOT 32-bit truncation on riscv64 (address-space handoff breaks).

**Major / 重要**
- Leaf->non-leaf page table split lacks TLB flush.
- UART blocking read returns `EDONTREPLY` without deferred reply.
- SBI legacy IPI/fence uses VA instead of PA when paging is enabled.

详见 `issue.md` 的证据与修复建议 / See `issue.md` for evidence and fixes.

## Evidence Sources / 证据来源

- `issue.md` (code review evidence with file/line references)
- `docs/RISCV64_KERNEL_BUILD_LOG.md` (build history and commands)
- `README-RISCV64.md` (boot/test notes and baseline procedures)

## Next Priorities / 下一阶段优先级

**中文**
1) 修复 PTROOT 传参为 64-bit 并端到端更新。
2) 修复 UART 阻塞读的延迟回复。
3) leaf->non-leaf 拆分后补齐 TLB 刷新。
4) 迁移 SBI legacy IPI/fence 到 v0.2+ 或传递 PA。

**English**
1) Make PTROOT 64-bit end-to-end.
2) Fix UART deferred reply for blocking reads.
3) Flush TLB after leaf->non-leaf splits.
4) Move SBI legacy IPI/fence to v0.2+ or pass PA.

## Success Criteria / 下一里程碑判定

**中文**
- QEMU 中稳定进入用户态（无 `System reset...` 循环）。
- 时钟中断驱动调度与时间推进。
- UART 阻塞读可正常返回。
- sys_vmctl PTROOT 在 >4GB 物理地址下可靠切换 SATP。

**English**
- Stable boot to userland in QEMU without reset loops.
- Timer IRQ drives scheduling/timekeeping.
- UART blocking reads return correctly.
- sys_vmctl PTROOT switches SATP reliably above 4GB physical roots.
