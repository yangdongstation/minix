/* MINIX RISC-V: no static TLS setup yet, provide a stub. */

#include <sys/cdefs.h>
#include <lwp.h>
#include <stddef.h>

__dso_hidden void
__libc_static_tls_setup(void)
{
	/* No-op until static TLS is implemented for this port. */
}

void
_lwp_setprivate(void *ptr)
{
	(void)ptr;
}

void *
_lwp_getprivate(void)
{
	return NULL;
}
