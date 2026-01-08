/*
 * RISC-V 64 IPC constants
 */

#ifndef _RISCV64_IPCCONST_H
#define _RISCV64_IPCCONST_H

/* IPC vector numbers - use ecall exception codes */
#define IPCVEC_INTR     8       /* ecall from U-mode */
#define KERVEC_INTR     9       /* ecall from S-mode */

#define IPC_STATUS_REG  a1

#endif /* _RISCV64_IPCCONST_H */
