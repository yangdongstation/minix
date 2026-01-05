PROG=	sysenv
BINDIR=	/bin
SRCS=	sysenv.c gp.c
LDFLAGS+= -Wl,--defsym,__global_pointer$$=_gp

.include <bsd.prog.mk>
