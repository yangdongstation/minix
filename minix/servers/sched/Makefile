# Makefile for Scheduler (SCHED)
PROG=	sched
SRCS=	main.c schedule.c utility.c gp.c

DPADD+=	${LIBSYS}
LDADD+=	-lsys

.if ${MACHINE_ARCH} == "riscv64" || ${MACHINE_ARCH} == "riscv"
LDFLAGS+= -Wl,--defsym,__global_pointer$$=_gp
.endif

.include <minix.service.mk>
