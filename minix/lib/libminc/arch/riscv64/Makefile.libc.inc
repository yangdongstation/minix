########################################################################
#
# Shared libc with userspace (/common/lib/libc) - RISC-V 64-bit version
#
.for f in \
	strlcpy.o strncpy.o
${f} ${f:C/\.o/.bc/}:	${LIBCOMMONCDIR}/string/${f:C/\.o/.c/}
${f} ${f:C/\.o/.go/}:	${LIBCOMMONCDIR}/string/${f:C/\.o/.c/}
OBJS+=	${f}
CLEANFILES+= ${f}

.if ${USE_BITCODE:Uno} == "yes"
OBJS+=	${f:C/\.o/.bc/}
CLEANFILES+= ${f:C/\.o/.bc/}
.endif # ${USE_BITCODE:Uno} == "yes"
.endfor

CPPFLAGS.strlcpy.c+=		-D_LIBC

.for f in \
	memchr.o memcmp.o memcpy.o memmove.o memset.o \
	strcat.o strchr.o strcmp.o strcpy.o strlen.o \
	strrchr.o strncmp.o
${f} ${f:C/\.o/.bc/}:	${LIBCOMMONCARCHDIR}/string/${f:C/\.o/.S/}
${f} ${f:C/\.o/.go/}:	${LIBCOMMONCARCHDIR}/string/${f:C/\.o/.S/}
OBJS+=	${f}
CLEANFILES+= ${f}

.if ${USE_BITCODE:Uno} == "yes"
OBJS+=	${f:C/\.o/.bc/}
CLEANFILES+= ${f:C/\.o/.bc/}
.endif # ${USE_BITCODE:Uno} == "yes"
.endfor

########################################################################
#
# Functions imported directly from libc.
#
.for f in \
	alloca.o
${f} ${f:C/\.o/.bc/}:	${LIBCARCHDIR}/gen/${f:C/\.o/.S/}
${f} ${f:C/\.o/.go/}:	${LIBCARCHDIR}/gen/${f:C/\.o/.S/}
OBJS+=	${f}
CLEANFILES+= ${f}

.if ${USE_BITCODE:Uno} == "yes"
OBJS+=	${f:C/\.o/.bc/}
CLEANFILES+= ${f:C/\.o/.bc/}
.endif # ${USE_BITCODE:Uno} == "yes"
.endfor