/*	$NetBSD$	*/

#ifndef _RISCV_FRAME_H_
#define _RISCV_FRAME_H_

#include <machine/types.h>

/*
 * RISC-V trap frame
 */
struct trapframe {
	register_t tf_ra;	/* x1  - return address */
	register_t tf_sp;	/* x2  - stack pointer */
	register_t tf_gp;	/* x3  - global pointer */
	register_t tf_tp;	/* x4  - thread pointer */
	register_t tf_t0;	/* x5  - temporary 0 */
	register_t tf_t1;	/* x6  - temporary 1 */
	register_t tf_t2;	/* x7  - temporary 2 */
	register_t tf_s0;	/* x8  - saved register 0 / frame pointer */
	register_t tf_s1;	/* x9  - saved register 1 */
	register_t tf_a0;	/* x10 - argument 0 / return value 0 */
	register_t tf_a1;	/* x11 - argument 1 / return value 1 */
	register_t tf_a2;	/* x12 - argument 2 */
	register_t tf_a3;	/* x13 - argument 3 */
	register_t tf_a4;	/* x14 - argument 4 */
	register_t tf_a5;	/* x15 - argument 5 */
	register_t tf_a6;	/* x16 - argument 6 */
	register_t tf_a7;	/* x17 - argument 7 */
	register_t tf_s2;	/* x18 - saved register 2 */
	register_t tf_s3;	/* x19 - saved register 3 */
	register_t tf_s4;	/* x20 - saved register 4 */
	register_t tf_s5;	/* x21 - saved register 5 */
	register_t tf_s6;	/* x22 - saved register 6 */
	register_t tf_s7;	/* x23 - saved register 7 */
	register_t tf_s8;	/* x24 - saved register 8 */
	register_t tf_s9;	/* x25 - saved register 9 */
	register_t tf_s10;	/* x26 - saved register 10 */
	register_t tf_s11;	/* x27 - saved register 11 */
	register_t tf_t3;	/* x28 - temporary 3 */
	register_t tf_t4;	/* x29 - temporary 4 */
	register_t tf_t5;	/* x30 - temporary 5 */
	register_t tf_t6;	/* x31 - temporary 6 */
	register_t tf_sepc;	/* exception program counter */
	register_t tf_sstatus;	/* status register */
	register_t tf_scause;	/* trap cause */
	register_t tf_stval;	/* trap value */
};

#endif /* _RISCV_FRAME_H_ */
