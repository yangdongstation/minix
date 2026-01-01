# Minimal MINIX RISC-V 64 Kernel Makefile

CROSS_COMPILE = riscv64-unknown-elf-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

# Compiler flags
CFLAGS = -march=rv64imafdc -mabi=lp64d -mcmodel=medany
CFLAGS += -ffreestanding -nostdlib -fno-builtin -fno-stack-protector
CFLAGS += -O2 -g -Wall
CFLAGS += -D__kernel__ -D_MINIX_SYSTEM

# Include paths
INCLUDES = -I. -Iinclude -Iarch/riscv64 -Iarch/riscv64/include

# Linker script
LDFLAGS = -T arch/riscv64/kernel.lds -nostdlib

# Source files
ARCH_C_SRCS = \
	arch/riscv64/kernel.c \
	arch/riscv64/pre_init.c \
	arch/riscv64/arch_system.c \
	arch/riscv64/memory.c \
	arch/riscv64/exception.c \
	arch/riscv64/hw_intr.c \
	arch/riscv64/plic.c \
	arch/riscv64/sbi.c \
	arch/riscv64/console.c \
	arch/riscv64/direct_tty_utils.c \
	arch/riscv64/bsp_init.c \
	arch/riscv64/bsp_serial.c \
	arch/riscv64/bsp_timer.c \
	arch/riscv64/bsp_reset.c \
	arch/riscv64/bsp_intr.c

ARCH_S_SRCS = \
	arch/riscv64/head.S \
	arch/riscv64/klib.S \
	arch/riscv64/mpx.S \
	arch/riscv64/exc.S \
	arch/riscv64/phys_copy.S \
	arch/riscv64/phys_memset.S

KERNEL_SRCS = \
	main.c \
	system.c \
	proc.c \
	table.c \
	utility.c \
	clock.c \
	interrupt.c

ARCH_C_OBJS = $(ARCH_C_SRCS:.c=.o)
ARCH_S_OBJS = $(ARCH_S_SRCS:.S=.o)
KERNEL_OBJS = $(KERNEL_SRCS:.c=.o)

OBJS = $(ARCH_C_OBJS) $(ARCH_S_OBJS) $(KERNEL_OBJS)

# Default target
all: kernel.elf kernel.bin

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile assembly files
%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Link kernel
kernel.elf: $(OBJS) arch/riscv64/kernel.lds
	$(LD) $(LDFLAGS) $(OBJS) -o $@

# Create binary image
kernel.bin: kernel.elf
	$(OBJCOPY) -O binary $< $@

# Disassembly for debugging
kernel.dis: kernel.elf
	$(OBJDUMP) -D $< > $@

# Clean
clean:
	rm -f $(OBJS) kernel.elf kernel.bin kernel.dis

.PHONY: all clean