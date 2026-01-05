# LiteOS-M and LiteOS-A Emulation Architecture

Status: design-only. This document describes a full emulation approach using
QEMU + host OS.

## Goals

- Staged delivery with verifiable milestones.
- Core kernel behavior emulation (scheduler, memory, IPC, timer, interrupts).
- API compatibility for LiteOS-M (CMSIS/POSIX subset) and LiteOS-A (broader
  POSIX + LiteIPC).
- Modular design to add devices and board support incrementally.
- Stable, long-running test capability.

## Constraints and Feasibility Notes

- LiteOS-M upstream dynlink loads ELF shared objects (ET_DYN) and uses ELF32
  for non-AARCH64; upstream RISC-V tree is RV32-only. This implies RV32 is the
  realistic starting point for RISC-V LiteOS-M emulation.
  References:
  - https://gitee.com/openharmony/kernel_liteos_m/blob/master/components/dynlink/los_dynlink.c#L103-L112
  - https://gitee.com/openharmony/kernel_liteos_m/blob/master/components/dynlink/los_dynlink.h#L44-L64
  - https://gitee.com/openharmony/kernel_liteos_m/blob/master/README.md#L23-L40
- LiteOS-A scope is substantially larger (MMU, processes, LiteIPC, richer
  POSIX). It should be a dedicated phase with explicit scope control.

## Architecture Overview

Layers (bottom to top):

1) Host OS layer:
   - Provides CPU, memory, filesystem, and networking resources.
2) Hardware emulation layer (QEMU):
   - Emulates CPU, timers, interrupt controllers, and basic peripherals.
3) HAL layer:
   - Provides a stable hardware abstraction interface to the kernel.
4) Kernel function layer:
   - LiteOS-M/A kernel logic with hardware calls routed through HAL.
5) API mapping layer:
   - Exposes CMSIS/POSIX/LiteOS APIs to applications.

## LiteOS-M Track (RV32 First)

- Use QEMU riscv32_virt (or a supported ARM Cortex-M board model) as the
  baseline.
- Implement minimal HAL bindings for timer, UART, and interrupt controller.
- Map LiteOS tasks to host threads (pthreads) for scheduling simulation.
- Emulate IPC primitives using host synchronization (mutex, condvar, sem).

## LiteOS-A Track (AP-Class)

- Use a QEMU Cortex-A or QEMU virt platform with MMU support.
- Implement process model, VM, and basic syscall dispatch.
- Add LiteIPC emulation (device-file-like interface + service manager model).

## 分阶段可执行清单

说明：每一阶段都要求“可交付成果 + 验证方式”。以下清单为最小可行路径，
可根据实际板卡与驱动清单细化。

### Phase 1: 环境与引导

目标：建立可重复的 QEMU 启动与日志链路。

- 步骤:
  - 确认 QEMU 版本与目标 machine 类型（riscv32_virt 或 Cortex-M）。
  - 配置交叉编译工具链并记录版本。
  - 构建最小内核镜像并启动到串口输出。
- 交付物:
  - 启动脚本（含 machine/cpu/mem/串口参数）。
  - 构建配置记录（工具链版本 + 构建参数）。
  - 启动日志（含关键初始化标记）。
- 验证:
  - 串口出现预期启动标记，重复启动结果一致。

### Phase 2: LiteOS-M 核心机制

目标：完成 LiteOS-M 最小可用内核闭环。

- 步骤:
  - HAL：完成 timer + UART + IRQ 控制器绑定。
  - 内核：任务创建/切换、tick 驱动、基础 IPC（信号量/队列/事件）。
  - 中断：注册/触发/回调路径可观测。
- 交付物:
  - “双任务交替 + 延时”示例。
  - IPC 最小测试（信号量/队列/事件）。
  - 中断触发日志（含 IRQ 号与回调确认）。
- 验证:
  - 任务交替输出符合预期节奏。
  - IPC 阻塞/唤醒行为可复现。
  - IRQ 触发与回调一致且无死锁。

### Phase 3: LiteOS-A 内核基础

目标：具备最小用户态执行链路。

- 步骤:
  - MMU 启用与页表管理。
  - 进程/线程创建与销毁。
  - 系统调用分发框架（最小 read/write/exit）。
- 交付物:
  - 最小用户程序（打印/退出）。
  - 页表与地址映射日志。
- 验证:
  - 用户程序可稳定运行并退出。
  - 缺页处理路径可观测且可复现。

### Phase 4: API 覆盖与 LiteIPC

目标：形成可移植 API 面。

- 步骤:
  - LiteOS-M：CMSIS/POSIX 子集映射。
  - LiteOS-A：更完整 POSIX 覆盖 + LiteIPC 基础流。
- 交付物:
  - API 覆盖清单（已支持/未支持/替代策略）。
  - LiteIPC 最小 RPC 示例（服务注册/调用/返回）。
- 验证:
  - API 示例程序可运行并与预期一致。
  - LiteIPC 流程可复现且日志清晰。

### Phase 5: 优化与回归

目标：性能与稳定性达标。

- 步骤:
  - 长时间稳定性测试（运行小时级）。
  - 性能剖析与瓶颈记录。
  - 回归套件与 CI 骨架。
- 交付物:
  - 稳定性报告（运行时长/崩溃率/资源曲线）。
  - 性能基线数据。
  - 回归用例列表与执行脚本。
- 验证:
  - 长时间运行无崩溃或资源泄漏。
  - 性能基线可重复。

## 验证策略（文档级）

- 以“日志可复现 + 最小用例可运行”为核心。
- 每个子系统至少 1 个最小用例，且有可比对的输出。
- LiteOS-A 必须包含系统调用与 LiteIPC 的最小闭环。

## Risks and Mitigations

- Cross-arch complexity:
  - Start with a single board/arch (riscv32_virt or a Cortex-M board).
- Behavioral drift from real hardware:
  - Keep HAL narrow and document all deviations.
- Scope creep for LiteOS-A:
  - Enforce explicit deliverables per phase.
