/*	$NetBSD$	*/

/*-
 * Copyright (c) 2014 The NetBSD Foundation, Inc.
 * All rights reserved.
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

#ifndef _RISCV_PARAM_H_
#define _RISCV_PARAM_H_

/*
 * Machine dependent constants for RISC-V.
 */

#ifdef _KERNEL_OPT
#include "opt_param.h"
#endif

#define _MACHINE	riscv
#define MACHINE		"riscv"

#ifdef __riscv64
#define _MACHINE_ARCH	riscv64
#define MACHINE_ARCH	"riscv64"
#define MID_MACHINE	MID_RISCV
#else
#define _MACHINE_ARCH	riscv32
#define MACHINE_ARCH	"riscv32"
#define MID_MACHINE	MID_RISCV
#endif

#define ALIGNBYTES32	(sizeof(int) - 1)
#define ALIGN32(p)	(((uintptr_t)(p) + ALIGNBYTES32) & ~ALIGNBYTES32)

#define STACK_ALIGNBYTES	(16 - 1)

/*
 * Round p (pointer or byte index) up to a correctly-aligned value
 * for all data types (int, long, ...).   The result is u_int and
 * must be cast to any desired pointer type.
 *
 * ALIGNED_POINTER is a boolean macro that checks whether an address
 * is valid to fetch data elements of type t from on this architecture.
 * This does not reflect the optimal alignment, just the possibility
 * (within reasonable limits).
 */
#ifndef PGSHIFT
#define PGSHIFT		12		/* LOG2(NBPG) */
#endif
#ifndef NBPG
#define NBPG		(1 << PGSHIFT)	/* bytes/page */
#endif
#ifndef PGOFSET
#define PGOFSET		(NBPG - 1)	/* byte offset into page */
#endif
#ifndef NPTEPG
#define NPTEPG		(NBPG / sizeof(void *))
#endif

#define SSIZE		1		/* initial stack size/NBPG */
#define SINCR		1		/* increment of stack/NBPG */
#ifdef MAXSSIZ
#undef MAXSSIZ
#endif
#define MAXSSIZ		(64*1024*1024)	/* max stack size */
#define UPAGES		2		/* pages of u-area */
#define USPACE		(UPAGES * NBPG)	/* total size of u-area */

#ifndef MSGBUFSIZE
#define MSGBUFSIZE	(8 * NBPG)	/* default message buffer size */
#endif

#ifndef MAXPHYS
#define MAXPHYS		(64 * 1024)	/* max raw I/O transfer size */
#endif

/*
 * Constants related to network buffer management.
 * MCLBYTES must be no larger than NBPG (the software page size), and,
 * on machines that exchange pages of input or output buffers with mbuf
 * clusters (MAPPED_MBUFS), MCLBYTES must also be an integral multiple
 * of the hardware page size.
 */
#define MSIZE		256		/* size of an mbuf */

#ifndef MCLSHIFT
#define MCLSHIFT	11		/* convert bytes to m_buf clusters */
					/* 2K cluster can hold Ether frame */
#endif	/* MCLSHIFT */

#define MCLBYTES	(1 << MCLSHIFT)	/* size of a m_buf cluster */

#ifndef NMBCLUSTERS_MAX
#define NMBCLUSTERS_MAX	(0x2000000 / MCLBYTES)	/* Limit to 32MB for clusters */
#endif

#ifdef _KERNEL
void delay(unsigned int);
#define DELAY(x)	delay(x)
#endif

#endif /* _RISCV_PARAM_H_ */
