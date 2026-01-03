# $NetBSD: Makefile,v 1.2 2013/11/09 00:26:06 christos Exp $

USE_SHLIBDIR=yes
.include <bsd.own.mk>

LIB=event

#__MINIX: Unsupported: kqueue.c
SRCS= \
buffer.c \
bufferevent.c \
bufferevent_filter.c \
bufferevent_pair.c \
bufferevent_ratelim.c \
bufferevent_sock.c \
evdns.c \
event.c \
event_tagging.c \
evmap.c \
evrpc.c \
evthread.c \
evutil.c \
evutil_rand.c \
http.c \
listener.c \
log.c \
poll.c \
select.c \
signal.c

.if ${ACTIVE_CC} == "gcc" && !defined(NOGCCERROR)
. if ${HAVE_GCC:U0} >= 7
CFLAGS+=	-Wno-error=builtin-declaration-mismatch -Wno-error=implicit-fallthrough
. endif
. if ${HAVE_GCC:U0} >= 8
CFLAGS+=	-Wno-error=array-parameter
. endif
. if ${HAVE_GCC:U0} >= 12
CFLAGS+=	-Wno-error=dangling-pointer
. endif
.endif

.if defined(HAVE_GCC) && ${HAVE_GCC} == 48
.if ${MACHINE_ARCH} == "sh3el"
COPTS.evdns.c=	-O0
.endif
.endif

.include <bsd.lib.mk>
