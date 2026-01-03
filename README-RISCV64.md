# MINIX 3 RISC-V 64-bit 移植指南

## 概述

本文档记录了 MINIX 3 操作系统在 RISC-V 64-bit 架构上的移植工作。MINIX 3 是一个基于微内核的操作系统，本移植版本针对 QEMU virt 平台。

## 系统要求

### 主机系统
- Linux x86_64
- 至少 8GB RAM
- 20GB 可用磁盘空间
- GCC 交叉编译工具链支持

### 目标平台
- RISC-V 64-bit (RV64GC)
- QEMU virt 平台
- 支持 SBI (Supervisor Binary Interface)

## 构建步骤

### 1. 准备交叉编译工具链

```bash
# 安装 RISC-V GCC 工具链（Ubuntu/Debian）
sudo apt-get install gcc-riscv64-unknown-elf

# 或从源码构建
git clone https://github.com/riscv/riscv-gnu-toolchain
cd riscv-gnu-toolchain
./configure --prefix=/opt/riscv --enable-multilib
make
```

### 2. 配置构建环境

```bash
cd /path/to/minix

# 设置构建变量
export TOOLDIR=/path/to/minix/obj/tooldir.$(uname -s)-$(uname -r)-$(uname -m)
export DESTDIR=/path/to/minix/obj/destdir.evbriscv64
```

### 3. 构建交叉编译工具

```bash
# 构建主机工具（需要较长时间）
MKPCI=no HOST_CFLAGS="-O -fcommon" HAVE_GOLD=no ./build.sh -U -m evbriscv64 tools

# 如需强制使用 GCC（避免自动选择 clang）
MKPCI=no HOST_CFLAGS="-O -fcommon" HAVE_GOLD=no \
./build.sh -U -m evbriscv64 \
  -V AVAILABLE_COMPILER=gcc -V ACTIVE_CC=gcc -V ACTIVE_CPP=gcc -V ACTIVE_CXX=gcc -V ACTIVE_OBJC=gcc \
  tools
```

### 4. 构建完整系统

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

说明：
- 若交叉编译器支持 `-march=rv64gc -mabi=lp64d`，可移除 `RISCV_ARCH_FLAGS`。
- `CHECKFLIST_FLAGS='-m -e'` 会允许缺失/多余文件，适用于当前不完整的 sets；若需严格检查，请移除该标志并恢复 `MKPIC/MKCXX/MKATF`。

## 本次移植与验证步骤（evbriscv64）

### 1. CSU 移植（让 lib/csu 可编译）
- 在 `lib/csu/arch/riscv64/` 增加 riscv64 入口与 Makefile 包装层，复用 `lib/csu/arch/riscv/*` 实现。
- 在 `lib/csu/arch/riscv64/machine/asm.h` 与 `lib/csu/arch/riscv/riscv64/machine/asm.h` 补齐 `PTR_SCALESHIFT`，避免未安装系统头时缺宏。

### 2. CSU 编译验证

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

### 3. 工具链可用性验证

```bash
obj/tooldir.*/bin/riscv64-elf32-minix-gcc -dumpmachine
# 期望输出：riscv64-elf32-minix
```

### 4. 完整构建（跳过 libobjc/libgomp/C++）

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

说明：
- `MKLIBOBJC=no` 与 `MKLIBGOMP=no` 用于避开 `pthread.h` 未齐全导致的构建失败。
- `NOGCCERROR=yes` 与 libevent 的版本化警告屏蔽避免旧 GCC 不识别 `-Wno-error=...`。

### 5. 测试与当前结果

```bash
./minix/tests/riscv64/run_tests.sh all
```

当前结果：
- 用户态编译测试：全部通过（脚本已自动使用 in-tree toolchain + sysroot，并统一 `-std=gnu99`）。
- 内核启动测试：失败，QEMU 中出现 `rv64: kernel_main` 后触发 `System reset...`，详见 `/tmp/boot_test.log`。

#### 5.1 内核启动复位排查记录

1. 执行测试：`./minix/tests/riscv64/run_tests.sh all`
2. 查看日志：`/tmp/boot_test.log`
   - 可见 OpenSBI banner
   - 内核初始化输出：`rv64: kernel_main`、`rv64: kmain entry`、`rv64: mods_count=0`
   - 随后出现 `System reset...`，OpenSBI banner 重复
3. 结论：内核已进入早期初始化，但在继续启动过程中触发复位（原因待进一步定位）
4. 初步排查建议：
   - 关注是否有显式复位/关机路径被触发（`minix/kernel/arch/riscv64/sbi.c`、`minix/kernel/arch/riscv64/exception.c`）
   - 在 `rv64:` 打点附近继续细化日志，定位复位发生点
   - 若需要交互调试，使用 `minix/scripts/qemu-riscv64.sh -d -k minix/kernel/obj/kernel -B obj/destdir.evbriscv64`

#### 5.2 测试脚本启动判定说明

`run_tests.sh` 的内核启动测试通过以下条件判断成功：

```bash
grep -q "MINIX" /tmp/boot_test.log
```

当前内核日志未输出 `MINIX` 字符串，因此该用例会失败。可选处理方式：

- 将判定条件调整为当前已有的启动标记（例如 `rv64: kernel_main` 或 `rv64: kmain entry`）
- 或者在内核早期输出包含 `MINIX` 的 banner

## 已知问题与解决方案

### 1. LLVM 编译问题

**问题**：LLVM 在 RISC-V 64-bit 上编译失败
**解决**：使用 `HAVE_LLVM=no MKLLVM=no` 跳过 LLVM 构建

### 2. 交叉编译器 `-march=rv64gc` 不兼容

**问题**：部分交叉编译器/本地 tooldir 的 GCC 不接受 `-march=rv64gc`
**解决**：使用 `-V RISCV_ARCH_FLAGS='-march=RV64IMAFD -mcmodel=medany'`

### 3. distribution 清单检查失败

**问题**：`checkflist` 报告缺失/多余文件，导致 distribution 失败
**解决**：在移植进行中可用 `-V CHECKFLIST_FLAGS='-m -e'` 临时放宽；要严格校验需恢复 `MKPIC/MKCXX/MKATF` 并补齐 sets

### 4. C++ 库目录问题（开启 C++ 时）

若启用 `MKCXX/MKLIBSTDCXX`，创建 distribution 时可能需要手动创建：
```bash
mkdir -p $DESTDIR/usr/include/g++/bits/riscv32
mkdir -p $DESTDIR/usr/include/g++/bits/riscv64
```

## 架构特定实现

### 内存映射（QEMU virt 平台）

| 地址范围       | 设备          | 描述                   |
|----------------|---------------|------------------------|
| 0x02000000     | CLINT         | Core Local Interruptor |
| 0x0C000000     | PLIC          | Platform-Level Interrupt Controller |
| 0x10000000     | UART          | NS16550 串口          |
| 0x10001000     | VirtIO MMIO   | VirtIO 设备           |
| 0x80000000     | RAM           | 物理内存起始           |

### SBI 接口使用

系统使用 SBI 进行：
- 控制台输出
- 定时器管理
- 系统关闭

## 测试

### 运行 RISC-V 特定测试

```bash
cd minix/tests/riscv64
./run_tests.sh all
```

可用的测试类别：
- `csr` - 控制和状态寄存器
- `atomic` - 原子操作
- `sbi` - SBI 接口调用
- `memory` - 内存管理
- `trap` - 陷阱处理
- `timer` - 定时器和 CLINT
- `ipc` - 进程间通信
- `vm` - 虚拟内存

### 使用 QEMU 运行

```bash
# 基本运行
./minix/scripts/qemu-riscv64.sh -k minix/kernel/obj/kernel -B obj/destdir.evbriscv64

# 调试模式
./minix/scripts/qemu-riscv64.sh -d -k minix/kernel/obj/kernel -B obj/destdir.evbriscv64

# 使用 GDB 调试
./minix/scripts/gdb-riscv64.sh minix/kernel/obj/kernel
```

## 构建输出

成功构建后，你将获得：

1. **交叉编译工具链**：`obj/tooldir.*/bin/riscv64-elf32-minix-*`
2. **内核镜像**：`minix/kernel/obj/kernel`
3. **系统库**：`obj/destdir.evbriscv64/usr/lib/`
4. **服务器程序**：`obj/destdir.evbriscv64/sbin/`
5. **用户程序**：`obj/destdir.evbriscv64/usr/bin/`

## 性能优化

### 编译优化标志

系统使用以下 RISC-V 特定优化：
- `-march=rv64gc` - 通用 RISC-V 64-bit 扩展（工具链支持时）
- `-mabi=lp64d` - LP64 数据模型，双精度浮点
- 兼容 fallback：`-march=RV64IMAFD -mcmodel=medany`

### 内核优化

- 使用内存屏障确保内存访问顺序
- 优化中断处理路径
- 支持 SMP 多核（通过 PLIC）

## 开发指南

### 添加新的 RISC-V 特性

1. 在 `minix/kernel/arch/riscv64/` 添加内核代码
2. 更新 `minix/lib/libc/arch/riscv64/` 的 C 库支持
3. 添加相应的测试到 `minix/tests/riscv64/`

### 调试技巧

1. 使用 `-d` 参数启动 QEMU 调试模式
2. GDB 可以连接到 QEMU 的 GDB 服务器
3. 查看内核日志：`cat /proc/kmsg`

## 贡献

欢迎为 MINIX RISC-V 64-bit 移植做出贡献。请：

1. 检查现有的 issue 和 pull request
2. 确保代码符合 MINIX 编码规范
3. 添加适当的测试
4. 更新文档

## 参考资料

- [RISC-V 规范](https://riscv.org/technical/specifications/)
- [SBI 规范](https://github.com/riscv-non-isa/riscv-sbi-doc)
- [QEMU RISC-V 支持](https://www.qemu.org/docs/master/system/riscv/riscv-virt.html)
- [MINIX 3 文档](http://www.minix3.org/docs/)

## 致谢

感谢所有为 MINIX RISC-V 移植做出贡献的开发者。特别感谢：

- RISC-V 国际基金会的规范工作
- QEMU 团队的 RISC-V 支持
- MINIX 社区的持续维护

## 许可证

MINIX 3 遵循 BSD 许可证。详见源码中的 LICENSE 文件。

---

**最后更新**：2026年1月
**版本**：0.2
