# MINIX 3 RISC-V 64 Port (Workspace) / MINIX 3 RISC-V 64 移植（工作区）

This repository contains the full MINIX 3 source tree plus an active RISC-V 64-bit
(evbriscv64) port targeting the QEMU virt platform.
本仓库包含完整的 MINIX 3 源码树，以及面向 QEMU virt 平台的 RISC-V 64 位移植版本。

**Documentation Version / 文档版本**: 1.2 (2026-01-07)

## Current Status / 当前状态（as of 2026-01-07 / 截至 2026-01-07）

- Build: passes with workaround flags; exact commands are in `README-RISCV64.md`.
  构建：使用绕过项后可通过，具体命令见 `README-RISCV64.md`。
- Runtime: kernel reaches early init; user space is not yet stable; see `RISC64-STATUS.md`.
  运行：内核可进入早期初始化，用户态尚不稳定，详见 `RISC64-STATUS.md`。
- Risk tracking: code-review issues with file/line evidence are in `issue.md`.
  风险跟踪：带文件/行号证据的评审问题见 `issue.md`。
- Pre-2026-01-06 01:00 changes: userland gp init (crt0 + gp.c), exec/ucontext +
  VM exec flags, IPC/pagefault ABI fixes (64-bit addr, senda arg order).
  2026-01-06 01:00 前变更：用户态 gp 初始化（crt0 + gp.c）、exec/ucontext 与 VM
  执行权限标记、IPC/缺页 ABI 修复（64 位地址、senda 参数顺序）。
- Status refresh: doc update after reviewing pre-2026-01-06 01:00 changes; no new
  build/test run since 2026-01-06.
  状态更新：已根据 2026-01-06 01:00 前代码变更补充文档，自 2026-01-06 起未重新构建或测试。

## Quick Links / 快速链接

- `README-RISCV64.md` — build/run/test manual / 构建、运行、测试说明
- `RISC64-STATUS.md` — status snapshot and risk list / 状态快照与风险清单
- `issue.md` — code-review issues with evidence / 评审问题与证据
- `docs/RISCV64_PORTING_GUIDE.md` — porting workflow and reading coverage / 移植流程与阅读覆盖
- `docs/RISCV64_KERNEL_BUILD_LOG.md` — build log and commands / 构建日志与命令记录
- `docs/RISCV64_PORT_PLAN.md` — plan and checklist / 计划与检查清单

## Repository Layout / 目录结构

- `minix/`        MINIX kernel, servers, drivers, libraries, tests / 内核、服务器、驱动、库、测试
- `sys/`          NetBSD compatibility layer / NetBSD 兼容层
- `include/`      Userland headers / 用户态头文件
- `tools/`        Build toolchain / 构建工具链
- `docs/`         Porting documentation / 移植文档
- `scripts/`      Build and QEMU helpers / 构建与 QEMU 脚本

## Build (RISC-V 64) / 构建（RISC-V 64）

Recommended baseline (GCC, no LLVM/C++). See `README-RISCV64.md` for details.
推荐基线（GCC、禁用 LLVM/C++），详细说明见 `README-RISCV64.md`。

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

## Run in QEMU / QEMU 运行

```bash
./minix/scripts/qemu-riscv64.sh -k minix/kernel/obj/kernel -B obj/destdir.evbriscv64
```

## Tests / 测试

```bash
./minix/tests/riscv64/run_tests.sh all
```

## Documentation Update Rules / 文档更新规则

- New issues: record in `issue.md` with symptoms, logs, file/line evidence, and impact.
  新问题：在 `issue.md` 记录症状、日志、文件/行号证据与影响。
- Status changes: update `RISC64-STATUS.md` with date and outcome.
  状态变更：更新 `RISC64-STATUS.md`，包含日期与结果。
- Build/test changes: append to `docs/RISCV64_KERNEL_BUILD_LOG.md` and adjust
  `README-RISCV64.md` if the baseline command changes.
  构建/测试变更：追加到 `docs/RISCV64_KERNEL_BUILD_LOG.md`，若基线命令变化则同步更新
  `README-RISCV64.md`。
- Doc-only refresh: append a note to `docs/RISCV64_KERNEL_BUILD_LOG.md` and bump
  doc dates where applicable.
  仅文档更新：在 `docs/RISCV64_KERNEL_BUILD_LOG.md` 记录说明并同步更新文档日期。
- Reading notes: add findings to `docs/RISCV64_PORTING_GUIDE.md`.
  阅读记录：补充到 `docs/RISCV64_PORTING_GUIDE.md`。
