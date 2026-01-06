# Makefile for VirtIO block driver (MMIO)

PROG=	virtio_blk_mmio
SRCS=	virtio_blk_mmio.c

DPADD+=	${LIBBLOCKDRIVER} ${LIBVIRTIO_MMIO} ${LIBSYS} ${LIBMTHREAD}
LDADD+=	-lblockdriver -lvirtio_mmio -lsys -lmthread

.if ${MACHINE_ARCH} == "riscv64" || ${MACHINE_ARCH} == "riscv"
LDFLAGS+= -Wl,--defsym,__global_pointer$$=_gp
.endif

.include <minix.service.mk>
