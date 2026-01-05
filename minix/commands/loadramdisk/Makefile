PROG=	loadramdisk
MAN=	loadramdisk.8
SRCS=	loadramdisk.c gp.c
LDFLAGS+= -Wl,--defsym,__global_pointer$$=_gp

.include <bsd.prog.mk>
