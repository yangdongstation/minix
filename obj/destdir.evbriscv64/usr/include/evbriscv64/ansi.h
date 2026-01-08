/*	$NetBSD$	*/

#ifndef _RISCV_ANSI_H_
#define _RISCV_ANSI_H_

#include <sys/cdefs.h>
#include <machine/int_types.h>

/*
 * Types which are fundamental to the implementation and may appear in
 * more than one standard header are defined here.
 */

#define _BSD_CLOCK_T_		unsigned long
#define _BSD_PTRDIFF_T_		long
#define _BSD_SIZE_T_		unsigned long
#define _BSD_SSIZE_T_		long
#define _BSD_TIME_T_		__int64_t
#define _BSD_CLOCKID_T_		int
#define _BSD_TIMER_T_		int
#define _BSD_SUSECONDS_T_	int
#define _BSD_USECONDS_T_	unsigned int
#define _BSD_WCHAR_T_		int
#define _BSD_WINT_T_		int

#endif /* _RISCV_ANSI_H_ */
