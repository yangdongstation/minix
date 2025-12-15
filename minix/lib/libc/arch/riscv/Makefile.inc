# Makefile for RISC-V libc architecture support

.PATH: ${.CURDIR}/arch/riscv

# Architecture-specific C sources
SRCS+= \
	_cpufeature.c

# Architecture-specific assembly
SRCS+= \
	get_bp.S \
	read_tsc.S

# Include system call stubs
.include "${.CURDIR}/arch/riscv/sys/Makefile.inc"
