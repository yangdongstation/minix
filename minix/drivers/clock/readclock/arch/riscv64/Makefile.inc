# Makefile for arch-dependent readclock code
.include <bsd.own.mk>

HERE=${.CURDIR}/arch/${MACHINE_ARCH}
.PATH:  ${HERE}

SRCS += arch_readclock.cSRCS += arch_readclock.c
