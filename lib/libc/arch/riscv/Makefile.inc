#	$NetBSD: Makefile.inc,v 1.1 2014/09/19 17:36:25 matt Exp $

.if ${RUMPRUN} != "yes" && !defined(__MINIX)
SRCS+=		__sigaction14_sigtramp.c __sigtramp2.S
.endif

CPPFLAGS+=	-I.

.if ${MKSOFTFLOAT} != "no"
.include <softfloat/Makefile.inc>
.endif
