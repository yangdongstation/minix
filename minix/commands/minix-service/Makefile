# Makefile for minix-service
PROGS=	minix-service printconfig
SRCS.minix-service=minix-service.c parse.c util.c gp.c
SRCS.printconfig=print.c parse.c util.c gp.c
LDFLAGS+= -Wl,--defsym,__global_pointer$$=_gp

BINDIR= /sbin
MAN.minix-service=	minix-service.8 system.conf.5
MAN.printconfig=

.include <bsd.prog.mk>
