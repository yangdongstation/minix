/* Console support for RISC-V 64-bit */
#include <minix/drivers.h>
#include <minix/ipc.h>
#include <minix/sysutil.h>
#include <minix/com.h>
#include <sys/termios.h>
#include <string.h>
#include "tty.h"

/* Forward declarations */
void riscv_cons_init(void);
void riscv_cons_putc(int c);

static int cons_write(tty_t *tp, int try);
static void cons_echo(tty_t *tp, int c);
static int cons_read(tty_t *tp, int try);

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
    /* Initialize console UART and hook into TTY core */
    riscv_cons_init();
    tp->tty_devread = cons_read;
    tp->tty_devwrite = cons_write;
    tp->tty_echo = cons_echo;
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

static int
cons_read(tty_t *tp, int try)
{
    /* No keyboard support yet; keep the line active for output. */
    (void)tp;
    if (try)
        return 0;
    return 0;
}

static int
cons_write(tty_t *tp, int try)
{
    int count;
    int result = OK;
    char buf[64];

    if (try)
        return 1;

    if ((count = tp->tty_outleft) == 0 || tp->tty_inhibited)
        return 0;

    do {
        if (count > (int)sizeof(buf))
            count = sizeof(buf);

        if (tp->tty_outcaller == KERNEL) {
            memcpy(buf, (char *)tp->tty_outgrant + tp->tty_outcum, count);
        } else {
            result = sys_safecopyfrom(tp->tty_outcaller, tp->tty_outgrant,
                tp->tty_outcum, (vir_bytes)buf, count);
            if (result != OK)
                break;
        }

        tp->tty_outcum += count;
        tp->tty_outleft -= count;

        for (int i = 0; i < count; i++)
            riscv_cons_putc(buf[i]);
    } while ((count = tp->tty_outleft) != 0 && !tp->tty_inhibited);

    if (tp->tty_outleft == 0 || result != OK) {
        if (tp->tty_outcaller != KERNEL)
            chardriver_reply_task(tp->tty_outcaller, tp->tty_outid,
                result != OK ? result : tp->tty_outcum);
        tp->tty_outcum = tp->tty_outleft = 0;
        tp->tty_outcaller = NONE;
    }

    return 0;
}

static void
cons_echo(tty_t *tp, int c)
{
    (void)tp;
    riscv_cons_putc(c);
}
