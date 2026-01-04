# MINIX 3 RISC-V 64-bit Port Guide / MINIX 3 RISC-V 64-bit 移植指南

## 概述 / Overview

本文档记录 MINIX 3 在 RISC-V 64-bit 架构上的移植与构建运行方法，面向 QEMU virt 平台。  
This document describes the MINIX 3 RISC-V 64-bit port, including build/run procedures
targeting the QEMU virt platform.

## 文档信息 / Document Info

**中文**
- 版本：0.3
- 最后更新：2026-01-04
- 适用范围：evbriscv64（QEMU virt）
- 文档性质：构建/运行/测试操作手册，不是开发计划

**English**
- Version: 0.3
- Last updated: 2026-01-04
- Scope: evbriscv64 (QEMU virt)
- Doc type: build/run/test manual, not a development plan

## 当前状态（截至 2026-01-04）/ Current Status (as of 2026-01-04)

**中文**
- 构建：可通过（需使用 workaround 组合，见本文构建命令与 `RISC64-STATUS.md`）
- 运行：内核可进入早期初始化，但尚未稳定进入用户态
- 关键风险：PTROOT 32 位截断、SATP 根地址 VA/PA 不一致、时钟未接入内核、UART 阻塞读缺少回复（详见 `issue.md`）
- 进度估计：约 60%（内核基础具备，VM/时钟/IO 关键链路仍待修复）

**English**
- Build: passes with workaround flags (see commands below and `RISC64-STATUS.md`)
- Runtime: kernel reaches early init; user space not yet stable
- Key risks: PTROOT 32-bit truncation, SATP root VA/PA mismatch, missing kernel clock hookup,
  UART blocking read reply (see `issue.md`)
- Progress estimate: ~60% (core kernel in place; VM/clock/IO still pending)

## 系统要求 / System Requirements

### 主机系统 / Host System

**中文**
- Linux x86_64
- 至少 8GB RAM
- 20GB 可用磁盘空间
- GCC 交叉编译工具链支持

**English**
- Linux x86_64
- At least 8GB RAM
- 20GB free disk space
- GCC cross-compilation toolchain support

### 目标平台 / Target Platform

**中文**
- RISC-V 64-bit (RV64GC)
- QEMU virt 平台
- 支持 SBI (Supervisor Binary Interface)

**English**
- RISC-V 64-bit (RV64GC)
- QEMU virt platform
- SBI (Supervisor Binary Interface) supported

## 构建步骤 / Build Steps

### 1. 准备交叉编译工具链 / Prepare Toolchain

```bash
# 安装 RISC-V GCC 工具链（Ubuntu/Debian）
sudo apt-get install gcc-riscv64-unknown-elf

# 或从源码构建
git clone https://github.com/riscv/riscv-gnu-toolchain
cd riscv-gnu-toolchain
./configure --prefix=/opt/riscv --enable-multilib
make
```

**中文**
- 若已安装交叉编译器，可跳过源码构建。

**English**
- If a cross-compiler is already available, the source build step can be skipped.

### 2. 配置构建环境 / Configure Build Environment

```bash
cd /path/to/minix

# 设置构建变量
export TOOLDIR=/path/to/minix/obj/tooldir.$(uname -s)-$(uname -r)-$(uname -m)
export DESTDIR=/path/to/minix/obj/destdir.evbriscv64
```

**中文**
- 变量用于指定工具链与输出目录。

**English**
- These variables point to the toolchain and output directories.

### 3. 构建交叉编译工具 / Build Tools

```bash
# 构建主机工具（需要较长时间）
MKPCI=no HOST_CFLAGS="-O -fcommon" HAVE_GOLD=no ./build.sh -U -m evbriscv64 tools

# 如需强制使用 GCC（避免自动选择 clang）
MKPCI=no HOST_CFLAGS="-O -fcommon" HAVE_GOLD=no \
./build.sh -U -m evbriscv64 \
  -V AVAILABLE_COMPILER=gcc -V ACTIVE_CC=gcc -V ACTIVE_CPP=gcc -V ACTIVE_CXX=gcc -V ACTIVE_OBJC=gcc \
  tools
```

**中文**
- 若工具链已在 `obj/tooldir.*` 中生成，可直接进入下一步。

**English**
- If `obj/tooldir.*` already exists, proceed to the distribution build.

### 4. 构建完整系统 / Build Distribution

```bash
# 构建 MINIX distribution（需要 2-4 小时）
# 当前可通过构建的组合：允许清单缺失/多余文件，适用于移植进行中阶段
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

**说明 / Notes**
- 若交叉编译器支持 `-march=rv64gc -mabi=lp64d`，可移除 `RISCV_ARCH_FLAGS`。  
  If the toolchain supports `-march=rv64gc -mabi=lp64d`, remove `RISCV_ARCH_FLAGS`.
- `CHECKFLIST_FLAGS='-m -e'` 允许缺失/多余文件，适用于当前不完整的 sets；若需严格检查，请移除该标志并恢复 `MKPIC/MKCXX/MKATF`。  
  `CHECKFLIST_FLAGS='-m -e'` allows missing/extra files while sets are incomplete; remove it
  and restore `MKPIC/MKCXX/MKATF` for strict checks.

## 本次移植与验证步骤（evbriscv64）/ Porting & Validation Steps (evbriscv64)

### 1. CSU 移植 / CSU Porting

**中文**
- 在 `lib/csu/arch/riscv64/` 增加 riscv64 入口与 Makefile 包装层，复用 `lib/csu/arch/riscv/*` 实现。
- 在 `lib/csu/arch/riscv64/machine/asm.h` 与 `lib/csu/arch/riscv/riscv64/machine/asm.h`
  补齐 `PTR_SCALESHIFT`，避免未安装系统头时缺宏。

**English**
- Add riscv64 entry and Makefile wrappers under `lib/csu/arch/riscv64/`, reusing
  `lib/csu/arch/riscv/*` implementations.
- Add `PTR_SCALESHIFT` in `lib/csu/arch/riscv64/machine/asm.h` and
  `lib/csu/arch/riscv/riscv64/machine/asm.h` to avoid missing macros before headers install.

### 2. CSU 编译验证 / CSU Build Check

```bash
MAKEFLAGS="-de -m $PWD/share/mk" \
TOOLDIR="$PWD/obj/tooldir.$(uname -s)-$(uname -r)-$(uname -m)" \
MACHINE=evbriscv64 MACHINE_ARCH=riscv64 NETBSDSRCDIR="$PWD" \
DESTDIR="$PWD/obj/destdir.evbriscv64" \
AVAILABLE_COMPILER=gcc ACTIVE_CC=gcc ACTIVE_CPP=gcc ACTIVE_CXX=gcc ACTIVE_OBJC=gcc \
RISCV_ARCH_FLAGS='-march=RV64IMAFD -mcmodel=medany' \
"$PWD/obj/tooldir.$(uname -s)-$(uname -r)-$(uname -m)/bin/nbmake" \
  -C lib/csu dependall
```

### 3. 工具链可用性验证 / Toolchain Check

```bash
obj/tooldir.*/bin/riscv64-elf32-minix-gcc -dumpmachine
# 期望输出：riscv64-elf32-minix
# Expected: riscv64-elf32-minix
```

### 4. 完整构建（跳过 libobjc/libgomp/C++）/ Full Build (skip libobjc/libgomp/C++)

```bash
unset EXTERNAL_TOOLCHAIN
HOST_CC="cc -Wno-implicit-int -Wno-implicit-function-declaration" \
MKPCI=no HOST_CFLAGS="-O -fcommon" HAVE_GOLD=no HAVE_LLVM=no MKLLVM=no \
./build.sh -u -j"$(nproc)" -m evbriscv64 \
  -V AVAILABLE_COMPILER=gcc -V ACTIVE_CC=gcc -V ACTIVE_CPP=gcc -V ACTIVE_CXX=gcc -V ACTIVE_OBJC=gcc \
  -V RISCV_ARCH_FLAGS='-march=RV64IMAFD -mcmodel=medany' \
  -V NOGCCERROR=yes -V MKPIC=no -V MKPICLIB=no -V MKPICINSTALL=no \
  -V MKGCC=yes -V MKGCCCMDS=no -V MKLIBSTDCXX=no \
  -V MKCXX=no -V MKLIBCXX=no -V MKATF=yes -V MKKYUA=yes -V USE_PCI=no \
  -V MKLIBOBJC=no -V MKLIBGOMP=no \
  -V CHECKFLIST_FLAGS='-m -e' \
  distribution
```

**说明 / Notes**
- `MKLIBOBJC=no` 与 `MKLIBGOMP=no` 用于避开 `pthread.h` 未齐全导致的构建失败。  
  `MKLIBOBJC=no` and `MKLIBGOMP=no` avoid failures from incomplete `pthread.h`.
- `NOGCCERROR=yes` 用于屏蔽旧 GCC 不识别的版本化警告。  
  `NOGCCERROR=yes` suppresses versioned warning flags on older GCC.

### 5. 测试与当前结果 / Tests and Current Results

```bash
./minix/tests/riscv64/run_tests.sh all
```

**中文（截至 2026-01-04）**
- 用户态编译测试：全部通过（脚本已自动使用 in-tree toolchain + sysroot，并统一 `-std=gnu99`）。
- 内核启动测试：失败，QEMU 中出现 `rv64: kernel_main` 后触发 `System reset...`，详见 `/tmp/boot_test.log`。
  该失败与地址空间切换/时钟链路等关键问题高度相关（详见 `issue.md`）。
- SMP initialization：当前脚本固定标记为跳过（not yet implemented）。

**English (as of 2026-01-04)**
- Userland compile tests: pass (script uses in-tree toolchain + sysroot and `-std=gnu99`).
- Kernel boot test: fails; QEMU shows `rv64: kernel_main` then `System reset...` (see `/tmp/boot_test.log`).
  This correlates with address-space handoff/clock issues (see `issue.md`).
- SMP initialization: script marks as skipped (not yet implemented).

#### 5.1 内核启动复位排查记录 / Boot Reset Investigation

1. 执行测试：`./minix/tests/riscv64/run_tests.sh all`  
   Run test: `./minix/tests/riscv64/run_tests.sh all`
2. 查看日志：`/tmp/boot_test.log`  
   Inspect log: `/tmp/boot_test.log`
   - 可见 OpenSBI banner  
     OpenSBI banner present
   - 内核初始化输出：`rv64: kernel_main`、`rv64: kmain entry`、`rv64: mods_count=0`  
     Kernel init output: `rv64: kernel_main`, `rv64: kmain entry`, `rv64: mods_count=0`
   - 随后出现 `System reset...`，OpenSBI banner 重复  
     Then `System reset...` and OpenSBI banner repeats
3. 结论：内核已进入早期初始化，但在继续启动过程中触发复位（原因待进一步定位）  
   Conclusion: kernel reaches early init but resets during further boot (root cause pending)
4. 初步排查建议：  
   Initial investigation steps:
   - 关注是否有显式复位/关机路径被触发（`minix/kernel/arch/riscv64/sbi.c`、`minix/kernel/arch/riscv64/exception.c`）  
     Check explicit reset/shutdown paths (`minix/kernel/arch/riscv64/sbi.c`, `minix/kernel/arch/riscv64/exception.c`)
   - 在 `rv64:` 打点附近继续细化日志，定位复位发生点  
     Add more logging around `rv64:` markers to locate reset point
   - 若需要交互调试，使用 `minix/scripts/qemu-riscv64.sh -d -k minix/kernel/obj/kernel -B obj/destdir.evbriscv64`  
     For interactive debug, use `minix/scripts/qemu-riscv64.sh -d -k minix/kernel/obj/kernel -B obj/destdir.evbriscv64`

#### 5.2 测试脚本启动判定说明 / Boot Test Criteria

`run_tests.sh` 的内核启动测试通过以下条件之一判断成功：  
`run_tests.sh` considers boot successful if any of the following is present:

```bash
grep -q "MINIX" /tmp/boot_test.log
# 或 / or
grep -q "rv64: arch_post_init" /tmp/boot_test.log
# 或 / or
grep -q "rv64: arch_boot_proc VM" /tmp/boot_test.log
```

当前内核日志未输出 `MINIX` 字符串，因此该用例会失败。  
The current log does not include `MINIX`, so the test fails.

可选处理方式 / Options:
- 将判定条件调整为当前已有的启动标记（例如 `rv64: kernel_main` 或 `rv64: kmain entry`）  
  Adjust the check to current markers (e.g., `rv64: kernel_main` or `rv64: kmain entry`)
- 或者在内核早期输出包含 `MINIX` 的 banner  
  Or emit a `MINIX` banner early in boot

#### 5.3 运行判定标准（最低可用）/ Runtime Pass Criteria (Minimum)

通过条件（任一满足即可）/ Pass criteria (any one):
1) `/tmp/boot_test.log` 出现 `rv64: arch_post_init` 或 `rv64: arch_boot_proc VM`  
   `/tmp/boot_test.log` shows `rv64: arch_post_init` or `rv64: arch_boot_proc VM`
2) 未出现 `System reset...` 且 OpenSBI banner 不重复  
   No `System reset...` and OpenSBI banner does not repeat

失败条件（任一满足即失败）/ Fail criteria (any one):
1) `rv64: kernel_main` 后出现 `System reset...` 并重复 OpenSBI banner  
   `rv64: kernel_main` followed by `System reset...` and repeated OpenSBI banner
2) 出现 `pagefault in VM` 或 `panic`  
   `pagefault in VM` or `panic` appears

当前状态：未满足通过条件（详见 `issue.md` 与 `RISC64-STATUS.md`）。  
Current status: pass criteria not met (see `issue.md` and `RISC64-STATUS.md`).

## 已知问题与解决方案 / Known Issues and Workarounds

### 0. 运行时关键问题（需先修复）/ Runtime Critical Issues (Fix First)

1. **PTROOT 32 位截断（sys_vmctl）**  
   - 表现：`pt_bind` 后 SATP 切换异常/超时  
   - 原因：`SVMCTL_PTROOT` 为 32-bit，riscv64 高位丢失  
   - 建议：改为 64-bit 传参并端到端更新（VM/内核/syslib）  
   **PTROOT 32-bit truncation (sys_vmctl)**  
   - Symptom: SATP switch fails/timeouts after `pt_bind`  
   - Cause: `SVMCTL_PTROOT` is 32-bit; riscv64 high bits lost  
   - Fix: make PTROOT 64-bit end-to-end (VM/kernel/syslib)

2. **SATP 根地址 VA/PA 不一致**  
   - 表现：启用分页后内核访问页表根发生异常  
   - 原因：`p_satp_v` 保存物理地址并当作 VA 使用  
   - 建议：使用 `ptroot_v` 或建立稳定的内核虚拟映射  
   **SATP root VA/PA mismatch**  
   - Symptom: kernel faults when accessing page-table root after paging  
   - Cause: `p_satp_v` stores PA but used as VA  
   - Fix: use `ptroot_v` or a stable KVA mapping

3. **时钟中断未接入内核时钟**  
   - 表现：调度/时间不前进  
   - 原因：`arch_clock_handler` 未调用内核 clock handler  
   - 建议：接入 MINIX 内核时钟处理函数  
   **Timer IRQ not wired to kernel clock**  
   - Symptom: no scheduling/timekeeping  
   - Cause: `arch_clock_handler` misses kernel clock handler  
   - Fix: call the kernel clock handler

4. **UART 阻塞读缺少回复**  
   - 表现：用户态 read 卡住  
   - 原因：驱动返回 `EDONTREPLY` 但中断路径未回复  
   - 建议：保存请求并在 RX 中断时回复  
   **UART blocking read lacks deferred reply**  
   - Symptom: userland read blocks  
   - Cause: driver returns `EDONTREPLY` without reply on RX interrupt  
   - Fix: store pending read and reply on RX IRQ

5. **leaf → non-leaf 变换未刷新 TLB**  
   - 表现：映射不一致/偶发错误  
   - 原因：页表拆分后未进行 SFENCE  
   - 建议：在拆分后刷新 TLB  
   **Leaf→non-leaf split lacks TLB flush**  
   - Symptom: inconsistent mappings  
   - Cause: missing SFENCE after split  
   - Fix: flush TLB after split

详细证据与文件行号见 `issue.md`。  
See `issue.md` for evidence and file/line references.

### 1. LLVM 编译问题 / LLVM Build Issues

**问题 / Problem**: LLVM 在 RISC-V 64-bit 上编译失败。  
**解决 / Fix**: 使用 `HAVE_LLVM=no MKLLVM=no` 跳过 LLVM 构建。

### 2. 交叉编译器 `-march=rv64gc` 不兼容 / `-march=rv64gc` Compatibility

**问题 / Problem**: 部分交叉编译器/本地 tooldir 的 GCC 不接受 `-march=rv64gc`。  
**解决 / Fix**: 使用 `-V RISCV_ARCH_FLAGS='-march=RV64IMAFD -mcmodel=medany'`。

### 3. distribution 清单检查失败 / distribution checkflist Failure

**问题 / Problem**: `checkflist` 报告缺失/多余文件，导致 distribution 失败。  
**解决 / Fix**: 移植阶段可用 `-V CHECKFLIST_FLAGS='-m -e'` 临时放宽；
完成 sets 后移除该标志并恢复 `MKPIC/MKCXX/MKATF`。

### 4. C++ 库目录问题（开启 C++ 时）/ C++ Include Directories (when C++ enabled)

若启用 `MKCXX/MKLIBSTDCXX`，创建 distribution 时可能需要手动创建：  
When `MKCXX/MKLIBSTDCXX` is enabled, create:

```bash
mkdir -p $DESTDIR/usr/include/g++/bits/riscv32
mkdir -p $DESTDIR/usr/include/g++/bits/riscv64
```

## 架构特定实现 / Architecture Details

### 内存映射（QEMU virt 平台）/ Memory Map (QEMU virt)

| 地址范围 / Address Range | 设备 / Device | 描述 / Description |
|--------------------------|--------------|--------------------|
| 0x02000000               | CLINT        | Core Local Interruptor |
| 0x0C000000               | PLIC         | Platform-Level Interrupt Controller |
| 0x10000000               | UART         | NS16550 serial console |
| 0x10001000               | VirtIO       | MMIO devices |
| 0x80000000               | RAM          | Physical memory start |

### SBI 接口使用 / SBI Usage

**中文**
- 系统使用 SBI 进行控制台输出、定时器管理与关机。

**English**
- SBI is used for console I/O, timer management, and system shutdown.

## 测试 / Tests

### 运行 RISC-V 特定测试 / Run RISC-V Tests

```bash
cd minix/tests/riscv64
./run_tests.sh all
```

可用的测试类别 / Available categories:
- `csr` - 控制和状态寄存器 / CSRs
- `atomic` - 原子操作 / Atomic ops
- `sbi` - SBI 接口调用 / SBI calls
- `memory` - 内存管理 / Memory management
- `trap` - 陷阱处理 / Trap handling
- `timer` - 定时器和 CLINT / Timer and CLINT
- `ipc` - 进程间通信 / IPC
- `vm` - 虚拟内存 / Virtual memory

### 使用 QEMU 运行 / Run in QEMU

```bash
# 基本运行 / Basic run
./minix/scripts/qemu-riscv64.sh -k minix/kernel/obj/kernel -B obj/destdir.evbriscv64

# 调试模式 / Debug mode
./minix/scripts/qemu-riscv64.sh -d -k minix/kernel/obj/kernel -B obj/destdir.evbriscv64

# 使用 GDB 调试 / GDB
./minix/scripts/gdb-riscv64.sh minix/kernel/obj/kernel
```

## 构建输出 / Build Outputs

1. **交叉编译工具链 / Toolchain**：`obj/tooldir.*/bin/riscv64-elf32-minix-*`
2. **内核镜像 / Kernel**：`minix/kernel/obj/kernel`
3. **系统库 / Libraries**：`obj/destdir.evbriscv64/usr/lib/`
4. **服务器程序 / Servers**：`obj/destdir.evbriscv64/sbin/`
5. **用户程序 / User programs**：`obj/destdir.evbriscv64/usr/bin/`

## 性能优化 / Performance Optimization

### 编译优化标志 / Compiler Flags

- `-march=rv64gc` - 通用 RISC-V 64-bit 扩展（工具链支持时）  
  RISC-V 64-bit general extensions (if toolchain supports)
- `-mabi=lp64d` - LP64 数据模型，双精度浮点  
  LP64 data model with double-precision FP
- 兼容 fallback：`-march=RV64IMAFD -mcmodel=medany`  
  Compatibility fallback: `-march=RV64IMAFD -mcmodel=medany`

### 内核优化 / Kernel Notes

- 使用内存屏障确保内存访问顺序 / Use memory barriers for ordering
- 优化中断处理路径 / Optimize interrupt paths
- 支持 SMP 多核（通过 PLIC）/ SMP support via PLIC

## 开发指南 / Development Guide

### 添加新的 RISC-V 特性 / Adding New RISC-V Features

1. 在 `minix/kernel/arch/riscv64/` 添加内核代码  
   Add kernel code under `minix/kernel/arch/riscv64/`
2. 更新 `minix/lib/libc/arch/riscv64/` 的 C 库支持  
   Update C library support under `minix/lib/libc/arch/riscv64/`
3. 添加相应的测试到 `minix/tests/riscv64/`  
   Add tests under `minix/tests/riscv64/`

### 调试技巧 / Debugging Tips

1. 使用 `-d` 参数启动 QEMU 调试模式  
   Start QEMU with `-d`
2. GDB 可以连接到 QEMU 的 GDB 服务器  
   GDB can attach to QEMU’s GDB server
3. 查看内核日志：`cat /proc/kmsg`  
   Read kernel log: `cat /proc/kmsg`

## 问题反馈与变更记录 / Issue Reporting and Change Log

1. 新问题：在 `issue.md` 记录症状、复现步骤、日志片段、涉及文件与行号、影响范围  
   New issues: record symptoms, repro steps, logs, files/lines, and impact in `issue.md`
2. 修复后：同步更新 `RISC64-STATUS.md` 的状态与风险评估  
   After fixes: update status and risk assessment in `RISC64-STATUS.md`
3. 关键变更：补充到 `docs/RISCV64_PORTING_GUIDE.md` 的移植步骤与验证记录  
   Key changes: add porting steps and verification notes to `docs/RISCV64_PORTING_GUIDE.md`

## 参考资料 / References

- [RISC-V 规范](https://riscv.org/technical/specifications/)
- [SBI 规范](https://github.com/riscv-non-isa/riscv-sbi-doc)
- [QEMU RISC-V 支持](https://www.qemu.org/docs/master/system/riscv/riscv-virt.html)
- [MINIX 3 文档](http://www.minix3.org/docs/)

## 许可证 / License

MINIX 3 遵循 BSD 许可证。详见源码中的 LICENSE 文件。  
MINIX 3 is licensed under BSD. See LICENSE in the source tree.

---

**最后更新 / Last updated**：2026-01-04  
**版本 / Version**：0.3
