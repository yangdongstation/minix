/*
 * NS16550A UART register definitions
 */

#ifndef _NS16550_H
#define _NS16550_H

/* Register offsets */
#define UART_RBR    0x00    /* Receive Buffer Register (read, DLAB=0) */
#define UART_THR    0x00    /* Transmit Holding Register (write, DLAB=0) */
#define UART_DLL    0x00    /* Divisor Latch Low (DLAB=1) */
#define UART_IER    0x01    /* Interrupt Enable Register (DLAB=0) */
#define UART_DLH    0x01    /* Divisor Latch High (DLAB=1) */
#define UART_IIR    0x02    /* Interrupt Identification Register (read) */
#define UART_FCR    0x02    /* FIFO Control Register (write) */
#define UART_LCR    0x03    /* Line Control Register */
#define UART_MCR    0x04    /* Modem Control Register */
#define UART_LSR    0x05    /* Line Status Register */
#define UART_MSR    0x06    /* Modem Status Register */
#define UART_SCR    0x07    /* Scratch Register */

/* IER bits */
#define IER_ERBFI   0x01    /* Enable Received Data Available Interrupt */
#define IER_ETBEI   0x02    /* Enable Transmitter Holding Register Empty Interrupt */
#define IER_ELSI    0x04    /* Enable Receiver Line Status Interrupt */
#define IER_EDSSI   0x08    /* Enable Modem Status Interrupt */

/* IIR bits */
#define IIR_PENDING     0x01    /* 0 = interrupt pending */
#define IIR_ID_MASK     0x0E    /* Interrupt ID */
#define IIR_MODEM       0x00    /* Modem status */
#define IIR_TX_EMPTY    0x01    /* Transmitter holding register empty */
#define IIR_RX_DATA     0x02    /* Received data available */
#define IIR_LINE        0x03    /* Receiver line status */
#define IIR_RX_TIMEOUT  0x06    /* Character timeout */

/* FCR bits */
#define FCR_ENABLE      0x01    /* Enable FIFOs */
#define FCR_RXRST       0x02    /* Reset receive FIFO */
#define FCR_TXRST       0x04    /* Reset transmit FIFO */
#define FCR_DMA         0x08    /* DMA mode select */
#define FCR_TRIGGER_1   0x00    /* Receive trigger level 1 */
#define FCR_TRIGGER_4   0x40    /* Receive trigger level 4 */
#define FCR_TRIGGER_8   0x80    /* Receive trigger level 8 */
#define FCR_TRIGGER_14  0xC0    /* Receive trigger level 14 */

/* LCR bits */
#define LCR_WLS_5       0x00    /* 5 data bits */
#define LCR_WLS_6       0x01    /* 6 data bits */
#define LCR_WLS_7       0x02    /* 7 data bits */
#define LCR_WLS_8       0x03    /* 8 data bits */
#define LCR_STB         0x04    /* 2 stop bits */
#define LCR_PEN         0x08    /* Parity enable */
#define LCR_EPS         0x10    /* Even parity */
#define LCR_STICK       0x20    /* Stick parity */
#define LCR_BREAK       0x40    /* Set break */
#define LCR_DLAB        0x80    /* Divisor latch access bit */

/* MCR bits */
#define MCR_DTR         0x01    /* Data Terminal Ready */
#define MCR_RTS         0x02    /* Request To Send */
#define MCR_OUT1        0x04    /* Output 1 */
#define MCR_OUT2        0x08    /* Output 2 (interrupt enable) */
#define MCR_LOOP        0x10    /* Loopback mode */

/* LSR bits */
#define LSR_DR          0x01    /* Data Ready */
#define LSR_OE          0x02    /* Overrun Error */
#define LSR_PE          0x04    /* Parity Error */
#define LSR_FE          0x08    /* Framing Error */
#define LSR_BI          0x10    /* Break Interrupt */
#define LSR_THRE        0x20    /* Transmitter Holding Register Empty */
#define LSR_TEMT        0x40    /* Transmitter Empty */
#define LSR_RXFE        0x80    /* Receiver FIFO Error */

/* MSR bits */
#define MSR_DCTS        0x01    /* Delta CTS */
#define MSR_DDSR        0x02    /* Delta DSR */
#define MSR_TERI        0x04    /* Trailing Edge Ring Indicator */
#define MSR_DDCD        0x08    /* Delta DCD */
#define MSR_CTS         0x10    /* Clear To Send */
#define MSR_DSR         0x20    /* Data Set Ready */
#define MSR_RI          0x40    /* Ring Indicator */
#define MSR_DCD         0x80    /* Data Carrier Detect */

#endif /* _NS16550_H */
