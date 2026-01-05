#	$NetBSD: Makefile,v 1.12 2003/05/18 07:57:31 lukem Exp $
#	@(#)Makefile	8.1 (Berkeley) 5/31/93

PROG=	cat
SRCS=	cat.c gp.c
LDFLAGS+= -Wl,--defsym,__global_pointer$$=_gp

.include <bsd.prog.mk>
