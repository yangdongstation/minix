/*	$NetBSD$	*/

#ifndef _RISCV_ASM_H_
#define _RISCV_ASM_H_

#define _C_LABEL(x)	x
#define _ASM_LABEL(x)	x

#ifdef __STDC__
#define __CONCAT(x,y)	x ## y
#define __STRING(x)	#x
#else
#define __CONCAT(x,y)	x/**/y
#define __STRING(x)	"x"
#endif

#if defined(__riscv_xlen)
#if __riscv_xlen == 64
#define PTR_SCALESHIFT	3
#elif __riscv_xlen == 32
#define PTR_SCALESHIFT	2
#else
#define PTR_SCALESHIFT	3
#endif
#elif defined(__riscv64)
#define PTR_SCALESHIFT	3
#elif defined(__riscv32)
#define PTR_SCALESHIFT	2
#else
#define PTR_SCALESHIFT	3
#endif

/* let kernels and loaders use something like <machine/asm.h> */
#ifndef _LOCORE
#define _LOCORE
#endif

#define _ENTRY(name) \
	.globl name; \
	.type name, @function; \
	.align 4; \
	name:

#define ENTRY(name) \
	.text; \
	_ENTRY(name)

#define ENTRY_NP(name) \
	.text; \
	_ENTRY(name)

#define END(name) \
	.size name, . - name

#define STRONG_ALIAS(alias,sym) \
	.globl alias; \
	alias = sym

#define WEAK_ALIAS(alias,sym) \
	.weak alias; \
	alias = sym

#define RCSID(x) \
	.pushsection ".ident"; .asciz x; .popsection

/*
 * WARN_REFERENCES: create a warning if the specified symbol is referenced.
 */
#define WARN_REFERENCES(sym, msg)				\
	.pushsection .gnu.warning. ## sym;			\
	.ascii msg;						\
	.popsection

#endif /* _RISCV_ASM_H_ */
