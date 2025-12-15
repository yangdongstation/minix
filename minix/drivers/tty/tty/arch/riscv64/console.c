/*
 * RISC-V 64-bit console driver for MINIX
 * Simplified version for QEMU virt platform using SBI console
 */

#include <minix/drivers.h>
#include <termios.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <sys/video.h>
#include <sys/mman.h>
#include <sys/termios.h>
#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/sys_config.h>
#include <minix/vm.h>
#include "tty.h"

/* RISC-V SBI console output */
static void sbi_console_putchar(char c)
{
	register long a7 __asm__("a7") = 1; /* SBI_CONSOLE_PUTCHAR */
	register long a0 __asm__("a0") = c;
	__asm__ volatile ("ecall"
		: "+r"(a0)
		: "r"(a7)
		: "memory");
}

/* Screen dimensions */
#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 25

/* Per console data structure */
typedef struct console {
	tty_t *c_tty;		/* associated TTY struct */
	int c_column;		/* current column number (0-origin) */
	int c_row;		/* current row (0 at top of screen) */
	int c_esc_state;	/* 0=normal, 1=ESC, 2=ESC[ */
	char c_reverse;		/* reverse video */
	char c_esc_intro;	/* Distinguishing character following ESC */
	int *c_esc_parmp;	/* pointer to current escape parameter */
	int c_esc_parmv[MAX_ESC_PARMS];	/* list of escape parameters */
	u16_t c_attr;		/* character attribute */
	u16_t c_blank;		/* blank attribute */
} console_t;

static int nr_cons = 1;
static console_t cons_table[NR_CONS];
static console_t *curcons = NULL;
static int shutting_down = FALSE;

static int cons_write(struct tty *tp, int try);
static void cons_echo(tty_t *tp, int c);
static void out_char(console_t *cons, int c);
static void parse_escape(console_t *cons, int c);
static void flush(console_t *cons);
static int cons_ioctl(tty_t *tp, int);
static void set_cursor(console_t *cons);
static void clear_screen(console_t *cons);

/* ANSI color mapping for light gray on black */
#define ANSI_COLOR_LIGHTGRAY 7

/*===========================================================================*
 *				cons_write				     *
 *===========================================================================*/
static int cons_write(tp, try)
register struct tty *tp;	/* tells which terminal is to be used */
int try;
{
	int count;
	int result = OK;
	char buf[64];
	console_t *cons = tp->tty_priv;

	if (try) return 1;	/* we can always write to console */

	if ((count = tp->tty_outleft) == 0 || tp->tty_inhibited) return 0;

	do {
		if (count > sizeof(buf)) count = sizeof(buf);
		if (tp->tty_outcaller == KERNEL) {
			memcpy(buf, (char *) tp->tty_outgrant + tp->tty_outcum, count);
		} else {
			if ((result = sys_safecopyfrom(tp->tty_outcaller,
					tp->tty_outgrant, tp->tty_outcum,
					(vir_bytes) buf, count)) != OK) {
				break;
			}
		}

		/* Output each byte */
		for (int i = 0; i < count; i++) {
			out_char(cons, buf[i]);
		}

		tp->tty_outcum += count;
		tp->tty_outleft -= count;
	} while ((count = tp->tty_outleft) != 0 && !tp->tty_inhibited);

	flush(cons);

	/* Reply to the writer if all output is finished or if an error occurred. */
	if (tp->tty_outleft == 0 || result != OK) {
		if (tp->tty_outcaller != KERNEL)
			chardriver_reply_task(tp->tty_outcaller, tp->tty_outid,
				result != OK ? result : tp->tty_outcum);
		tp->tty_outcum = tp->tty_outleft = 0;
		tp->tty_outcaller = NONE;
	}

	return 0;
}

/*===========================================================================*
 *				cons_echo				     *
 *===========================================================================*/
static void cons_echo(tp, c)
register tty_t *tp;		/* pointer to tty struct */
int c;				/* character to be echoed */
{
	console_t *cons = tp->tty_priv;
	out_char(cons, c);
	flush(cons);
}

/*===========================================================================*
 *				out_char				     *
 *===========================================================================*/
static void out_char(cons, c)
register console_t *cons;	/* pointer to console struct */
int c;				/* character to be output */
{
	if (cons->c_esc_state > 0) {
		parse_escape(cons, c);
		return;
	}

	switch(c) {
	case 000:		/* null is typically used for padding */
		return;

	case 007:		/* bell - just ignore for now */
		return;

	case '\b':		/* backspace */
		if (cons->c_column > 0) cons->c_column--;
		return;

	case '\n':		/* line feed */
		cons->c_row++;
		if (cons->c_row >= SCREEN_HEIGHT) {
			cons->c_row = SCREEN_HEIGHT - 1;
			/* Scroll up - for now just move to top */
			cons->c_row = 0;
		}
		cons->c_column = 0;
		return;

	case '\r':		/* carriage return */
		cons->c_column = 0;
		return;

	case '\t':		/* tab */
		cons->c_column = (cons->c_column + 8) & ~7;
		if (cons->c_column >= SCREEN_WIDTH) {
			cons->c_column = 0;
			cons->c_row++;
			if (cons->c_row >= SCREEN_HEIGHT) {
				cons->c_row = SCREEN_HEIGHT - 1;
				cons->c_row = 0;
			}
		}
		return;

	case 033:		/* ESC - start of an escape sequence */
		cons->c_esc_state = 1;	/* mark ESC as seen */
		return;

	default:		/* printable chars */
		if (c >= ' ' && c <= 126) {
			if (cons->c_column >= SCREEN_WIDTH) {
				cons->c_column = 0;
				cons->c_row++;
				if (cons->c_row >= SCREEN_HEIGHT) {
					cons->c_row = SCREEN_HEIGHT - 1;
					cons->c_row = 0;
				}
			}
			sbi_console_putchar(c);
			cons->c_column++;
		}
		return;
	}
}

/*===========================================================================*
 *				flush					     *
 *===========================================================================*/
static void flush(cons)
register console_t *cons;	/* pointer to console struct */
{
	/* Update cursor position */
	set_cursor(cons);
}

/*===========================================================================*
 *				set_cursor				     *
 *===========================================================================*/
static void set_cursor(cons)
register console_t *cons;	/* pointer to console struct */
{
	/* For simplicity, we don't implement cursor positioning */
	/* In a real implementation, you would use ANSI escape codes */
}

/*===========================================================================*
 *				clear_screen				     *
 *===========================================================================*/
static void clear_screen(cons)
register console_t *cons;	/* pointer to console struct */
{
	/* Clear screen using ANSI escape code */
	const char *clear = "\033[2J\033[H";
	while (*clear) {
		sbi_console_putchar(*clear++);
	}
	cons->c_row = 0;
	cons->c_column = 0;
}

/*===========================================================================*
 *				parse_escape				     *
 *===========================================================================*/
static void parse_escape(cons, c)
register console_t *cons;	/* pointer to console struct */
char c;				/* next character in escape sequence */
{
	switch (cons->c_esc_state) {
	case 1:			/* ESC seen */
		cons->c_esc_intro = '\0';
		cons->c_esc_parmp = cons->c_esc_parmv + MAX_ESC_PARMS - 1;
		*cons->c_esc_parmp = 0;
		switch (c) {
		case '[':	/* Control Sequence Introducer */
			cons->c_esc_intro = c;
			cons->c_esc_state = 2;
			break;
		default:
			cons->c_esc_state = 0;
		}
		break;

	case 2:			/* ESC [ seen */
		if (c >= '0' && c <= '9') {
			if (cons->c_esc_parmp > cons->c_esc_parmv) {
				*cons->c_esc_parmp = *cons->c_esc_parmp * 10 + (c-'0');
			}
		} else if (c == ';') {
			if (cons->c_esc_parmp > cons->c_esc_parmv) {
				cons->c_esc_parmp--;
			}
		} else {
			/* Handle the escape sequence */
			switch (c) {
			case 'J':	/* Clear screen */
				if (cons->c_esc_parmv[MAX_ESC_PARMS - 1] == 2) {
					clear_screen(cons);
				}
				break;
			case 'H':	/* Move cursor */
			case 'm':	/* Set attributes - ignore for now */
			default:
				break;
			}
			cons->c_esc_state = 0;
		}
		break;
	}
}

/*===========================================================================*
 *				cons_ioctl				     *
 *===========================================================================*/
static int cons_ioctl(tty_t *tp, int UNUSED(try))
{
	/* Set the screen dimensions. */
	tp->tty_winsize.ws_row = SCREEN_HEIGHT;
	tp->tty_winsize.ws_col = SCREEN_WIDTH;
	tp->tty_winsize.ws_xpixel = SCREEN_WIDTH * 8;
	tp->tty_winsize.ws_ypixel = SCREEN_HEIGHT * 16;

	return 0;
}

/*===========================================================================*
 *				scr_init				     *
 *===========================================================================*/
void scr_init(tp)
tty_t *tp;
{
	console_t *cons;
	int line;

	/* Associate console and TTY. */
	line = tp - &tty_table[0];
	if (line >= nr_cons) return;
	cons = &cons_table[line];
	cons->c_tty = tp;
	tp->tty_priv = cons;

	/* Fill in TTY function hooks. */
	tp->tty_devwrite = cons_write;
	tp->tty_echo = cons_echo;
	tp->tty_ioctl = cons_ioctl;

	/* Initialize console state */
	cons->c_column = 0;
	cons->c_row = 0;
	cons->c_esc_state = 0;
	cons->c_attr = cons->c_blank = ANSI_COLOR_LIGHTGRAY;
	cons->c_reverse = FALSE;

	if (line == 0) {
		curcons = cons;
		clear_screen(cons);
	}
}

/*===========================================================================*
 *				cons_stop				     *
 *===========================================================================*/
void cons_stop()
{
	shutting_down = TRUE;
}

/*===========================================================================*
 *				select_console				     *
 *===========================================================================*/
void select_console(cons_line)
int cons_line;
{
	if (shutting_down) return;
	if (cons_line < 0 || cons_line >= nr_cons) return;
	curcons = &cons_table[cons_line];
}

/*===========================================================================*
 *				do_video				     *
 *===========================================================================*/
void do_video(message *m, int ipc_status)
{
	/* No video driver support for RISC-V console */
}

/*===========================================================================*
 *				beep_x					     *
 *===========================================================================*/
void beep_x(freq, dur)
unsigned freq;
clock_t dur;
{
	/* No beep support for RISC-V console */
}