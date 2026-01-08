/*	$NetBSD$	*/

#ifndef _RISCV_KCORE_H_
#define _RISCV_KCORE_H_

#include <sys/types.h>

typedef struct cpu_kcore_hdr {
	uint32_t	nmemsegs;	/* Number of RAM segments */
} cpu_kcore_hdr_t;

#endif /* _RISCV_KCORE_H_ */
