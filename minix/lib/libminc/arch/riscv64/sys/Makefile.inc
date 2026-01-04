# Makefile for RISC-V 64 libc system call stubs

.PATH: ${.PARSEDIR}

TMP=ucontextoffsets.h.tmp
CF=${NETBSDSRCDIR}/minix/lib/libc/arch/${MACHINE_ARCH}/sys/ucontextoffsets.cf

ucontext.o: ucontextoffsets.h

SRCS+= \
	_ipc.S \
	_do_kernel_call_intr.S \
	ipc_minix_kerninfo.S \
	__sigreturn.S \
	ucontext.S \
	brksize.S

ucontextoffsets.h: ${CF}
ucontextoffsets.h: ${NETBSDSRCDIR}/sys/sys/ucontext.h
ucontextoffsets.h: ${NETBSDSRCDIR}/minix/include/arch/${MACHINE_ARCH}/include/stackframe.h
ucontextoffsets.h:
	${_MKTARGET_CREATE}
	${TOOL_CAT} ${CF} | \
		${TOOL_GENASSYM} -- ${CC} ${CFLAGS:N-Wa,*} \
		${CPPFLAGS} ${PROF} ${GENASSYM_CPPFLAGS} >$TMP && \
	mv -f $TMP $@
