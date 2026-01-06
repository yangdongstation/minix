#include "syslib.h"

#if defined(__riscv64__)
int sys_vmctl(endpoint_t who, int param, u64_t value)
#else
int sys_vmctl(endpoint_t who, int param, u32_t value)
#endif
{
  message m;
  int r;

  m.SVMCTL_WHO = who;
  m.SVMCTL_PARAM = param;
  m.SVMCTL_VALUE = value;
  r = _kernel_call(SYS_VMCTL, &m);
  return(r);
}

/* Get page directory base register */
#if defined(__riscv64__)
int sys_vmctl_get_pdbr(endpoint_t who, phys_bytes *pdbr)
#else
int sys_vmctl_get_pdbr(endpoint_t who, u32_t *pdbr)
#endif
{
  message m;
  int r;

  m.SVMCTL_WHO = who;
  m.SVMCTL_PARAM = VMCTL_GET_PDBR;
  r = _kernel_call(SYS_VMCTL, &m);
  if(r == OK) {
	*pdbr = m.SVMCTL_VALUE;
  }
  return(r);
}

int sys_vmctl_set_addrspace(endpoint_t who,
        phys_bytes ptroot, void *ptroot_v)
{
  message m;
  int r;

  m.SVMCTL_WHO = who;
  m.SVMCTL_PARAM = VMCTL_SETADDRSPACE;
#if defined(__riscv64__)
  m.SVMCTL_PTROOT = (void *)(unsigned long)ptroot;
#else
  m.SVMCTL_PTROOT = ptroot;
#endif
  m.SVMCTL_PTROOT_V = ptroot_v;
  r = _kernel_call(SYS_VMCTL, &m);

  return(r);
}

int sys_vmctl_get_memreq(endpoint_t *who, vir_bytes *mem,
        vir_bytes *len, int *wrflag, endpoint_t *who_s, vir_bytes *mem_s,
        endpoint_t *requestor)
{
  message m;
  int r;

  m.SVMCTL_WHO = SELF;
  m.SVMCTL_PARAM = VMCTL_MEMREQ_GET;
  r = _kernel_call(SYS_VMCTL, &m);
  if(r >= 0) {
	*who = m.SVMCTL_MRG_TARGET;
	*mem = m.SVMCTL_MRG_ADDR;
	*len = m.SVMCTL_MRG_LENGTH;
	*wrflag = m.SVMCTL_MRG_FLAG;
	*who_s = m.SVMCTL_MRG_EP2;
	*mem_s = m.SVMCTL_MRG_ADDR2;
	*requestor = (endpoint_t)(uintptr_t) m.SVMCTL_MRG_REQUESTOR;
  }
  return r;
}

int sys_vmctl_get_mapping(int index,
	phys_bytes *addr, phys_bytes *len, int *flags)
{
	int r;
	message m;

	m.SVMCTL_WHO = SELF;
	m.SVMCTL_PARAM = VMCTL_KERN_PHYSMAP;
	m.SVMCTL_VALUE = (int) index;

	r = _kernel_call(SYS_VMCTL, &m);

	if(r != OK)
		return r;

	*addr = m.SVMCTL_MAP_PHYS_ADDR;
	*len = m.SVMCTL_MAP_PHYS_LEN;
	*flags = m.SVMCTL_MAP_FLAGS;

	return OK;
}

int sys_vmctl_reply_mapping(int index, vir_bytes addr)
{
	message m;

	m.SVMCTL_WHO = SELF;
	m.SVMCTL_PARAM = VMCTL_KERN_MAP_REPLY;
	m.SVMCTL_VALUE = index;
	m.SVMCTL_MAP_VIR_ADDR = (char *) addr;

	return _kernel_call(SYS_VMCTL, &m);
}
