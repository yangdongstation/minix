#	$NetBSD: Makefile,v 1.8 1997/07/20 22:37:09 christos Exp $
#	@(#)Makefile	8.1 (Berkeley) 5/31/93

PROG=	kill
SRCS=	kill.c gp.c
LDFLAGS+= -Wl,--defsym,__global_pointer$$=_gp

.include <bsd.prog.mk>
