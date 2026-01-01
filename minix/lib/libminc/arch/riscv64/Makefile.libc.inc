########################################################################
#
# Shared libc with userspace (/common/lib/libc)
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

.if exists(${LIBCOMMONCARCHDIR}/gen/byte_swap_2.S)
.for f in \
	byte_swap_2.o byte_swap_4.o
${f} ${f:C/\.o/.bc/}:	${LIBCOMMONCARCHDIR}/gen/${f:C/\.o/.S/}
${f} ${f:C/\.o/.go/}:	${LIBCOMMONCARCHDIR}/gen/${f:C/\.o/.S/}
OBJS+=	${f}
CLEANFILES+= ${f}

.if ${USE_BITCODE:Uno} == "yes"
OBJS+=	${f:C/\.o/.bc/}
CLEANFILES+= ${f:C/\.o/.bc/}
.endif # ${USE_BITCODE:Uno} == "yes"
.endfor
.endif

.for f in \
	memchr.o memcmp.o memcpy.o memmove.o memset.o \
	strcat.o strchr.o strcmp.o strcpy.o strlen.o \
	strrchr.o
${f} ${f:C/\.o/.bc/}:	${LIBCOMMONCDIR}/string/${f:C/\.o/.c/}
${f} ${f:C/\.o/.go/}:	${LIBCOMMONCDIR}/string/${f:C/\.o/.c/}
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
.if exists(${LIBCARCHDIR}/gen/alloca.S)
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
.endif

.for f in \
	strncmp.o
${f} ${f:C/\.o/.bc/}:	${LIBCOMMONCDIR}/string/${f:C/\.o/.c/}
${f} ${f:C/\.o/.go/}:	${LIBCOMMONCDIR}/string/${f:C/\.o/.c/}
OBJS+=	${f}
CLEANFILES+= ${f}

.if ${USE_BITCODE:Uno} == "yes"
OBJS+=	${f:C/\.o/.bc/}
CLEANFILES+= ${f:C/\.o/.bc/}
.endif # ${USE_BITCODE:Uno} == "yes"
.endfor
