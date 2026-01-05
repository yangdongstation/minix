FSCK=   ${NETBSDSRCDIR}/sbin/fsck
PROG=	fsck_mfs
SRCS=	fsck.c gp.c
CPPFLAGS+= -I${NETBSDSRCDIR}/minix/fs -I${FSCK}
BINDIR=	/sbin
MAN=	fsck.mfs.1
LDFLAGS+= -Wl,--defsym,__global_pointer$$=_gp

.include <bsd.prog.mk>
