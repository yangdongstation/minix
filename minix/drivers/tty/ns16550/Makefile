# Makefile for NS16550 UART driver

PROG=   ns16550
SRCS=   ns16550.c

DPADD+= ${LIBCHARDRIVER} ${LIBSYS}
LDADD+= -lchardriver -lsys

MAN=

.include <bsd.prog.mk>
