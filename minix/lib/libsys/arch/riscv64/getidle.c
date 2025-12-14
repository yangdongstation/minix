/*
 * RISC-V 64 idle time measurement
 */

#include <lib.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>

/*
 * Get CPU idle time information
 */
int sys_getidletsc(u64_t *idlep)
{
    message m;
    int r;

    memset(&m, 0, sizeof(m));
    m.m_type = SYS_GETIDLETSC;

    r = _kernel_call(SYS_GETIDLETSC, &m);

    if (r == OK && idlep != NULL) {
        *idlep = m.m_lsys_krn_sys_getidletsc.idle_time;
    }

    return r;
}

/*
 * Get idle counter for a specific CPU
 */
int getidle(void)
{
    u64_t idle;
    int r;

    r = sys_getidletsc(&idle);
    if (r != OK)
        return 0;

    /* Return lower 32 bits as percentage indicator */
    return (int)(idle & 0xFFFFFFFF);
}
