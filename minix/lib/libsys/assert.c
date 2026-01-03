/*
 * assert.c - diagnostics
 */

#include	<assert.h>
#include	<stdio.h>

#include <sys/types.h>
#include <stdlib.h>
#include <minix/sysutil.h>

#if defined(__kernel__) && defined(__riscv64)
void direct_print(const char *str);
void direct_print_dec(u64_t val);
#endif

void
__assert13(const char *file, int line, const char *function,
    const char *failedexpr)
{
#if defined(__kernel__) && defined(__riscv64)
	direct_print(file);
	direct_print(":");
	direct_print_dec((u64_t)line);
	direct_print(": assert \"");
	direct_print(failedexpr);
	direct_print("\" failed");
	if (function) {
		direct_print(", function \"");
		direct_print(function);
		direct_print("\"");
	}
	direct_print("\n");
#else
	(void)printf("%s:%d: assert \"%s\" failed", file, line, failedexpr);
	if(function) printf(", function \"%s\"", function);
	printf("\n");
#endif
	panic("assert failed");
	/* NOTREACHED */
}

void
__assert(const char *file, int line, const char *failedexpr)
{

	__assert13(file, line, NULL, failedexpr);
	/* NOTREACHED */
}
