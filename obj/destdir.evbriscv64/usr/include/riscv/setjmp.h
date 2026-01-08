/*	$NetBSD$	*/

#ifndef _RISCV_SETJMP_H_
#define _RISCV_SETJMP_H_

/*
 * machine/setjmp.h: machine dependent setjmp-related information.
 */

#define _JBLEN		32	/* size, in longs, of a jmp_buf */
				/* ra, sp, s0-s11, fs0-fs11, fcsr, sigmask */

typedef long		_BSD_JBSLOT_T_;

/* jmp_buf indices */
#define _JB_MAGIC	0
#define _JB_RA		1
#define _JB_SP		2
#define _JB_GP		3
#define _JB_TP		4
#define _JB_S0		5
#define _JB_S1		6
#define _JB_S2		7
#define _JB_S3		8
#define _JB_S4		9
#define _JB_S5		10
#define _JB_S6		11
#define _JB_S7		12
#define _JB_S8		13
#define _JB_S9		14
#define _JB_S10		15
#define _JB_S11		16
#define _JB_FCSR	17
#define _JB_FS0		18
#define _JB_FS1		19
#define _JB_FS2		20
#define _JB_FS3		21
#define _JB_FS4		22
#define _JB_FS5		23
#define _JB_FS6		24
#define _JB_FS7		25
#define _JB_FS8		26
#define _JB_FS9		27
#define _JB_FS10	28
#define _JB_FS11	29
#define _JB_SIGMASK	30

#endif /* _RISCV_SETJMP_H_ */
