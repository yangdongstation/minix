/*
 * RISC-V 64 IPC system call interface
 *
 * MINIX IPC uses ecall to trap to kernel.
 * Arguments:
 *   a0 = destination/source endpoint
 *   a1 = message pointer
 *   a2 = IPC operation (SEND, RECEIVE, etc.)
 *   a7 = syscall number (IPCVEC)
 *
 * Returns:
 *   a0 = return value (0 on success, error code on failure)
 *   a1 = status (for receive operations)
 */

#include <minix/ipcconst.h>
#include <machine/asm.h>

/* Stack offsets for arguments (LP64 ABI) */
#define SRC_DST     0       /* a0: source/destination */
#define MESSAGE     8       /* a1: message pointer */
#define STATUS      16      /* a2: status pointer (for receive) */
#define MSGTAB      0       /* a0: message table (for senda) */
#define TABCOUNT    8       /* a1: table count (for senda) */

/*
 * int _ipc_send_intr(endpoint_t dest, message *m_ptr)
 *
 * Send a message to destination endpoint
 */
ENTRY(_ipc_send_intr)
    /* a0 = dest, a1 = message pointer (already in place) */
    li      a2, SEND            /* a2 = operation */
    li      a7, IPCVEC_INTR     /* a7 = IPC syscall vector */
    ecall                       /* trap to kernel */
    ret                         /* a0 = return value */
END(_ipc_send_intr)

/*
 * int _ipc_receive_intr(endpoint_t src, message *m_ptr, int *status)
 *
 * Receive a message from source endpoint
 */
ENTRY(_ipc_receive_intr)
    /* Save status pointer */
    mv      t0, a2              /* t0 = status pointer */
    /* a0 = src, a1 = message pointer (already in place) */
    li      a2, RECEIVE         /* a2 = operation */
    li      a7, IPCVEC_INTR     /* a7 = IPC syscall vector */
    ecall                       /* trap to kernel */
    /* Store status if pointer is not NULL */
    beqz    t0, 1f
    sw      a1, 0(t0)           /* *status = a1 */
1:
    ret                         /* a0 = return value */
END(_ipc_receive_intr)

/*
 * int _ipc_sendrec_intr(endpoint_t srcdest, message *m_ptr)
 *
 * Send a message and wait for reply
 */
ENTRY(_ipc_sendrec_intr)
    /* a0 = srcdest, a1 = message pointer (already in place) */
    li      a2, SENDREC         /* a2 = operation */
    li      a7, IPCVEC_INTR     /* a7 = IPC syscall vector */
    ecall                       /* trap to kernel */
    ret                         /* a0 = return value */
END(_ipc_sendrec_intr)

/*
 * int _ipc_notify_intr(endpoint_t dest)
 *
 * Send a notification to destination
 */
ENTRY(_ipc_notify_intr)
    /* a0 = dest (already in place) */
    li      a1, 0               /* a1 = NULL (no message) */
    li      a2, NOTIFY          /* a2 = operation */
    li      a7, IPCVEC_INTR     /* a7 = IPC syscall vector */
    ecall                       /* trap to kernel */
    ret                         /* a0 = return value */
END(_ipc_notify_intr)

/*
 * int _ipc_sendnb_intr(endpoint_t dest, message *m_ptr)
 *
 * Non-blocking send
 */
ENTRY(_ipc_sendnb_intr)
    /* a0 = dest, a1 = message pointer (already in place) */
    li      a2, SENDNB          /* a2 = operation */
    li      a7, IPCVEC_INTR     /* a7 = IPC syscall vector */
    ecall                       /* trap to kernel */
    ret                         /* a0 = return value */
END(_ipc_sendnb_intr)

/*
 * int _ipc_senda_intr(asynmsg_t *table, size_t count)
 *
 * Asynchronous send to multiple destinations
 */
ENTRY(_ipc_senda_intr)
    /* a0 = table, a1 = count (swap to match kernel expectations) */
    mv      t0, a0              /* t0 = table */
    mv      a0, a1              /* a0 = count */
    mv      a1, t0              /* a1 = table */
    li      a2, SENDA           /* a2 = operation */
    li      a7, IPCVEC_INTR     /* a7 = IPC syscall vector */
    ecall                       /* trap to kernel */
    ret                         /* a0 = return value */
END(_ipc_senda_intr)
