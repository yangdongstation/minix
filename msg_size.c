#include <minix/ipc.h>
char msg_size[sizeof(message)];
char msg_align[__alignof__(message)];
