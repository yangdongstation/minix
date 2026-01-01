/*	$NetBSD$	*/

#ifndef _RISCV_LIMITS_H_
#define _RISCV_LIMITS_H_

#include <sys/featuretest.h>

#define CHAR_BIT	8		/* number of bits in a char */

#define SCHAR_MAX	0x7f		/* max value for a signed char */
#define SCHAR_MIN	(-0x7f-1)	/* min value for a signed char */

#define UCHAR_MAX	0xff		/* max value for an unsigned char */

#define USHRT_MAX	0xffff		/* max value for an unsigned short */
#define SHRT_MAX	0x7fff		/* max value for a short */
#define SHRT_MIN	(-0x7fff-1)	/* min value for a short */

#define UINT_MAX	0xffffffffU	/* max value for an unsigned int */
#define INT_MAX		0x7fffffff	/* max value for an int */
#define INT_MIN		(-0x7fffffff-1)	/* min value for an int */

#ifdef __riscv64
#define ULONG_MAX	0xffffffffffffffffUL	/* max value for an unsigned long */
#define LONG_MAX	0x7fffffffffffffffL	/* max value for a long */
#define LONG_MIN	(-0x7fffffffffffffffL-1) /* min value for a long */
#else
#define ULONG_MAX	0xffffffffUL	/* max value for an unsigned long */
#define LONG_MAX	0x7fffffffL	/* max value for a long */
#define LONG_MIN	(-0x7fffffffL-1) /* min value for a long */
#endif

#if defined(_POSIX_C_SOURCE) || defined(_XOPEN_SOURCE) || \
    defined(_NETBSD_SOURCE)
#define SSIZE_MAX	LONG_MAX	/* max value for a ssize_t */

#if defined(_ISOC99_SOURCE) || (__STDC_VERSION__ - 0) >= 199901L || \
    defined(_NETBSD_SOURCE)
#define ULLONG_MAX	0xffffffffffffffffULL	/* max unsigned long long */
#define LLONG_MAX	0x7fffffffffffffffLL	/* max signed long long */
#define LLONG_MIN	(-0x7fffffffffffffffLL-1) /* min signed long long */
#endif

#if defined(_NETBSD_SOURCE)
#define SIZE_T_MAX	ULONG_MAX	/* max value for a size_t */

#define UQUAD_MAX	0xffffffffffffffffULL	/* max unsigned quad */
#define QUAD_MAX	0x7fffffffffffffffLL	/* max signed quad */
#define QUAD_MIN	(-0x7fffffffffffffffLL-1) /* min signed quad */

#endif /* _NETBSD_SOURCE */
#endif /* _POSIX_C_SOURCE || _XOPEN_SOURCE || _NETBSD_SOURCE */

#if defined(_XOPEN_SOURCE) || defined(_NETBSD_SOURCE)
#ifdef __riscv64
#define LONG_BIT	64
#else
#define LONG_BIT	32
#endif
#define WORD_BIT	32

#ifndef DBL_DIG
#define DBL_DIG		__DBL_DIG__
#endif
#ifndef DBL_MAX
#define DBL_MAX		__DBL_MAX__
#endif
#ifndef DBL_MIN
#define DBL_MIN		__DBL_MIN__
#endif

#ifndef FLT_DIG
#define FLT_DIG		__FLT_DIG__
#endif
#ifndef FLT_MAX
#define FLT_MAX		__FLT_MAX__
#endif
#ifndef FLT_MIN
#define FLT_MIN		__FLT_MIN__
#endif

#endif /* _XOPEN_SOURCE || _NETBSD_SOURCE */

#endif /* _RISCV_LIMITS_H_ */
