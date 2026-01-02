/* MINIX RISC-V: no static TLS setup yet, provide a stub. */

#include <sys/cdefs.h>

__dso_hidden void
__libc_static_tls_setup(void)
{
	/* No-op until static TLS is implemented for this port. */
}
