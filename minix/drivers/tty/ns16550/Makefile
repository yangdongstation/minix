# Makefile for NS16550 UART driver

PROG=   ns16550
SRCS=   ns16550.c gp.c

DPADD+= ${LIBCHARDRIVER} ${LIBSYS}
LDADD+= -lchardriver -lsys
LDFLAGS+= -Wl,--defsym,__global_pointer$$=_gp

MAN=

.include <bsd.prog.mk>
