.include <bsd.own.mk>

# Makefile for Reincarnation Server (RS)
PROG=	rs
SRCS=	exec.c main.c request.c manager.c table.c utility.c error.c update.c \
	gp.c

.if ${USE_PCI} != "no"
CPPFLAGS+= -DUSE_PCI
.endif

.if ${USE_PCI} != "no"
CPPFLAGS+= -DUSE_PCI
.endif

.if ${MACHINE_ARCH} == "riscv64" || ${MACHINE_ARCH} == "riscv"
LDFLAGS+= -Wl,--defsym,__global_pointer$$=_gp
.endif

DPADD+=	${LIBSYS} ${LIBEXEC}
LDADD+=	-lsys -lexec

CPPFLAGS+=	-I${NETBSDSRCDIR}/minix

.include <minix.service.mk>
