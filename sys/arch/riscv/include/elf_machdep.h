/*	$NetBSD$	*/

#ifndef _RISCV_ELF_MACHDEP_H_
#define _RISCV_ELF_MACHDEP_H_

#if defined(__riscv64) || defined(__riscv64__)
#define ARCH_ELFSIZE		64	/* MD://arch-specific as "riscv64" */
#else
#define ARCH_ELFSIZE		32	/* MD://arch-specific as "riscv32" */
#endif

#define ELF32_MACHDEP_ENDIANNESS	ELFDATA2LSB
#define ELF32_MACHDEP_ID_CASES		\
		case EM_RISCV:		\
			break;
#define ELF32_MACHDEP_ID	EM_RISCV

#define ELF64_MACHDEP_ENDIANNESS	ELFDATA2LSB
#define ELF64_MACHDEP_ID_CASES		\
		case EM_RISCV:		\
			break;
#define ELF64_MACHDEP_ID	EM_RISCV

#define ELF_TARG_CLASS		ELFCLASS64
#define ELF_TARG_DATA		ELFDATA2LSB
#define ELF_TARG_MACH		EM_RISCV

/* RISC-V relocations */
#define R_RISCV_NONE		0
#define R_RISCV_32		1
#define R_RISCV_64		2
#define R_RISCV_RELATIVE	3
#define R_RISCV_COPY		4
#define R_RISCV_JUMP_SLOT	5
#define R_RISCV_TLS_DTPMOD32	6
#define R_RISCV_TLS_DTPMOD64	7
#define R_RISCV_TLS_DTPREL32	8
#define R_RISCV_TLS_DTPREL64	9
#define R_RISCV_TLS_TPREL32	10
#define R_RISCV_TLS_TPREL64	11

#define R_TYPE(name)		R_RISCV_ ## name
#define R_TLS_TYPE(name)	R_RISCV_ ## name

#endif /* _RISCV_ELF_MACHDEP_H_ */
