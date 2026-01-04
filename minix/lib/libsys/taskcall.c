/* _taskcall() is the same as _syscall() except it returns negative error
 * codes directly and not in errno.  This is a better interface for PM and
 * VFS.
 */

#include <lib.h>
#include <minix/syslib.h>
#include <stdio.h>

int _taskcall(endpoint_t who, int syscallnr, message *msgptr)
{
  int status;
#if defined(__riscv)
  static int vm_taskcall_log_count;
#endif

  msgptr->m_type = syscallnr;
#if defined(__riscv)
  if (who == VM_PROC_NR && vm_taskcall_log_count < 8) {
	char buf[80];
	int len = snprintf(buf, sizeof(buf),
	    "taskcall: who=%d call=%d\n", who, syscallnr);
	if (len < 0)
		len = 0;
	if (len > (int)sizeof(buf))
		len = (int)sizeof(buf);
	sys_diagctl_diag(buf, len);
	vm_taskcall_log_count++;
  }
#endif
  status = ipc_sendrec(who, msgptr);
  if (status != 0) return(status);
  return(msgptr->m_type);
}
