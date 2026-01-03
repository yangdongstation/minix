#include "syslib.h"
#include <stdio.h>

/*===========================================================================*
 *                                sys_exit			     	     *
 *===========================================================================*/
int sys_exit(void)
{
/* A system process requests to exit. */
  message m;

#ifdef __riscv64
  {
	static int sys_exit_trace_count;
	void *ra = __builtin_return_address(0);
	endpoint_t me = NONE;
	char name[20];
	int priv_flags, init_flags;

	if (sys_exit_trace_count < 4) {
		char buf[128];
		int len = 0;

		if (sys_whoami(&me, name, sizeof(name),
		    &priv_flags, &init_flags) != OK) {
			name[0] = '\0';
			me = NONE;
		}

		len = snprintf(buf, sizeof(buf),
		    "rv64: sys_exit ep=%d name=%s ra=%p\n",
		    me, name, ra);
		if (len > 0)
			sys_diagctl_diag(buf, len);
		sys_exit_trace_count++;
	}
  }
#endif

  return(_kernel_call(SYS_EXIT, &m));
}
