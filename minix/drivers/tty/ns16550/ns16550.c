/*
 * NS16550A UART driver for MINIX 3
 *
 * For RISC-V QEMU virt platform UART at 0x10000000
 */

#include <minix/drivers.h>
#include <minix/chardriver.h>
#include <minix/sysutil.h>
#include <minix/vm.h>
#include <sys/termios.h>

#include "ns16550.h"

/* UART MMIO base address (QEMU virt) */
#define UART_BASE       0x10000000UL
#define UART_SIZE       0x100
#define UART_IRQ        10

/* Driver name */
static const char *name = "ns16550";

/* UART MMIO base pointer */
static volatile u8_t *uart_base;

/* Current terminal settings */
static struct termios term;

/* Input buffer */
#define INBUF_SIZE      256
static char inbuf[INBUF_SIZE];
static int inbuf_head;
static int inbuf_tail;
static int inbuf_count;

/* IRQ hook */
static int irq_hook;

/* Forward declarations */
static int ns16550_open(devminor_t minor, int access, endpoint_t user);
static int ns16550_close(devminor_t minor);
static ssize_t ns16550_read(devminor_t minor, u64_t position, endpoint_t endpt,
    cp_grant_id_t grant, size_t size, int flags, cdev_id_t id);
static ssize_t ns16550_write(devminor_t minor, u64_t position, endpoint_t endpt,
    cp_grant_id_t grant, size_t size, int flags, cdev_id_t id);
static int ns16550_ioctl(devminor_t minor, unsigned long request, endpoint_t endpt,
    cp_grant_id_t grant, int flags, endpoint_t user, cdev_id_t id);
static void ns16550_intr(unsigned int irqs);

/* Character driver table */
static struct chardriver ns16550_tab = {
    .cdr_open   = ns16550_open,
    .cdr_close  = ns16550_close,
    .cdr_read   = ns16550_read,
    .cdr_write  = ns16550_write,
    .cdr_ioctl  = ns16550_ioctl,
};

/*
 * Read UART register
 */
static inline u8_t uart_read(int reg)
{
    return uart_base[reg];
}

/*
 * Write UART register
 */
static inline void uart_write(int reg, u8_t val)
{
    uart_base[reg] = val;
}

/*
 * Wait for transmit buffer empty
 */
static void uart_wait_tx(void)
{
    while ((uart_read(UART_LSR) & LSR_THRE) == 0)
        ;
}

/*
 * Output a character
 */
static void uart_putc(int c)
{
    uart_wait_tx();
    uart_write(UART_THR, c);
}

/*
 * Check if receive data available
 */
static int uart_rx_ready(void)
{
    return (uart_read(UART_LSR) & LSR_DR) != 0;
}

/*
 * Get a character (non-blocking)
 */
static int uart_getc(void)
{
    if (!uart_rx_ready())
        return -1;
    return uart_read(UART_RBR);
}

/*
 * Initialize UART hardware
 */
static int uart_hw_init(void)
{
    /* Map UART MMIO */
    uart_base = vm_map_phys(SELF, (void *)UART_BASE, UART_SIZE);
    if (uart_base == MAP_FAILED)
        return ENOMEM;

    /* Disable interrupts */
    uart_write(UART_IER, 0x00);

    /* Enable DLAB */
    uart_write(UART_LCR, LCR_DLAB);

    /* Set divisor (115200 baud) */
    uart_write(UART_DLL, 0x01);
    uart_write(UART_DLH, 0x00);

    /* 8N1 */
    uart_write(UART_LCR, LCR_WLS_8);

    /* Enable and clear FIFO */
    uart_write(UART_FCR, FCR_ENABLE | FCR_RXRST | FCR_TXRST | FCR_TRIGGER_14);

    /* Enable receive interrupt */
    uart_write(UART_IER, IER_ERBFI);

    /* Setup IRQ */
    irq_hook = UART_IRQ;
    if (sys_irqsetpolicy(UART_IRQ, 0, &irq_hook) != OK)
        return EIO;
    if (sys_irqenable(&irq_hook) != OK)
        return EIO;

    return OK;
}

/*
 * Open device
 */
static int ns16550_open(devminor_t minor, int access, endpoint_t user)
{
    if (minor != 0)
        return ENXIO;
    return OK;
}

/*
 * Close device
 */
static int ns16550_close(devminor_t minor)
{
    return OK;
}

/*
 * Read from device
 */
static ssize_t ns16550_read(devminor_t minor, u64_t position, endpoint_t endpt,
    cp_grant_id_t grant, size_t size, int flags, cdev_id_t id)
{
    size_t count = 0;
    char c;
    int r;

    if (minor != 0)
        return ENXIO;

    while (count < size && inbuf_count > 0) {
        c = inbuf[inbuf_tail];
        inbuf_tail = (inbuf_tail + 1) % INBUF_SIZE;
        inbuf_count--;

        r = sys_safecopyto(endpt, grant, count, (vir_bytes)&c, 1);
        if (r != OK)
            return r;
        count++;
    }

    if (count == 0 && !(flags & CDEV_NONBLOCK))
        return EDONTREPLY;  /* Block until data available */

    return count;
}

/*
 * Write to device
 */
static ssize_t ns16550_write(devminor_t minor, u64_t position, endpoint_t endpt,
    cp_grant_id_t grant, size_t size, int flags, cdev_id_t id)
{
    size_t i;
    char c;
    int r;

    if (minor != 0)
        return ENXIO;

    for (i = 0; i < size; i++) {
        r = sys_safecopyfrom(endpt, grant, i, (vir_bytes)&c, 1);
        if (r != OK)
            return r;

        if (c == '\n')
            uart_putc('\r');
        uart_putc(c);
    }

    return size;
}

/*
 * IOCTL handling
 */
static int ns16550_ioctl(devminor_t minor, unsigned long request, endpoint_t endpt,
    cp_grant_id_t grant, int flags, endpoint_t user, cdev_id_t id)
{
    int r;

    if (minor != 0)
        return ENXIO;

    switch (request) {
    case TCGETS:
        r = sys_safecopyto(endpt, grant, 0, (vir_bytes)&term, sizeof(term));
        break;

    case TCSETS:
    case TCSETSW:
    case TCSETSF:
        r = sys_safecopyfrom(endpt, grant, 0, (vir_bytes)&term, sizeof(term));
        break;

    default:
        r = ENOTTY;
    }

    return r;
}

/*
 * Interrupt handler
 */
static void ns16550_intr(unsigned int irqs)
{
    u8_t iir;
    int c;

    while (1) {
        iir = uart_read(UART_IIR);
        if (iir & IIR_PENDING)
            break;

        switch ((iir >> 1) & 0x07) {
        case IIR_RX_DATA:
        case IIR_RX_TIMEOUT:
            /* Receive data */
            while (uart_rx_ready()) {
                c = uart_getc();
                if (c >= 0 && inbuf_count < INBUF_SIZE) {
                    inbuf[inbuf_head] = c;
                    inbuf_head = (inbuf_head + 1) % INBUF_SIZE;
                    inbuf_count++;
                }
            }
            break;

        case IIR_TX_EMPTY:
            /* Transmit buffer empty - handled in write */
            break;

        case IIR_MODEM:
            /* Modem status change - read to clear */
            (void)uart_read(UART_MSR);
            break;

        case IIR_LINE:
            /* Line status - read to clear */
            (void)uart_read(UART_LSR);
            break;
        }
    }

    /* Re-enable IRQ */
    sys_irqenable(&irq_hook);
}

/*
 * Main entry point
 */
int main(void)
{
    message m;
    int r, ipc_status;

    /* Initialize driver */
    sef_startup();

    /* Initialize hardware */
    r = uart_hw_init();
    if (r != OK) {
        printf("%s: hardware init failed: %d\n", name, r);
        return 1;
    }

    /* Initialize input buffer */
    inbuf_head = 0;
    inbuf_tail = 0;
    inbuf_count = 0;

    /* Initialize termios */
    memset(&term, 0, sizeof(term));
    term.c_iflag = ICRNL;
    term.c_oflag = OPOST | ONLCR;
    term.c_cflag = CS8 | CREAD | CLOCAL;
    term.c_lflag = ECHO | ECHOE | ECHOK | ICANON | ISIG;
    term.c_cc[VINTR] = 0x03;    /* Ctrl-C */
    term.c_cc[VEOF] = 0x04;     /* Ctrl-D */
    cfsetispeed(&term, B115200);
    cfsetospeed(&term, B115200);

    printf("%s: UART driver started\n", name);

    /* Main loop */
    while (1) {
        r = sef_receive_status(ANY, &m, &ipc_status);
        if (r != OK) {
            printf("%s: receive error: %d\n", name, r);
            continue;
        }

        if (is_ipc_notify(ipc_status)) {
            switch (m.m_source) {
            case HARDWARE:
                ns16550_intr(m.m_notify.interrupts);
                break;
            default:
                printf("%s: unexpected notify from %d\n", name, m.m_source);
            }
            continue;
        }

        chardriver_process(&ns16550_tab, &m, ipc_status);
    }

    return 0;
}

/*
 * SEF callbacks
 */
static int sef_cb_init(int type, sef_init_info_t *info)
{
    return OK;
}

void sef_startup(void)
{
    sef_setcb_init_fresh(sef_cb_init);
    sef_startup_default();
}
