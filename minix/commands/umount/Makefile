PROG=	umount
BINDIR=	/bin
SRCS=	umount.c gp.c
LDFLAGS+= -Wl,--defsym,__global_pointer$$=_gp

.include <bsd.prog.mk>
