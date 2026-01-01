/*
 * QEMU virt NS16550A UART driver
 *
 * UART base address: 0x10000000
 * IRQ: 10 (PLIC source)
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"

/* NS16550A register offsets */
#define UART_RBR    0x00    /* Receive Buffer Register (read) */
#define UART_THR    0x00    /* Transmit Holding Register (write) */
#define UART_IER    0x01    /* Interrupt Enable Register */
#define UART_IIR    0x02    /* Interrupt Identification Register (read) */
#define UART_FCR    0x02    /* FIFO Control Register (write) */
#define UART_LCR    0x03    /* Line Control Register */
#define UART_MCR    0x04    /* Modem Control Register */
#define UART_LSR    0x05    /* Line Status Register */
#define UART_MSR    0x06    /* Modem Status Register */
#define UART_SCR    0x07    /* Scratch Register */
#define UART_DLL    0x00    /* Divisor Latch Low (when DLAB=1) */
#define UART_DLH    0x01    /* Divisor Latch High (when DLAB=1) */

/* Line Status Register bits */
#define LSR_DR      0x01    /* Data Ready */
#define LSR_OE      0x02    /* Overrun Error */
#define LSR_PE      0x04    /* Parity Error */
#define LSR_FE      0x08    /* Framing Error */
#define LSR_BI      0x10    /* Break Interrupt */
#define LSR_THRE    0x20    /* Transmit Holding Register Empty */
#define LSR_TEMT    0x40    /* Transmitter Empty */
#define LSR_RXFE    0x80    /* Receiver FIFO Error */

/* Line Control Register bits */
#define LCR_WLS_5   0x00    /* 5 bits */
#define LCR_WLS_6   0x01    /* 6 bits */
#define LCR_WLS_7   0x02    /* 7 bits */
#define LCR_WLS_8   0x03    /* 8 bits */
#define LCR_STB     0x04    /* 2 stop bits */
#define LCR_PEN     0x08    /* Parity Enable */
#define LCR_EPS     0x10    /* Even Parity Select */
#define LCR_DLAB    0x80    /* Divisor Latch Access Bit */

/* FIFO Control Register bits */
#define FCR_ENABLE  0x01    /* FIFO Enable */
#define FCR_RXRST   0x02    /* Receiver FIFO Reset */
#define FCR_TXRST   0x04    /* Transmitter FIFO Reset */

/* Interrupt Enable Register bits */
#define IER_ERBFI   0x01    /* Enable Received Data Available Interrupt */
#define IER_ETBEI   0x02    /* Enable Transmit Holding Register Empty Interrupt */

/* UART base address */
static volatile u8_t *uart_base = (volatile u8_t *)UART0_BASE;

/* UART initialized flag */
static int uart_initialized = 0;

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
 * Initialize UART
 */
void bsp_serial_init(void)
{
    /* Disable interrupts */
    uart_write(UART_IER, 0x00);

    /* Enable DLAB to set baud rate */
    uart_write(UART_LCR, LCR_DLAB);

    /* Set divisor (115200 baud with 1.8432 MHz clock would be 1) */
    /* QEMU doesn't care about actual baud rate */
    uart_write(UART_DLL, 0x01);
    uart_write(UART_DLH, 0x00);

    /* 8 bits, no parity, 1 stop bit */
    uart_write(UART_LCR, LCR_WLS_8);

    /* Enable and clear FIFO */
    uart_write(UART_FCR, FCR_ENABLE | FCR_RXRST | FCR_TXRST);

    /* Enable receive interrupt */
    uart_write(UART_IER, IER_ERBFI);

    uart_initialized = 1;
}

/*
 * Output a character
 */
void bsp_serial_putc(int c)
{
    if (!uart_initialized) {
        /* Fall back to SBI console */
        sbi_console_putchar(c);
        return;
    }

    /* Wait for transmit buffer to be empty */
    while ((uart_read(UART_LSR) & LSR_THRE) == 0)
        ;

    /* Send character */
    uart_write(UART_THR, c);
}

/*
 * Input a character (non-blocking)
 * Returns -1 if no character available
 */
int bsp_serial_getc(void)
{
    if (!uart_initialized) {
        return sbi_console_getchar();
    }

    /* Check if data is ready */
    if ((uart_read(UART_LSR) & LSR_DR) == 0)
        return -1;

    return uart_read(UART_RBR);
}

/*
 * Check if transmit buffer is empty
 */
int bsp_serial_tx_ready(void)
{
    if (!uart_initialized)
        return 1;
    return (uart_read(UART_LSR) & LSR_THRE) != 0;
}

/*
 * Check if receive data is available
 */
int bsp_serial_rx_ready(void)
{
    if (!uart_initialized)
        return 0;
    return (uart_read(UART_LSR) & LSR_DR) != 0;
}

/*
 * UART interrupt handler
 */
void bsp_serial_intr(void)
{
    u8_t iir = uart_read(UART_IIR);

    /* Check interrupt pending bit (0 = pending) */
    if (iir & 0x01)
        return;

    /* Handle based on interrupt type */
    switch ((iir >> 1) & 0x07) {
    case 0x02:  /* Received Data Available */
    case 0x06:  /* Character Timeout */
        /* Read character to clear interrupt */
        /* TODO: Pass to TTY driver */
        (void)uart_read(UART_RBR);
        break;

    case 0x01:  /* Transmit Holding Register Empty */
        /* TODO: Send next character from buffer */
        break;

    default:
        break;
    }
}
