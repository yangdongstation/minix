/*	$NetBSD$	*/

#ifndef _RISCV_REG_H_
#define _RISCV_REG_H_

/*
 * General purpose registers
 */
struct reg {
	__register_t r_ra;	/* x1  - return address */
	__register_t r_sp;	/* x2  - stack pointer */
	__register_t r_gp;	/* x3  - global pointer */
	__register_t r_tp;	/* x4  - thread pointer */
	__register_t r_t0;	/* x5  - temporary 0 */
	__register_t r_t1;	/* x6  - temporary 1 */
	__register_t r_t2;	/* x7  - temporary 2 */
	__register_t r_s0;	/* x8  - saved register 0 / frame pointer */
	__register_t r_s1;	/* x9  - saved register 1 */
	__register_t r_a0;	/* x10 - argument 0 / return value 0 */
	__register_t r_a1;	/* x11 - argument 1 / return value 1 */
	__register_t r_a2;	/* x12 - argument 2 */
	__register_t r_a3;	/* x13 - argument 3 */
	__register_t r_a4;	/* x14 - argument 4 */
	__register_t r_a5;	/* x15 - argument 5 */
	__register_t r_a6;	/* x16 - argument 6 */
	__register_t r_a7;	/* x17 - argument 7 */
	__register_t r_s2;	/* x18 - saved register 2 */
	__register_t r_s3;	/* x19 - saved register 3 */
	__register_t r_s4;	/* x20 - saved register 4 */
	__register_t r_s5;	/* x21 - saved register 5 */
	__register_t r_s6;	/* x22 - saved register 6 */
	__register_t r_s7;	/* x23 - saved register 7 */
	__register_t r_s8;	/* x24 - saved register 8 */
	__register_t r_s9;	/* x25 - saved register 9 */
	__register_t r_s10;	/* x26 - saved register 10 */
	__register_t r_s11;	/* x27 - saved register 11 */
	__register_t r_t3;	/* x28 - temporary 3 */
	__register_t r_t4;	/* x29 - temporary 4 */
	__register_t r_t5;	/* x30 - temporary 5 */
	__register_t r_t6;	/* x31 - temporary 6 */
	__register_t r_pc;	/* program counter */
};

/*
 * Floating point registers
 */
struct fpreg {
	__uint64_t fp_f[32];	/* f0-f31 */
	__uint64_t fp_fcsr;	/* floating point control and status register */
};

#endif /* _RISCV_REG_H_ */
