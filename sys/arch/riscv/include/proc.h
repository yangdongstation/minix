/*	$NetBSD$	*/

/*
 * Machine-dependent part of the LWP and proc structures for RISC-V.
 */

#ifndef _RISCV_PROC_H_
#define _RISCV_PROC_H_

#include <machine/frame.h>

struct lwp;

struct mdlwp {
	struct trapframe *md_tf;
	int md_flags;
};

struct mdproc {
	void (*md_syscall)(struct trapframe *, struct lwp *, register_t);
};

#endif /* _RISCV_PROC_H_ */
