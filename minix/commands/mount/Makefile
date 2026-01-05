PROG=	mount
BINDIR=	/bin
SRCS=	mount.c gp.c
LDFLAGS+= -Wl,--defsym,__global_pointer$$=_gp

.include <bsd.prog.mk>
