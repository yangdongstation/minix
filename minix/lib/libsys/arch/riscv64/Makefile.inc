# Makefile for arch-dependent libsys code (RISC-V 64)
.include <bsd.own.mk>

HERE=${.CURDIR}/arch/${MACHINE_ARCH}
.PATH:  ${HERE}

SRCS+=  \
	get_randomness.c \
	getidle.c \
	ser_putc.c \
	spin.c \
	sys_sdevio.c \
	sys_umap_remote.c \
	tsc_util.c

CPPFLAGS+=	-I${HERE}/../../
