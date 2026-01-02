#include <minix/ipc.h>
#include <sys/ttycom.h>
#include <termios.h>

#include "tty.h"

void
rs_init(struct tty *tp)
{
	(void)tp;
}

void
rs_interrupt(message *m)
{
	(void)m;
}
