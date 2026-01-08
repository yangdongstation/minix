/*	$NetBSD$	*/

/*-
 * Copyright (c) 2026 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Matt Thomas of 3am Software Foundry.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RISCV_MCONTEXT_H_
#define _RISCV_MCONTEXT_H_

#include <sys/cdefs.h>
#include <sys/stdint.h>

/*
 * General register state. The layout must match the offsets used by
 * minix/lib/libc/arch/riscv64/sys/ucontext.S.
 */
#define _NGREG		32
typedef unsigned long	__greg_t;
typedef __greg_t	__gregset_t[_NGREG];

#define _REG_PC		0
#define _REG_RA		1
#define _REG_SP		2
#define _REG_GP		3
#define _REG_TP		4
#define _REG_T0		5
#define _REG_T1		6
#define _REG_T2		7
#define _REG_S0		8
#define _REG_S1		9
#define _REG_A0		10
#define _REG_A1		11
#define _REG_A2		12
#define _REG_A3		13
#define _REG_A4		14
#define _REG_A5		15
#define _REG_A6		16
#define _REG_A7		17
#define _REG_S2		18
#define _REG_S3		19
#define _REG_S4		20
#define _REG_S5		21
#define _REG_S6		22
#define _REG_S7		23
#define _REG_S8		24
#define _REG_S9		25
#define _REG_S10	26
#define _REG_S11	27
#define _REG_T3		28
#define _REG_T4		29
#define _REG_T5		30
#define _REG_T6		31
#define _REG_RV		_REG_A0

typedef struct {
	__gregset_t	__gregs;
	int		mc_flags;
	int		mc_magic;
} mcontext_t;

/* Machine-dependent uc_flags */
#define _UC_SETSTACK	0x00010000
#define _UC_CLRSTACK	0x00020000
#define _UC_TLSBASE	0x00080000

#define _UC_MACHINE_SP(uc)	((uc)->uc_mcontext.__gregs[_REG_SP])
#define _UC_MACHINE_PC(uc)	((uc)->uc_mcontext.__gregs[_REG_PC])
#define _UC_MACHINE_INTRV(uc)	((uc)->uc_mcontext.__gregs[_REG_A0])

#define	_UC_MACHINE_SET_PC(uc, pc)	_UC_MACHINE_PC(uc) = (pc)

#if defined(__minix)
#define _UC_MACHINE_STACK(uc)		((uc)->uc_mcontext.__gregs[_REG_SP])
#define	_UC_MACHINE_SET_STACK(uc, sp)	_UC_MACHINE_STACK(uc) = (sp)

#define _UC_MACHINE_FP(uc)		((uc)->uc_mcontext.__gregs[_REG_S0])
#define	_UC_MACHINE_SET_FP(uc, fp)	_UC_MACHINE_FP(uc) = (fp)

#define _UC_MACHINE_LR(uc)		((uc)->uc_mcontext.__gregs[_REG_RA])
#define	_UC_MACHINE_SET_LR(uc, lr)	_UC_MACHINE_LR(uc) = (lr)

#define _UC_MACHINE_R0(uc)		((uc)->uc_mcontext.__gregs[_REG_A0])
#define	_UC_MACHINE_SET_R0(uc, val)	_UC_MACHINE_R0(uc) = (val)
#define _UC_MACHINE_R1(uc)		((uc)->uc_mcontext.__gregs[_REG_A1])
#define	_UC_MACHINE_SET_R1(uc, val)	_UC_MACHINE_R1(uc) = (val)
#define _UC_MACHINE_R2(uc)		((uc)->uc_mcontext.__gregs[_REG_A2])
#define	_UC_MACHINE_SET_R2(uc, val)	_UC_MACHINE_R2(uc) = (val)
#define _UC_MACHINE_R3(uc)		((uc)->uc_mcontext.__gregs[_REG_A3])
#define	_UC_MACHINE_SET_R3(uc, val)	_UC_MACHINE_R3(uc) = (val)
#define _UC_MACHINE_R4(uc)		((uc)->uc_mcontext.__gregs[_REG_A4])
#define	_UC_MACHINE_SET_R4(uc, val)	_UC_MACHINE_R4(uc) = (val)
#define _UC_MACHINE_R5(uc)		((uc)->uc_mcontext.__gregs[_REG_A5])
#define	_UC_MACHINE_SET_R5(uc, val)	_UC_MACHINE_R5(uc) = (val)
#define _UC_MACHINE_R6(uc)		((uc)->uc_mcontext.__gregs[_REG_A6])
#define	_UC_MACHINE_SET_R6(uc, val)	_UC_MACHINE_R6(uc) = (val)
#define _UC_MACHINE_R7(uc)		((uc)->uc_mcontext.__gregs[_REG_A7])
#define	_UC_MACHINE_SET_R7(uc, val)	_UC_MACHINE_R7(uc) = (val)

#define _UC_MACHINE_S1(uc)		((uc)->uc_mcontext.__gregs[_REG_S1])
#define	_UC_MACHINE_SET_S1(uc, val)	_UC_MACHINE_S1(uc) = (val)

__BEGIN_DECLS
int setmcontext(const mcontext_t *mcp);
int getmcontext(mcontext_t *mcp);
__END_DECLS

#define MCF_MAGIC 0xc0ffee
#endif /* defined(__minix) */

static __inline void *
__lwp_getprivate_fast(void)
{
	void *tp;

	__asm volatile("mv %0, tp" : "=r"(tp));
	return tp;
}

#endif /* !_RISCV_MCONTEXT_H_ */
