/*	$NetBSD$	*/

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

#ifndef _RISCV_TYPES_H_
#define _RISCV_TYPES_H_

#include <sys/cdefs.h>
#include <sys/featuretest.h>
#include <machine/int_types.h>

#if defined(_KERNEL) || defined(_KMEMUSER) || defined(_KERNTYPES)

typedef unsigned long	paddr_t;
typedef unsigned long	psize_t;
typedef unsigned long	vaddr_t;
typedef unsigned long	vsize_t;
#define PRIxPADDR	"lx"
#define PRIxPSIZE	"lx"
#define PRIuPSIZE	"lu"
#define PRIxVADDR	"lx"
#define PRIxVSIZE	"lx"
#define PRIuVSIZE	"lu"

typedef long int	register_t;
typedef unsigned long	uregister_t;
#define PRIxREGISTER	"lx"

#endif /* _KERNEL || _KMEMUSER || _KERNTYPES */

#if defined(_KERNEL)
typedef struct label_t {
	register_t lb_reg[14];	/* ra, sp, s0-s11 */
} label_t;
#endif

typedef unsigned long	__cpu_simple_lock_nv_t;
typedef long		__register_t;

#define __SIMPLELOCK_LOCKED	1
#define __SIMPLELOCK_UNLOCKED	0

#define __HAVE_COMMON___TLS_GET_ADDR
#define __HAVE_CPU_COUNTER
#define __HAVE_CPU_DATA_FIRST
#define __HAVE_FAST_SOFTINTS
#define __HAVE_MM_MD_DIRECT_MAPPED_PHYS
#define __HAVE_NEW_STYLE_BUS_H
#define __HAVE_SYSCALL_INTERN
#define __HAVE_TLS_VARIANT_I
#define __HAVE___LWP_GETPRIVATE_FAST

#ifdef __riscv64
#define __HAVE_ATOMIC64_OPS
#endif

#endif /* _RISCV_TYPES_H_ */
