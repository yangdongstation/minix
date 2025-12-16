/*
 * Direct TTY utilities for early boot and debugging
 *
 * Uses SBI console for early boot, switches to UART when available
 */

#include "kernel/kernel.h"
#include "archconst.h"
#include "arch_proto.h"
#include <stdarg.h>

/* Flag to indicate if UART is available */
static int uart_ready = 0;

/*
 * Clear screen (not really possible via SBI, just print newlines)
 */
void direct_cls(void)
{
    int i;
    for (i = 0; i < 25; i++) {
        sbi_console_putchar('\n');
    }
}

/*
 * Print a string directly via SBI
 */
void direct_print(const char *str)
{
    while (*str) {
        direct_print_char(*str++);
    }
}

/*
 * Initialize direct output system
 */
void direct_init(void)
{
    riscv_cons_init();
    uart_ready = 1;
}

/*
 * Print a single character
 */
void direct_print_char(int c)
{
    if (uart_ready) {
        /* Use UART for faster output */
        riscv_cons_putc(c);
    } else {
        /* Fall back to SBI */
        if (c == '\n') {
            sbi_console_putchar('\r');
        }
        sbi_console_putchar(c);
    }
}

/*
 * Read a character
 * Returns character or -1 if none available
 */
int direct_read_char(void)
{
    if (uart_ready) {
        return riscv_cons_getc();
    } else {
        return sbi_console_getchar();
    }
}

/*
 * Print a number in hex
 */
void direct_print_hex(u64_t val)
{
    static const char hex[] = "0123456789abcdef";
    int i;
    int started = 0;

    direct_print("0x");

    for (i = 60; i >= 0; i -= 4) {
        int digit = (val >> i) & 0xF;
        if (digit != 0 || started || i == 0) {
            direct_print_char(hex[digit]);
            started = 1;
        }
    }
}

/*
 * Print a number in decimal
 */
void direct_print_dec(u64_t val)
{
    char buf[21];
    int i = 20;

    buf[i] = '\0';

    if (val == 0) {
        direct_print_char('0');
        return;
    }

    while (val > 0 && i > 0) {
        buf[--i] = '0' + (val % 10);
        val /= 10;
    }

    direct_print(&buf[i]);
}

/*
 * Simple printf-like function for early debugging
 */
void direct_printf(const char *fmt, ...)
{
    /* Very simple implementation - only supports %s, %x, %d, %p */
    va_list ap;
    va_start(ap, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
            case 's':
                direct_print(va_arg(ap, const char *));
                break;
            case 'x':
            case 'p':
                direct_print_hex(va_arg(ap, u64_t));
                break;
            case 'd':
                direct_print_dec(va_arg(ap, u64_t));
                break;
            case '%':
                direct_print_char('%');
                break;
            case '\0':
                goto done;
            default:
                direct_print_char('%');
                direct_print_char(*fmt);
            }
        } else {
            direct_print_char(*fmt);
        }
        fmt++;
    }

done:
    va_end(ap);
}
