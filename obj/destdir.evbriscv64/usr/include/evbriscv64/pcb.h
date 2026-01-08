/*	$NetBSD$	*/

/*
 * Machine-dependent process control block for RISC-V.
 */

#ifndef _RISCV_PCB_H_
#define _RISCV_PCB_H_

struct pcb {
	void *pcb_onfault;
};

struct md_coredump {
	int md_empty;
};

#endif /* _RISCV_PCB_H_ */
