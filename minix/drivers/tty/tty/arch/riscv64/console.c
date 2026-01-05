/* Console support for RISC-V 64-bit */
#include <minix/drivers.h>
#include <minix/ipc.h>
#include <minix/sysutil.h>
#include <minix/com.h>
#include <minix/vm.h>
#include <sys/termios.h>
#include <sys/mman.h>
#include <stdint.h>
#include <string.h>
#include "tty.h"

/* Forward declarations */
void riscv_cons_init(void);
void riscv_cons_putc(int c);

static int cons_write(tty_t *tp, int try);
static void cons_echo(tty_t *tp, int c);
static int cons_read(tty_t *tp, int try);

/* UART MMIO base address (QEMU virt). */
#define UART_BASE 0x10000000UL
#define UART_SIZE 0x100

/* UART register offsets. */
#define UART_RBR 0x00
#define UART_THR 0x00
#define UART_LSR 0x05

/* UART line status bits. */
#define LSR_DR 0x01
#define LSR_THRE 0x20

static volatile u8_t *uart_base;

static inline int
uart_rx_ready(void)
{
	return uart_base != NULL && (uart_base[UART_LSR] & LSR_DR) != 0;
}

static inline int
uart_tx_ready(void)
{
	return uart_base != NULL && (uart_base[UART_LSR] & LSR_THRE) != 0;
}

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
	if (uart_base != NULL)
		return;

	uart_base = vm_map_phys(SELF, (void *)UART_BASE, UART_SIZE);
	if (uart_base == MAP_FAILED)
		uart_base = NULL;
}

void
riscv_cons_putc(int c)
{
	if (uart_base == NULL)
		riscv_cons_init();

	if (uart_base == NULL) {
		ser_putc((char)c);
		return;
	}

	while (!uart_tx_ready())
		;
	uart_base[UART_THR] = (u8_t)c;
}

static int
cons_read(tty_t *tp, int try)
{
	char buf[64];
	int count;

	if (uart_base == NULL)
		riscv_cons_init();

	if (try)
		return uart_rx_ready();

	if (uart_base == NULL)
		return 0;

	count = 0;
	while (count < (int)sizeof(buf) && uart_rx_ready()) {
		buf[count++] = (char)uart_base[UART_RBR];
	}

	if (count > 0)
		in_process(tp, buf, count);

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
            const char *src = (const char *)(uintptr_t)(uint32_t)tp->tty_outgrant;
            memcpy(buf, src + tp->tty_outcum, count);
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
