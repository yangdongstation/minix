/*	$NetBSD$	*/

#ifndef _RISCV_FENV_H_
#define _RISCV_FENV_H_

#include <sys/cdefs.h>

typedef unsigned long fenv_t;
typedef unsigned int fexcept_t;

#define	FE_INVALID	0x10
#define	FE_DIVBYZERO	0x08
#define	FE_OVERFLOW	0x04
#define	FE_UNDERFLOW	0x02
#define	FE_INEXACT	0x01

#define	FE_ALL_EXCEPT	0x1f

#define	FE_TONEAREST	0
#define	FE_TOWARDZERO	1
#define	FE_DOWNWARD	2
#define	FE_UPWARD	3

__BEGIN_DECLS
extern const fenv_t	__fe_dfl_env;
#define FE_DFL_ENV	(&__fe_dfl_env)
__END_DECLS

#endif /* _RISCV_FENV_H_ */
