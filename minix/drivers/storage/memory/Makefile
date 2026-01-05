# Makefile for memory driver (MEMORY)
.include <bsd.own.mk>

USE_BITCODE:=no

PROG=	memory
SRCS=	memory.c imgrd.mfs gp.c
OBJS=	${SRCS:N*.h:R:S/$/.o/g}
MKBUILDEXT2RD?=	no

RAMDISK_PATH= ${NETBSDSRCDIR}/minix/drivers/storage/ramdisk
.if ${.OBJDIR:M*/obj}
RAMDISK_IMAGE= ../../ramdisk/obj/image
.else
RAMDISK_IMAGE= ../ramdisk/image
.endif
DPADD+=	${LIBBLOCKDRIVER} ${LIBCHARDRIVER}
LDADD+=	-lblockdriver -lchardriver

CPPFLAGS.memory.c+=	-I${NETBSDSRCDIR}/minix

.if ${MACHINE_ARCH} == "riscv64" || ${MACHINE_ARCH} == "riscv"
LDFLAGS+= -Wl,--defsym,__global_pointer$$=_gp
.endif

imgrd.d: touch-genfiles
touch-genfiles:
	[ -e ${RAMDISK_IMAGE} ] || touch -t 197001020000.00 ${RAMDISK_IMAGE}


.SUFFIXES:      .mfs .c .o

.mfs.o:
	${_MKTARGET_CREATE}
	${OBJCOPY} -Ibinary -B${MACHINE_CPU} -O${MACHINE_GNU_PLATFORM} $< $@

CLEANFILES+=	${RAMDISK_IMAGE}
# BJG - don't invoke parallel Makes
#../ramdisk/image: .PHONY
#	${MAKE} -C ${RAMDISK_PATH} image

CLEANFILES+=	imgrd.mfs
imgrd.mfs: ${RAMDISK_IMAGE}
	${HOST_LN} -fs ${RAMDISK_IMAGE} $@

.include <minix.service.mk>
