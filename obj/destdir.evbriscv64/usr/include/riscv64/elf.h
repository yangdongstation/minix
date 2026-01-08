/*
 * RISC-V 64 ELF definitions
 */

#ifndef _RISCV64_ELF_H
#define _RISCV64_ELF_H

/* ELF machine type */
#define EM_RISCV        243

/* Define "machine" characteristics */
#define ELF_TARG_VER    1

/* RISC-V specific ELF flags */
#define EF_RISCV_RVC                0x0001  /* Uses compressed instructions */
#define EF_RISCV_FLOAT_ABI_MASK     0x0006  /* Floating point ABI */
#define EF_RISCV_FLOAT_ABI_SOFT     0x0000  /* Soft float */
#define EF_RISCV_FLOAT_ABI_SINGLE   0x0002  /* Single precision */
#define EF_RISCV_FLOAT_ABI_DOUBLE   0x0004  /* Double precision */
#define EF_RISCV_FLOAT_ABI_QUAD     0x0006  /* Quad precision */
#define EF_RISCV_RVE                0x0008  /* Uses E base ISA */
#define EF_RISCV_TSO                0x0010  /* RVTSO memory model */

/* Relocation types */
#define R_RISCV_NONE            0
#define R_RISCV_32              1
#define R_RISCV_64              2
#define R_RISCV_RELATIVE        3
#define R_RISCV_COPY            4
#define R_RISCV_JUMP_SLOT       5
#define R_RISCV_TLS_DTPMOD32    6
#define R_RISCV_TLS_DTPMOD64    7
#define R_RISCV_TLS_DTPREL32    8
#define R_RISCV_TLS_DTPREL64    9
#define R_RISCV_TLS_TPREL32     10
#define R_RISCV_TLS_TPREL64     11

/* Program header types */
#define PT_RISCV_ATTRIBUTES     0x70000003

#endif /* _RISCV64_ELF_H */
