/*	$NetBSD: asm.h,v 1.1 2014/03/18 18:08:08 matt Exp $	*/

/*-
 * Copyright (c) 2014 The NetBSD Foundation, Inc.
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

#ifndef _RISCV_ASM_H_
#define _RISCV_ASM_H_

#ifdef __PIC__
#define PIC_PROLOGUE	\
	.cfi_startproc; \
	.option push; \
	.option norelax; \
	la	gp, __global_pointer$; \
	.option pop; \
	.cfi_endproc
#define PIC_EPILOGUE
#define PIC_PLT(x)	x
#define PIC_GOT(x)	x
#define PIC_GOTOFF(x)	x
#else
#define PIC_PROLOGUE
#define PIC_EPILOGUE
#define PIC_PLT(x)	x
#define PIC_GOT(x)	x
#define PIC_GOTOFF(x)	x
#endif

#ifdef __ELF__
# define _C_LABEL(x)	x
#else
# ifdef __STDC__
#  define _C_LABEL(x)	_ ## x
# else
#  define _C_LABEL(x)	_/**/x
# endif
#endif
#define	_ASM_LABEL(x)	x

#define CVAROFF(x, y)		_C_LABEL(x) + y

#ifdef __STDC__
# define __CONCAT(x,y)	x ## y
# define __STRING(x)	#x
#else
# define __CONCAT(x,y)	x/**/y
# define __STRING(x)	"x"
#endif

/* let kernels and others override entrypoint alignment */
#if !defined(_ALIGN_TEXT) && !defined(_KERNEL)
# ifdef _STANDALONE
#  define _ALIGN_TEXT .align 2
# elif defined __ELF__
#  define _ALIGN_TEXT .align 2
# else
#  define _ALIGN_TEXT .align 2
# endif
#endif

#define _ENTRY(x) \
	.text; _ALIGN_TEXT; .globl x; .type x,@function; x:
#define _LABEL(x) \
	.globl x; x:

#ifdef _KERNEL

#define CPUVAR(off) %tp:__CONCAT(CPU_INFO_,off)

#define	IDTVEC(name) \
	ALIGN_TEXT; .globl X ## name; .type X ## name,@function; X ## name:
#define	IDTVEC_END(name) \
	.size X ## name, . - X ## name

#ifdef _STANDALONE
#define ALIGN_DATA	.align	3
#define ALIGN_TEXT	.align	2	/* 4-byte boundaries */
#define SUPERALIGN_TEXT	.align	4	/* 16-byte boundaries */
#elif defined __ELF__
#define ALIGN_DATA	.align	3
#define ALIGN_TEXT	.align	2	/* 4-byte boundaries */
#define SUPERALIGN_TEXT	.align	4	/* 16-byte boundaries */
#else
#define ALIGN_DATA	.align	3
#define ALIGN_TEXT	.align	2	/* 4-byte boundaries */
#define SUPERALIGN_TEXT	.align	4	/* 16-byte boundaries */
#endif /* __ELF__ */

#define _ALIGN_TEXT ALIGN_TEXT

#ifdef GPROF
#ifdef __ELF__
#define	MCOUNT_ASM	call	_C_LABEL(__mcount)
#else /* __ELF__ */
#define	MCOUNT_ASM	call	_C_LABEL(mcount)
#endif /* __ELF__ */
#else /* GPROF */
#define	MCOUNT_ASM	/* nothing */
#endif /* GPROF */

#endif /* _KERNEL */


#ifdef GPROF
# ifdef __ELF__
#  define _PROF_PROLOGUE	\
	call	_C_LABEL(__mcount)
# else
#  define _PROF_PROLOGUE	\
	call	C_LABEL(mcount)
# endif
#else
# define _PROF_PROLOGUE
#endif

#define	ENTRY(y)	_ENTRY(_C_LABEL(y)); _PROF_PROLOGUE
#define	NENTRY(y)	_ENTRY(_C_LABEL(y))
#define	ASENTRY(y)	_ENTRY(_ASM_LABEL(y)); _PROF_PROLOGUE
#define	LABEL(y)	_LABEL(_C_LABEL(y))
#define	END(y)		.size y, . - y

#define	ASMSTR		.asciz

#ifdef __ELF__
#define RCSID(x)	.pushsection ".ident"; .asciz x; .popsection
#else
#define RCSID(x)	.text; .asciz x
#endif

#ifdef NO_KERNEL_RCSIDS
#define	__KERNEL_RCSID(_n, _s)	/* nothing */
#else
#define	__KERNEL_RCSID(_n, _s)	RCSID(_s)
#endif

#ifdef __ELF__
#define	WEAK_ALIAS(alias,sym)						\
	.weak alias;							\
	alias = sym
#endif
/*
 * STRONG_ALIAS: create a strong alias.
 */
#define STRONG_ALIAS(alias,sym)						\
	.globl alias;							\
	alias = sym

#ifdef __STDC__
#define	WARN_REFERENCES(sym,msg)					\
	.pushsection .gnu.warning. ## sym;				\
	.ascii msg;							\
	.popsection
#else
#define	WARN_REFERENCES(sym,msg)					\
	.pushsection .gnu.warning./**/sym;				\
	.ascii msg;							\
	.popsection
#endif /* __STDC__ */

#if defined(__minix)
#define IMPORT(sym)               \
        .extern _C_LABEL(sym)
#endif /* defined(__minix) */

#endif /* !_RISCV_ASM_H_ */