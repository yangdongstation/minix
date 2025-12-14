# Makefile for arch-dependent kernel code (RISC-V 64)
.include <bsd.own.mk>

HERE=${.CURDIR}/arch/${MACHINE_ARCH}
.PATH:	${HERE}

# objects we want unpaged from -lsys
SYS_OBJS_UNPAGED=assert.o stacktrace.o
assert.o:	${NETBSDSRCDIR}/minix/lib/libsys/assert.c
stacktrace.o:	${NETBSDSRCDIR}/minix/lib/libsys/stacktrace.c

# objects we want unpaged from -lminc
MINC_OBJS_UNPAGED= atoi.o \
	printf.o subr_prf.o \
	strcmp.o strcpy.o strlen.o strncmp.o \
	memcpy.o memmove.o memset.o
atoi.o:		${NETBSDSRCDIR}/minix/lib/libminc/atoi.c
printf.o:	${NETBSDSRCDIR}/sys/lib/libsa/printf.c
subr_prf.o:	${NETBSDSRCDIR}/sys/lib/libsa/subr_prf.c

# Use generic C implementations for string functions
memcpy.o:	${NETBSDSRCDIR}/common/lib/libc/string/memcpy.c
memmove.o:	${NETBSDSRCDIR}/common/lib/libc/string/memmove.c
memset.o:	${NETBSDSRCDIR}/common/lib/libc/string/memset.c
strlen.o:	${NETBSDSRCDIR}/common/lib/libc/string/strlen.c
strcpy.o:	${NETBSDSRCDIR}/common/lib/libc/string/strcpy.c
strcmp.o:	${NETBSDSRCDIR}/common/lib/libc/string/strcmp.c
strncmp.o:	${NETBSDSRCDIR}/common/lib/libc/string/strncmp.c

# LSC: putchar and kputc have the same signature. A bit hackish.
CPPFLAGS.subr_prf.c+= -Dputchar=kputc

# Activate optional support, may be deactivated.
CPPFLAGS.subr_prf.c+= -DLIBSA_PRINTF_LONGLONG_SUPPORT -DLIBSA_PRINTF_WIDTH_SUPPORT

# Board Support Package for QEMU virt platform
.include "bsp/virt/Makefile.inc"

# some object files we give a symbol prefix (or namespace) of __k_unpaged_
# that must live in their own unique namespace.
#
.for unpaged_obj in head.o pre_init.o direct_tty_utils.o \
	pg_utils.o klib.o utility.o arch_reset.o \
	${MINC_OBJS_UNPAGED} ${SYS_OBJS_UNPAGED} ${BSP_OBJS_UNPAGED}
unpaged_${unpaged_obj}: ${unpaged_obj}
	${OBJCOPY} --prefix-symbols=__k_unpaged_ ${.OBJDIR}/${unpaged_obj} $@
UNPAGED_OBJS += unpaged_${unpaged_obj}
ORIG_UNPAGED_OBJS += ${unpaged_obj}
.endfor

CLEANFILES+= ${ORIG_UNPAGED_OBJS}

# Core kernel source files
SRCS+=	head.S mpx.S klib.S exc.S
SRCS+=	arch_clock.c arch_do_vmctl.c arch_system.c
SRCS+=	exception.c hw_intr.c memory.c
SRCS+=	protect.c direct_tty_utils.c arch_reset.c
SRCS+=	pg_utils.c phys_copy.S phys_memset.S
SRCS+=	sbi.c plic.c

OBJS.kernel+=	${UNPAGED_OBJS}

klib.o mpx.o head.o: procoffsets.h

SRCS+= procoffsets.h

PROCOFFSETSCF=procoffsets.cf

.PATH: ${NETBSDSRCDIR}/minix/include/arch/${MACHINE_ARCH}/include

procoffsets.h: ${PROCOFFSETSCF} kernel.h proc.h stackframe.h archtypes.h
	${_MKTARGET_CREATE}
	${TOOL_CAT} ${HERE}/${PROCOFFSETSCF} | \
		${TOOL_GENASSYM} -- ${CC} ${CFLAGS:N-Wa,*} \
		${CPPFLAGS} ${PROF} ${${USE_BITCODE:Uno} == "yes":? -fno-lto:} \
		${GENASSYM_CPPFLAGS} >$@.tmp && \
	  mv -f $@.tmp $@

sconst.h: procoffsets.h
head.o klib.o mpx.o: sconst.h
