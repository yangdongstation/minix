/*
 * Early console support for RISC-V 64-bit
 * Used before TTY system is initialized
 */

#include <minix/type.h>
#include <minix/com.h>
#include "kernel.h"
#include "archconst.h"

/* UART base address */
#define UART_BASE       0x10000000UL

/* UART register offsets */
#define UART_RBR    0x00    /* Receive Buffer Register */
#define UART_THR    0x00    /* Transmit Holding Register */
#define UART_DLL    0x00    /* Divisor Latch Low */
#define UART_DLH    0x01    /* Divisor Latch High */
#define UART_IER    0x01    /* Interrupt Enable Register */
#define UART_IIR    0x02    /* Interrupt Identification Register */
#define UART_FCR    0x02    /* FIFO Control Register */
#define UART_LCR    0x03    /* Line Control Register */
#define UART_MCR    0x04    /* Modem Control Register */
#define UART_LSR    0x05    /* Line Status Register */

/* LSR bits */
#define LSR_THRE    0x20    /* Transmitter Holding Register Empty */
#define LSR_DR      0x01    /* Data Ready */

/* LCR bits */
#define LCR_WLS_8   0x03    /* 8 data bits */
#define LCR_DLAB    0x80    /* Divisor latch access bit */

/*
 * Early console initialization
 */
void riscv_cons_init(void)
{
    volatile u8_t *uart = (volatile u8_t *)UART_BASE;

    /* Disable interrupts */
    uart[UART_IER] = 0x00;

    /* Set baud rate to 115200 */
    uart[UART_LCR] = LCR_DLAB;
    uart[UART_DLL] = 0x01;    /* Divisor for 115200 */
    uart[UART_DLH] = 0x00;

    /* 8N1 configuration */
    uart[UART_LCR] = LCR_WLS_8;

    /* Enable FIFO */
    uart[UART_FCR] = 0x01;    /* Enable FIFO */
}

/*
 * Output a character to early console
 */
void riscv_cons_putc(int c)
{
    volatile u8_t *uart = (volatile u8_t *)UART_BASE;

    /* Wait for transmitter ready */
    while ((uart[UART_LSR] & LSR_THRE) == 0)
        ;

    /* Send character */
    uart[UART_THR] = c;
}

/*
 * Input a character from early console (non-blocking)
 */
int riscv_cons_getc(void)
{
    volatile u8_t *uart = (volatile u8_t *)UART_BASE;

    /* Check if data available */
    if (uart[UART_LSR] & LSR_DR)
        return uart[UART_RBR];

    return -1;
}