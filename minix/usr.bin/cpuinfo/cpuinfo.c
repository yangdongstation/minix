/*
 * cpuinfo - display CPU information for RISC-V 64
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#if defined(__riscv)
#include <machine/cpu.h>
#endif

static void
print_riscv_info(void)
{
	printf("Architecture:          riscv64\n");
	printf("Byte Order:            Little Endian\n");

#if defined(__riscv)
	/* ISA extensions */
	printf("ISA:                   rv64");
#ifdef __riscv_mul
	printf("i");
#endif
#ifdef __riscv_muldiv
	printf("m");
#endif
#ifdef __riscv_atomic
	printf("a");
#endif
#ifdef __riscv_flen
#if __riscv_flen >= 32
	printf("f");
#endif
#if __riscv_flen >= 64
	printf("d");
#endif
#endif
#ifdef __riscv_compressed
	printf("c");
#endif
	printf("\n");

	/* Address size */
	printf("Address sizes:         39 bits virtual (Sv39)\n");
#endif

	/* Try to get CPU count from sysctl */
	int ncpu = 1;
#ifdef HW_NCPU
	size_t len = sizeof(ncpu);
	int mib[2] = { CTL_HW, HW_NCPU };
	sysctl(mib, 2, &ncpu, &len, NULL, 0);
#endif
	printf("CPU(s):                %d\n", ncpu);
}

static void
print_generic_info(void)
{
	int ncpu = 1;

#ifdef HW_NCPU
	size_t len = sizeof(ncpu);
	int mib[2] = { CTL_HW, HW_NCPU };
	sysctl(mib, 2, &ncpu, &len, NULL, 0);
#endif

#if defined(__x86_64__)
	printf("Architecture:          x86_64\n");
#elif defined(__i386__)
	printf("Architecture:          i386\n");
#elif defined(__aarch64__)
	printf("Architecture:          aarch64\n");
#elif defined(__arm__)
	printf("Architecture:          arm\n");
#else
	printf("Architecture:          unknown\n");
#endif
	printf("CPU(s):                %d\n", ncpu);
}

int
main(int argc, char *argv[])
{
#if defined(__riscv)
	print_riscv_info();
#else
	print_generic_info();
#endif
	return 0;
}
