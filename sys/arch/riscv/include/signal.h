/*	$NetBSD$	*/

#ifndef _RISCV_SIGNAL_H_
#define _RISCV_SIGNAL_H_

#include <sys/featuretest.h>

typedef int sig_atomic_t;

#if defined(_NETBSD_SOURCE)

#include <machine/reg.h>

/*
 * Information pushed on stack when a signal is delivered.
 * This is used by the kernel to restore state following
 * execution of the signal handler.  It is also made available
 * to the handler to allow it to restore state properly if
 * a non-standard exit is performed.
 */

#define __HAVE_STRUCT_SIGCONTEXT

struct sigcontext {
	int		sc_onstack;	/* saved onstack flag */
	int		__sc_mask13;	/* saved signal mask (old style) */
	struct reg	sc_regs;	/* saved registers */
	struct fpreg	sc_fpregs;	/* saved FP registers */
	long		sc_mask;	/* saved signal mask (new style) */
};

#endif /* _NETBSD_SOURCE */

#endif /* _RISCV_SIGNAL_H_ */
