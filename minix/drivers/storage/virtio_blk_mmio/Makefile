# Makefile for VirtIO block driver (MMIO)

PROG=   virtio_blk_mmio
SRCS=   virtio_blk_mmio.c

DPADD+= ${LIBBLOCKDRIVER} ${LIBVIRTIO_MMIO} ${LIBSYS}
LDADD+= -lblockdriver -lvirtio_mmio -lsys

MAN=

.include <bsd.prog.mk>
