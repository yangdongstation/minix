/* Console support for RISC-V 64-bit */
#include <minix/ipc.h>
#include <minix/sysutil.h>
#include <minix/com.h>
#include <sys/termios.h>
#include "tty.h"

/* Forward declarations */
void riscv_cons_init(void);
void riscv_cons_putc(int c);

/* Video operations (no-op for serial console) */
void
do_video(message *m, int ipc_status)
{
    /* No video hardware on QEMU virt */
}

/* Screen initialization */
void
scr_init(tty_t *tp)
{
    /* Initialize console UART */
    riscv_cons_init();
}

/* Console stop (no-op) */
void
cons_stop(void)
{
    /* Nothing to stop for serial console */
}

/* Beep support (no-op) */
void
beep_x(unsigned int freq, clock_t dur)
{
    /* No speaker on QEMU virt */
}

/* Load font (no-op) */
int
con_loadfont(endpoint_t endpt, cp_grant_id_t grant)
{
    /* No graphics on serial console */
    return ENODEV;
}

void
riscv_cons_init(void)
{
    /* No hardware init needed for SBI console. */
}

void
riscv_cons_putc(int c)
{
    ser_putc((char)c);
}
