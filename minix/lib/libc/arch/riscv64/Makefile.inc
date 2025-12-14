# Makefile for RISC-V 64 libc architecture support

.PATH: ${.CURDIR}/arch/riscv64

# Architecture-specific C sources
SRCS+= \
	_cpufeature.c

# Architecture-specific assembly
SRCS+= \
	get_bp.S \
	read_tsc.S

# Include system call stubs
.include "${.CURDIR}/arch/riscv64/sys/Makefile.inc"
