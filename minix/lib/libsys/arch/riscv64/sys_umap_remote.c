/*
 * RISC-V 64 remote memory mapping
 */

#include <lib.h>
#include <minix/syslib.h>
#include <string.h>

/*
 * Map remote memory segment
 */
int sys_umap_remote(endpoint_t ep, endpoint_t grantee, int seg,
    vir_bytes offset, vir_bytes len, phys_bytes *addr)
{
    message m;
    int r;

    memset(&m, 0, sizeof(m));
    m.m_type = SYS_UMAP_REMOTE;
    m.m_lsys_krn_sys_umap.src_endpt = ep;
    m.m_lsys_krn_sys_umap.dst_endpt = grantee;
    m.m_lsys_krn_sys_umap.segment = seg;
    m.m_lsys_krn_sys_umap.src_addr = offset;
    m.m_lsys_krn_sys_umap.nr_bytes = len;

    r = _kernel_call(SYS_UMAP_REMOTE, &m);

    if (r == OK && addr != NULL) {
        *addr = m.m_krn_lsys_sys_umap.dst_addr;
    }

    return r;
}
