/*
 * RISC-V 64 serial output for debugging
 *
 * Uses SBI console for output
 */

#include "sysutil.h"

/*
 * SBI ecall for console output
 */
static void sbi_putchar(int ch)
{
    register unsigned long a0 __asm__("a0") = ch;
    register unsigned long a7 __asm__("a7") = 0x01;  /* SBI_CONSOLE_PUTCHAR */

    __asm__ __volatile__(
        "ecall"
        : "+r"(a0)
        : "r"(a7)
        : "memory"
    );
}

/*
 * Output a character to serial console
 */
void ser_putc(char c)
{
    if (c == '\n')
        sbi_putchar('\r');
    sbi_putchar(c);
}
