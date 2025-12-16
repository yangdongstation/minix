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
```

### 4. 构建完整系统

```bash
# 构建 MINIX distribution（需要 2-4 小时）
MKPCI=no HOST_CFLAGS="-O -fcommon" HAVE_GOLD=no HAVE_LLVM=no MKLLVM=no \
./build.sh -U -m evbriscv64 distribution
```

## 已知问题与解决方案

### 1. LLVM 编译问题

**问题**：LLVM 在 RISC-V 64-bit 上编译失败
**解决**：使用 `HAVE_LLVM=no MKLLVM=no` 跳过 LLVM 构建

### 2. 缺失的架构支持文件

以下文件需要为 RISC-V 64-bit 创建：

- `minix/lib/libminc/arch/riscv64/Makefile.libc.inc`
- `minix/lib/libminc/arch/riscv64/sys/Makefile.inc`
- `minix/tests/arch/riscv64/Makefile.inc`

### 3. 驱动程序支持

需要为 RISC-V 64-bit 创建以下驱动架构文件：

- `minix/drivers/clock/readclock/arch/riscv64/Makefile.inc`
- `minix/drivers/tty/tty/arch/riscv64/Makefile.inc`
- `minix/drivers/bus/i2c/arch/riscv64/Makefile.inc`
- `minix/drivers/video/fb/arch/riscv64/Makefile.inc`

### 4. C++ 库目录问题

在创建 distribution 时需要手动创建：
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
./minix/scripts/qemu-riscv64.sh -k minix/kernel/arch/riscv64/kernel

# 调试模式
./minix/scripts/qemu-riscv64.sh -d -k minix/kernel/arch/riscv64/kernel

# 使用 GDB 调试
./minix/scripts/gdb-riscv64.sh minix/kernel/arch/riscv64/kernel
```

## 构建输出

成功构建后，你将获得：

1. **交叉编译工具链**：`obj/tooldir.*/bin/riscv64-elf32-minix-*`
2. **内核镜像**：`minix/kernel/arch/riscv64/kernel`
3. **系统库**：`obj/destdir.evbriscv64/usr/lib/`
4. **服务器程序**：`obj/destdir.evbriscv64/sbin/`
5. **用户程序**：`obj/destdir.evbriscv64/usr/bin/`

## 性能优化

### 编译优化标志

系统使用以下 RISC-V 特定优化：
- `-march=rv64gc` - 通用 RISC-V 64-bit 扩展
- `-mabi=lp64d` - LP64 数据模型，双精度浮点

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

**最后更新**：2025年12月
**版本**：0.1 初始版本