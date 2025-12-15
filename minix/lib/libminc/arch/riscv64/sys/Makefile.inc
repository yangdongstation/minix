# Makefile for RISC-V libminc system call stubs

.PATH: ${.CURDIR}/sys

# Minimal system call stubs for RISC-V
SRCS+= \
	_ipc.S \
	_do_kernel_call_intr.S \
	ipc_minix_kerninfo.S \
	__sigreturn.S \
	ucontext.S