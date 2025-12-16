/*
 * RS232 support for RISC-V 64-bit
 * Implements RS232 line discipline for NS16550-compatible UART
 */

#include <minix/config.h>
#include <minix/drivers.h>
#include <minix/type.h>
#include <sys/types.h>
#include <termios.h>
#include <minix/com.h>
#include "tty.h"

#if NR_RS_LINES > 0

/* Include NS16550 definitions */
#include "../../../ns16550/ns16550.h"

/* UART base address for QEMU virt platform */
#define UART_BASE       0x10000000UL

/* Per-line state */
struct rs232_line {
    int initialized;
    u32_t baudrate;
    u8_t lcr;     /* Line control register */
    u8_t mcr;     /* Modem control register */
};

static struct rs232_line rs232_lines[NR_RS_LINES];

/*
 * Helper functions for UART register access
 */
static inline u8_t uart_read(int reg)
{
    volatile u8_t *uart = (volatile u8_t *)UART_BASE;
    return uart[reg];
}

static inline void uart_write(int reg, u8_t val)
{
    volatile u8_t *uart = (volatile u8_t *)UART_BASE;
    uart[reg] = val;
}

/*
 * Convert termios speed to baud rate divisor
 */
static u16_t speed_to_divisor(speed_t speed)
{
    /* Base clock frequency for QEMU virt UART */
    const u32_t base_freq = 1843200;
    u16_t divisor;

    switch (speed) {
    case B0:      return 0;
    case B50:     return base_freq / (50 * 16);
    case B75:     return base_freq / (75 * 16);
    case B110:    return base_freq / (110 * 16);
    case B134:    return base_freq / (134.5 * 16);
    case B150:    return base_freq / (150 * 16);
    case B200:    return base_freq / (200 * 16);
    case B300:    return base_freq / (300 * 16);
    case B600:    return base_freq / (600 * 16);
    case B1200:   return base_freq / (1200 * 16);
    case B1800:   return base_freq / (1800 * 16);
    case B2400:   return base_freq / (2400 * 16);
    case B4800:   return base_freq / (4800 * 16);
    case B9600:   return base_freq / (9600 * 16);
    case B19200:  return base_freq / (19200 * 16);
    case B38400:  return base_freq / (38400 * 16);
    case B57600:  return base_freq / (57600 * 16);
    case B115200: return base_freq / (115200 * 16);
    default:      return 1;  /* Default to 115200 */
    }
}

/*
 * Initialize RS232 line
 */
void rs232_init(tp)
tty_t *tp;
{
    struct rs232_line *line;

    if (tp->tty_line < 0 || tp->tty_line >= NR_RS_LINES)
        return;

    line = &rs232_lines[tp->tty_line];

    if (!line->initialized) {
        /* Initialize hardware */
        uart_write(UART_IER, 0x00);  /* Disable interrupts */

        /* Default to 115200 8N1 */
        line->baudrate = 115200;
        line->lcr = LCR_WLS_8;
        line->mcr = MCR_DTR | MCR_RTS | MCR_OUT2;

        /* Set baud rate */
        uart_write(UART_LCR, LCR_DLAB);
        uart_write(UART_DLL, 1);    /* 115200 divisor */
        uart_write(UART_DLH, 0);
        uart_write(UART_LCR, line->lcr);

        /* Enable FIFO */
        uart_write(UART_FCR, FCR_ENABLE | FCR_RXRST | FCR_TXRST | FCR_TRIGGER_14);

        /* Set modem control */
        uart_write(UART_MCR, line->mcr);

        /* Enable receive interrupts */
        uart_write(UART_IER, IER_ERBFI);

        line->initialized = 1;
    }
}

/*
 * Set line parameters from termios
 */
void rs232_param(tp)
tty_t *tp;
{
    struct rs232_line *line;
    u8_t new_lcr;
    u16_t divisor;

    if (tp->tty_line < 0 || tp->tty_line >= NR_RS_LINES)
        return;

    line = &rs232_lines[tp->tty_line];
    if (!line->initialized)
        return;

    /* Build new LCR */
    new_lcr = 0;

    /* Data bits */
    switch (tp->tty_termios.c_cflag & CSIZE) {
    case CS5: new_lcr |= LCR_WLS_5; break;
    case CS6: new_lcr |= LCR_WLS_6; break;
    case CS7: new_lcr |= LCR_WLS_7; break;
    case CS8:
    default:  new_lcr |= LCR_WLS_8; break;
    }

    /* Stop bits */
    if (tp->tty_termios.c_cflag & CSTOPB)
        new_lcr |= LCR_STB;

    /* Parity */
    if (tp->tty_termios.c_cflag & PARENB) {
        new_lcr |= LCR_PEN;
        if (!(tp->tty_termios.c_cflag & PARODD))
            new_lcr |= LCR_EPS;
    }

    /* Update baud rate if changed */
    divisor = speed_to_divisor(cfgetispeed(&tp->tty_termios));
    if (divisor != 0) {
        uart_write(UART_LCR, LCR_DLAB);
        uart_write(UART_DLL, divisor & 0xFF);
        uart_write(UART_DLH, (divisor >> 8) & 0xFF);
    }

    /* Set new line control */
    uart_write(UART_LCR, new_lcr);
    line->lcr = new_lcr;
}

/*
 * Output handling
 */
void rs232_write(tp, try)
tty_t *tp;
int try;
{
    int count;
    char buf[64];

    while ((count = q_size(&tp->tty_outq)) > 0) {
        int n = count;
        if (n > sizeof(buf))
            n = sizeof(buf);

        /* Extract from queue */
        n = q_get(&tp->tty_outq, buf, n);

        /* Output to UART */
        for (int i = 0; i < n; i++) {
            /* Wait for transmitter ready */
            while ((uart_read(UART_LSR) & LSR_THRE) == 0) {
                if (try) {
                    /* Put back remaining data and exit */
                    q_put(&tp->tty_outq, &buf[i], n - i);
                    return;
                }
            }

            /* Send character */
            uart_write(UART_THR, buf[i]);
        }
    }
}

/*
 * Echo handling (local echo)
 */
void rs232_echo(tp)
tty_t *tp;
{
    char c;

    while (tp->tty_incount > 0) {
        /* Get character to echo */
        c = tp->tty_inhead[0];
        tp->tty_inhead++;
        tp->tty_incount--;
        if (tp->tty_inhead >= tp->tty_inbuf + tp->tty_insize)
            tp->tty_inhead = tp->tty_inbuf;

        /* Echo back */
        (void)out_char(tp, c);
    }
}

/*
 * Stop output
 */
void rs232_stop(tp)
tty_t *tp;
{
    struct rs232_line *line;

    if (tp->tty_line < 0 || tp->tty_line >= NR_RS_LINES)
        return;

    line = &rs232_lines[tp->tty_line];
    if (!line->initialized)
        return;

    /* Clear RTS to stop remote */
    line->mcr &= ~MCR_RTS;
    uart_write(UART_MCR, line->mcr);
}

/*
 * Start output
 */
void rs232_start(tp)
tty_t *tp;
{
    struct rs232_line *line;

    if (tp->tty_line < 0 || tp->tty_line >= NR_RS_LINES)
        return;

    line = &rs232_lines[tp->tty_line];
    if (!line->initialized)
        return;

    /* Set RTS to allow remote */
    line->mcr |= MCR_RTS;
    uart_write(UART_MCR, line->mcr);

    /* Try to start output */
    rs232_write(tp, 1);
}

/*
 * Cancel input
 */
void rs232_icancel(tp)
tty_t *tp;
{
    /* Clear receive FIFO */
    uart_write(UART_FCR, FCR_ENABLE | FCR_RXRST);

    /* Clear input queue */
    q_clear(&tp->tty_inq);
}

/*
 * Cancel output
 */
void rs232_ocancel(tp)
tty_t *tp;
{
    /* Clear output queue */
    q_clear(&tp->tty_outq);

    /* Clear transmit FIFO */
    uart_write(UART_FCR, FCR_ENABLE | FCR_TXRST);
}

/*
 * Send break
 */
void rs232_break(tp)
tty_t *tp;
{
    /* Set break in LCR */
    u8_t lcr = uart_read(UART_LCR);
    uart_write(UART_LCR, lcr | LCR_BREAK);

    /* Wait a bit */
    micro_delay(250000);  /* 250ms */

    /* Clear break */
    uart_write(UART_LCR, lcr);
}

/*
 * IOCTL handling
 */
void rs232_ioctl(tp)
tty_t *tp;
{
    /* Handle RS232-specific ioctls */
    switch (tp->tty_request) {
    case TIOCSBRK:    /* Set break */
        rs232_break(tp);
        tp->tty_reply = OK;
        break;

    case TIOCCBRK:    /* Clear break */
        /* Already cleared */
        tp->tty_reply = OK;
        break;

    case TIOCMBIC:    /* Clear modem bits */
        /* Not implemented */
        tp->tty_reply = ENOTTY;
        break;

    case TIOCMBIS:    /* Set modem bits */
        /* Not implemented */
        tp->tty_reply = ENOTTY;
        break;

    case TIOCMGET:    /* Get modem bits */
        /* Return basic status */
        tp->tty_tiocmget = TIOCM_DTR | TIOCM_RTS | TIOCM_CTS;
        tp->tty_reply = OK;
        break;

    default:
        tp->tty_reply = ENOTTY;
    }
}

/*
 * Interrupt handling
 */
void rs232_int(tp)
tty_t *tp;
{
    u8_t iir, lsr;
    int c;

    if (tp->tty_line < 0 || tp->tty_line >= NR_RS_LINES)
        return;

    /* Handle all pending interrupts */
    while (1) {
        iir = uart_read(UART_IIR);
        if (iir & IIR_PENDING)
            break;

        switch ((iir >> 1) & 0x07) {
        case IIR_RX_DATA:
        case IIR_RX_TIMEOUT:
            /* Received data available */
            while ((lsr = uart_read(UART_LSR)) & LSR_DR) {
                c = uart_read(UART_RBR);

                /* Check for errors */
                if (lsr & (LSR_OE | LSR_PE | LSR_FE | LSR_BI)) {
                    /* Handle error */
                    if (lsr & LSR_BI)
                        in_ready(tp);
                    continue;
                }

                /* Input character */
                in_char(tp, c);
            }
            break;

        case IIR_TX_EMPTY:
            /* Transmitter empty - try to send more */
            rs232_write(tp, 1);
            break;

        case IIR_MODEM:
            /* Modem status change */
            (void)uart_read(UART_MSR);
            break;

        case IIR_LINE:
            /* Line status change */
            (void)uart_read(UART_LSR);
            break;
        }
    }
}

/*
 * Reset line
 */
void rs232_reset(tp)
tty_t *tp;
{
    struct rs232_line *line;

    if (tp->tty_line < 0 || tp->tty_line >= NR_RS_LINES)
        return;

    line = &rs232_lines[tp->tty_line];
    line->initialized = 0;

    /* Reinitialize */
    rs232_init(tp);
}

#endif /* NR_RS_LINES > 0 */