/*
 * arch - display machine architecture
 *
 * Enhanced version with RISC-V 64-bit support
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>

static void
usage(void)
{
	fprintf(stderr, "usage: arch [-k]\n");
	exit(1);
}

int
main(int argc, char *argv[])
{
	struct utsname name;
	int kflag = 0;
	int ch;

	while ((ch = getopt(argc, argv, "k")) != -1) {
		switch (ch) {
		case 'k':
			kflag = 1;
			break;
		default:
			usage();
		}
	}

	if (uname(&name) < 0) {
		perror("uname");
		return 1;
	}

	if (kflag) {
		/* Print kernel architecture */
		printf("%s\n", name.machine);
	} else {
		/* Print user-land architecture */
#if defined(__riscv) && __riscv_xlen == 64
		printf("riscv64\n");
#elif defined(__riscv) && __riscv_xlen == 32
		printf("riscv32\n");
#elif defined(__x86_64__)
		printf("x86_64\n");
#elif defined(__i386__)
		printf("i386\n");
#elif defined(__aarch64__)
		printf("aarch64\n");
#elif defined(__arm__)
		printf("arm\n");
#else
		printf("%s\n", name.machine);
#endif
	}

	return 0;
}
