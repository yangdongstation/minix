/*	$NetBSD$	*/

#ifndef _RISCV_INT_LIMITS_H_
#define _RISCV_INT_LIMITS_H_

/*
 * 7.18.2 Limits of specified-width integer types
 */

/* 7.18.2.1 Limits of exact-width integer types */

/* minimum values of exact-width signed integer types */
#define INT8_MIN	(-0x7f-1)
#define INT16_MIN	(-0x7fff-1)
#define INT32_MIN	(-0x7fffffff-1)
#define INT64_MIN	(-0x7fffffffffffffffLL-1)

/* maximum values of exact-width signed integer types */
#define INT8_MAX	0x7f
#define INT16_MAX	0x7fff
#define INT32_MAX	0x7fffffff
#define INT64_MAX	0x7fffffffffffffffLL

/* maximum values of exact-width unsigned integer types */
#define UINT8_MAX	0xff
#define UINT16_MAX	0xffff
#define UINT32_MAX	0xffffffffU
#define UINT64_MAX	0xffffffffffffffffULL

/* 7.18.2.2 Limits of minimum-width integer types */

/* minimum values of minimum-width signed integer types */
#define INT_LEAST8_MIN	(-0x7f-1)
#define INT_LEAST16_MIN	(-0x7fff-1)
#define INT_LEAST32_MIN	(-0x7fffffff-1)
#define INT_LEAST64_MIN	(-0x7fffffffffffffffLL-1)

/* maximum values of minimum-width signed integer types */
#define INT_LEAST8_MAX	0x7f
#define INT_LEAST16_MAX	0x7fff
#define INT_LEAST32_MAX	0x7fffffff
#define INT_LEAST64_MAX	0x7fffffffffffffffLL

/* maximum values of minimum-width unsigned integer types */
#define UINT_LEAST8_MAX	 0xff
#define UINT_LEAST16_MAX 0xffff
#define UINT_LEAST32_MAX 0xffffffffU
#define UINT_LEAST64_MAX 0xffffffffffffffffULL

/* 7.18.2.3 Limits of fastest minimum-width integer types */

/* minimum values of fastest minimum-width signed integer types */
#define INT_FAST8_MIN	(-0x7f-1)
#define INT_FAST16_MIN	(-0x7fffffff-1)
#define INT_FAST32_MIN	(-0x7fffffff-1)
#define INT_FAST64_MIN	(-0x7fffffffffffffffLL-1)

/* maximum values of fastest minimum-width signed integer types */
#define INT_FAST8_MAX	0x7f
#define INT_FAST16_MAX	0x7fffffff
#define INT_FAST32_MAX	0x7fffffff
#define INT_FAST64_MAX	0x7fffffffffffffffLL

/* maximum values of fastest minimum-width unsigned integer types */
#define UINT_FAST8_MAX	0xff
#define UINT_FAST16_MAX	0xffffffffU
#define UINT_FAST32_MAX	0xffffffffU
#define UINT_FAST64_MAX	0xffffffffffffffffULL

/* 7.18.2.4 Limits of integer types capable of holding object pointers */

#ifdef __riscv64
#define INTPTR_MIN	(-0x7fffffffffffffffL-1)
#define INTPTR_MAX	0x7fffffffffffffffL
#define UINTPTR_MAX	0xffffffffffffffffUL
#else
#define INTPTR_MIN	(-0x7fffffffL-1)
#define INTPTR_MAX	0x7fffffffL
#define UINTPTR_MAX	0xffffffffUL
#endif

/* 7.18.2.5 Limits of greatest-width integer types */

#define INTMAX_MIN	(-0x7fffffffffffffffLL-1)
#define INTMAX_MAX	0x7fffffffffffffffLL
#define UINTMAX_MAX	0xffffffffffffffffULL

/*
 * 7.18.3 Limits of other integer types
 */

/* limits of ptrdiff_t */
#ifdef __riscv64
#define PTRDIFF_MIN	(-0x7fffffffffffffffL-1)
#define PTRDIFF_MAX	0x7fffffffffffffffL
#else
#define PTRDIFF_MIN	(-0x7fffffffL-1)
#define PTRDIFF_MAX	0x7fffffffL
#endif

/* limit of size_t */
#ifdef __riscv64
#define SIZE_MAX	0xffffffffffffffffUL
#else
#define SIZE_MAX	0xffffffffUL
#endif

/* limits of wchar_t */
#define WCHAR_MIN	(-0x7fffffff-1)
#define WCHAR_MAX	0x7fffffff

/* limits of wint_t */
#define WINT_MIN	(-0x7fffffff-1)
#define WINT_MAX	0x7fffffff

/* limits of sig_atomic_t */
#define SIG_ATOMIC_MIN	(-0x7fffffff-1)
#define SIG_ATOMIC_MAX	0x7fffffff

#endif /* _RISCV_INT_LIMITS_H_ */
