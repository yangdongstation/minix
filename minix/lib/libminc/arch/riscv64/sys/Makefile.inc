# Makefile for RISC-V 64 libc system call stubs

.PATH: ${.CURDIR}/sys

SRCS+= \
	_ipc.S \
	_do_kernel_call_intr.S \
	ipc_minix_kerninfo.S \
	__sigreturn.S \
	ucontext.S \
	brksize.S
