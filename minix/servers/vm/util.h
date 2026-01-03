
#ifndef _UTIL_H
#define _UTIL_H 1

#include "vm.h"
#include "glo.h"

#define ELEMENTS(a) (int)(sizeof(a)/sizeof((a)[0]))

void vm_stacktrace_at(const char *file, int line);
#define VM_STACKTRACE() vm_stacktrace_at(__FILE__, __LINE__)

#endif
