/*
stacktrace.c

Created:	Jan 19, 1993 by Philip Homburg

Copyright 1995 Philip Homburg
*/

#include <stdio.h>
#include <string.h>
#include <machine/stackframe.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>

extern reg_t get_bp(void);

void util_stacktrace(void)
{
#if defined(__riscv64__)
#ifndef __kernel__
	{
		static char msg[] = "syslib: stacktrace disabled on riscv64\n";
		sys_diagctl_diag(msg, (int)strlen(msg));
	}
#endif
	return;
#endif
#if USE_SYSDEBUG
	reg_t bp, pc, hbp;

	bp= get_bp();
	while(bp)
	{
		pc= ((reg_t *)bp)[1];
		hbp= ((reg_t *)bp)[0];
		printf("0x%lx ", (unsigned long) pc);
		if (hbp != 0 && hbp <= bp)
		{
			printf("0x%lx ", (unsigned long) -1);
			break;
		}
		bp= hbp;
	}
	printf("\n");
#endif /* USE_SYSDEBUG */
}
