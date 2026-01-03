/* The kernel call that is implemented in this file:
 *	m_type: SYS_KILL
 *
 * The parameters for this kernel call are:
 *	m_sigcalls.endpt  	# process to signal/ pending
 *	m_sigcalls.sig		# signal number to send to process
 */

#include "kernel/system.h"
#ifdef __riscv64
#include "arch_proto.h"
#endif
#include <signal.h>

#if USE_KILL

/*===========================================================================*
 *			          do_kill				     *
 *===========================================================================*/
int do_kill(struct proc * caller, message * m_ptr)
{
/* Handle sys_kill(). Cause a signal to be sent to a process. Any request
 * is added to the map of pending signals and the signal manager
 * associated to the process is informed about the new signal. The signal
 * is then delivered using POSIX signal handlers for user processes, or
 * translated into an IPC message for system services.
 */
  proc_nr_t proc_nr, proc_nr_e;
  int sig_nr = m_ptr->m_sigcalls.sig;

  proc_nr_e = (proc_nr_t)m_ptr->m_sigcalls.endpt;

#ifdef __riscv64
  {
	static int sigabort_trace_count;

	if (sig_nr == SIGABRT && sigabort_trace_count < 4) {
		direct_print("rv64: sys_kill caller=");
		direct_print(caller->p_name);
		direct_print("/");
		direct_print_hex((u64_t)caller->p_endpoint);
		direct_print(" target=");
		direct_print_hex((u64_t)proc_nr_e);
		direct_print(" sig=");
		direct_print_hex((u64_t)sig_nr);
		direct_print("\n");
		sigabort_trace_count++;
	}
  }
#endif

  if (!isokendpt(proc_nr_e, &proc_nr)) return(EINVAL);
  if (sig_nr >= _NSIG) return(EINVAL);
  if (iskerneln(proc_nr)) return(EPERM);

  /* Set pending signal to be processed by the signal manager. */
  cause_sig(proc_nr, sig_nr);

  return(OK);
}

#endif /* USE_KILL */
