# Makefile for the input server

PROG=	input
SRCS=	input.c gp.c
LDFLAGS+= -Wl,--defsym,__global_pointer$$=_gp

DPADD+=	${LIBCHARDRIVER} ${LIBSYS}
LDADD+=	-lchardriver -lsys

.include <minix.service.mk>
