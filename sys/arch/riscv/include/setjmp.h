/*	$NetBSD$	*/

#ifndef _RISCV_SETJMP_H_
#define _RISCV_SETJMP_H_

/*
 * machine/setjmp.h: machine dependent setjmp-related information.
 */

#define _JBLEN		22	/* size, in longs, of a jmp_buf */
				/* ra, sp, s0-s11, fs0-fs11, fcsr, sigmask */

#endif /* _RISCV_SETJMP_H_ */
