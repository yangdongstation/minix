#include "syslib.h"
#include <string.h>

int sys_vircopy(endpoint_t src_proc, vir_bytes src_vir, endpoint_t dst_proc,
	vir_bytes dst_vir, phys_bytes bytes, int flags)
{
/* Transfer a block of data.  The source and destination can each either be a
 * process number or SELF (to indicate own process number).
 */

  message copy_mess;

  if (bytes == 0L) return(OK);
  memset(&copy_mess, 0, sizeof(copy_mess));
  copy_mess.m_lsys_krn_sys_copy.src_endpt = src_proc;
  copy_mess.m_lsys_krn_sys_copy.src_addr = src_vir;
  copy_mess.m_lsys_krn_sys_copy.dst_endpt = dst_proc;
  copy_mess.m_lsys_krn_sys_copy.dst_addr = dst_vir;
  copy_mess.m_lsys_krn_sys_copy.nr_bytes = bytes;
  copy_mess.m_lsys_krn_sys_copy.flags = flags;

  return(_kernel_call(SYS_VIRCOPY, &copy_mess));
}
