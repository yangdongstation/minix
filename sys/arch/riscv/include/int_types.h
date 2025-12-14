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

#ifndef _RISCV_INT_TYPES_H_
#define _RISCV_INT_TYPES_H_

#include <sys/cdefs.h>

/*
 * 7.18.1 Integer types
 */

/* 7.18.1.1 Exact-width integer types */

typedef signed char		__int8_t;
typedef unsigned char		__uint8_t;
typedef short int		__int16_t;
typedef unsigned short int	__uint16_t;
typedef int			__int32_t;
typedef unsigned int		__uint32_t;
#ifdef __riscv64
typedef long int		__int64_t;
typedef unsigned long int	__uint64_t;
#else
__extension__ typedef long long int		__int64_t;
__extension__ typedef unsigned long long int	__uint64_t;
#endif

#define __BIT_TYPES_DEFINED__

/* 7.18.1.4 Integer types capable of holding object pointers */

#ifdef __riscv64
typedef long int		__intptr_t;
typedef unsigned long int	__uintptr_t;
#else
typedef int			__intptr_t;
typedef unsigned int		__uintptr_t;
#endif

#endif /* _RISCV_INT_TYPES_H_ */
