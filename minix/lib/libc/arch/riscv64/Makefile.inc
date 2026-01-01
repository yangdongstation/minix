# Makefile for RISC-V 64 libc architecture support

ARCHSUBDIR?= ${MACHINE_CPU}
C_HERE=	${NETBSDSRCDIR}/minix/lib/libc/arch/${ARCHSUBDIR}
.PATH: ${C_HERE}

# Architecture-specific C sources
SRCS+= \
	_cpufeature.c

# Architecture-specific assembly
SRCS+= \
	get_bp.S \
	read_tsc.S

# Include system call stubs
.include "${C_HERE}/sys/Makefile.inc"
