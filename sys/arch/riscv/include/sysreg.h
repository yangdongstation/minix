/*	$NetBSD$	*/

#ifndef _RISCV_SYSREG_H_
#define _RISCV_SYSREG_H_

#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/types.h>

#define FCSR_FFLAGS	__BITS(4, 0)
#define FCSR_FRM	__BITS(7, 5)
#define FCSR_FMASK	0
#define FCSR_MASK	0

#define FCSR_FRM_RNE	0
#define FCSR_FRM_RTZ	1
#define FCSR_FRM_RDN	2
#define FCSR_FRM_RUP	3
#define FCSR_FRM_RMM	4

static __inline unsigned long
riscvreg_fcsr_read(void)
{
	unsigned long val;

#if defined(__riscv_flen)
	__asm volatile("csrr %0, fcsr" : "=r"(val));
#else
	val = 0;
#endif
	return val;
}

static __inline unsigned long
riscvreg_fcsr_write(unsigned long val)
{
	unsigned long old;

#if defined(__riscv_flen)
	__asm volatile("csrrw %0, fcsr, %1" : "=r"(old) : "r"(val));
#else
	(void)val;
	old = 0;
#endif
	return old;
}

static __inline unsigned long
riscvreg_fcsr_read_fflags(void)
{
	unsigned long val;

#if defined(__riscv_flen)
	__asm volatile("csrr %0, fflags" : "=r"(val));
#else
	val = 0;
#endif
	return val;
}

static __inline unsigned long
riscvreg_fcsr_write_fflags(unsigned long val)
{
	unsigned long old;

#if defined(__riscv_flen)
	__asm volatile("csrrw %0, fflags, %1" : "=r"(old) : "r"(val));
#else
	(void)val;
	old = 0;
#endif
	return old;
}

static __inline unsigned long
riscvreg_fcsr_read_frm(void)
{
	unsigned long val;

#if defined(__riscv_flen)
	__asm volatile("csrr %0, frm" : "=r"(val));
#else
	val = 0;
#endif
	return val;
}

static __inline unsigned long
riscvreg_fcsr_write_frm(unsigned long val)
{
	unsigned long old;

#if defined(__riscv_flen)
	__asm volatile("csrrw %0, frm, %1" : "=r"(old) : "r"(val));
#else
	(void)val;
	old = 0;
#endif
	return old;
}

static __inline unsigned long
riscvreg_fcsr_write_fmask(unsigned long val)
{
	(void)val;
	return riscvreg_fcsr_read();
}

#endif /* _RISCV_SYSREG_H_ */
