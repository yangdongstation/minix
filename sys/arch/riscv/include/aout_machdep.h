/*	$NetBSD$	*/

#ifndef _RISCV_AOUT_MACHDEP_H_
#define _RISCV_AOUT_MACHDEP_H_

#define	AOUT_LDPGSZ	4096

/* Relocation format. */
struct relocation_info_riscv {
	int r_address;		/* offset in text or data segment */
	unsigned r_symbolnum:24;/* ordinal number of add symbol */
	unsigned r_pcrel:1;	/* 1 if value should be pc-relative */
	unsigned r_length:2;	/* 0=byte, 1=word, 2=long, 3=24bits shifted by 2 */
	unsigned r_extern:1;	/* 1 if need to add symbol to value */
	unsigned r_neg:1;	/* 1 if addend is negative */
	unsigned r_baserel:1;	/* 1 if linkage table relative */
	unsigned r_jmptable:1;	/* 1 if relocation to jump table */
	unsigned r_relative:1;	/* 1 if load address relative */
};

#define relocation_info relocation_info_riscv

/* No special executable format */
#define	cpu_exec_aout_makecmds(a, b)	ENOEXEC

#endif /* _RISCV_AOUT_MACHDEP_H_ */
