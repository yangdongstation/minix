#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <minix/sysutil.h>

#include "syslib.h"

void panic_hook(void);

__weak_alias(panic_hook, __panic_hook);

void __panic_hook(void)
{
	;
}

/*===========================================================================*
 *				panic				     *
 *===========================================================================*/
void panic(const char *fmt, ...)
{
/* Something awful has happened. Panics are caused when an internal
 * inconsistency is detected, e.g., a programming error or illegal 
 * value of a defined constant.
 */
  endpoint_t me = NONE;
  char name[20];
  int priv_flags;
  int init_flags;
  void (*suicide)(void);
  va_list args;
  char diagbuf[256];
  int diaglen = 0;

  if(sys_whoami(&me, name, sizeof(name), &priv_flags, &init_flags) == OK && me != NONE)
	diaglen = snprintf(diagbuf, sizeof(diagbuf), "%s(%d): panic: ",
	    name, me);
  else
	diaglen = snprintf(diagbuf, sizeof(diagbuf),
	    "(sys_whoami failed): panic: ");

  if (diaglen < 0)
	diaglen = 0;

  if(fmt) {
	va_start(args, fmt);
	vsnprintf(diagbuf + diaglen, sizeof(diagbuf) - diaglen, fmt, args);
	va_end(args);

	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
  } else {
	snprintf(diagbuf + diaglen, sizeof(diagbuf) - diaglen, "no message");
	printf("no message\n");
  }
  printf("\n");

  diaglen = (int)strnlen(diagbuf, sizeof(diagbuf));
  if (diaglen < (int)sizeof(diagbuf) - 1) {
	diagbuf[diaglen++] = '\n';
	diagbuf[diaglen] = '\0';
  }
  if (diaglen > 0)
	sys_diagctl_diag(diagbuf, diaglen);

  printf("syslib:panic.c: stacktrace: ");
  util_stacktrace();

  panic_hook();

  /* Try exit */
  _exit(1);

  /* Try to signal ourself */
  abort();

  /* If exiting nicely through PM fails for some reason, try to
   * commit suicide. E.g., message to PM might fail due to deadlock.
   */
  suicide = (void (*)(void)) -1;
  suicide();

  /* If committing suicide fails for some reason, hang. */
  for(;;) { }
}
