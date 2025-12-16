# Makefile for arch-dependent i2c code
.include <bsd.own.mk>

HERE=${.CURDIR}/arch/${MACHINE_ARCH}
.PATH:  ${HERE}

# RISC-V platforms typically don't use PCI-based I2C controllers
# Add platform-specific I2C controller drivers here as needed
# SRCS += riscv_i2c.c