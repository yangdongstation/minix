# Makefile for RISC-V 64-bit tests
#
# Usage:
#   make              - build all tests
#   make test         - run all tests
#   make test-kernel  - run kernel tests only
#   make test-user    - run userspace tests only
#

TESTS=	test_csr \
	test_memory \
	test_trap \
	test_sbi \
	test_ipc \
	test_vm \
	test_timer \
	test_atomic

CC=	riscv64-unknown-elf-gcc
CFLAGS=	-march=rv64gc -mabi=lp64d -O2 -Wall
LDFLAGS= -nostdlib -static

.PHONY: all clean test test-kernel test-user

all: $(TESTS)

test_csr: test_csr.c
	$(CC) $(CFLAGS) -o $@ $<

test_memory: test_memory.c
	$(CC) $(CFLAGS) -o $@ $<

test_trap: test_trap.c
	$(CC) $(CFLAGS) -o $@ $<

test_sbi: test_sbi.c
	$(CC) $(CFLAGS) -o $@ $<

test_ipc: test_ipc.c
	$(CC) $(CFLAGS) -o $@ $<

test_vm: test_vm.c
	$(CC) $(CFLAGS) -o $@ $<

test_timer: test_timer.c
	$(CC) $(CFLAGS) -o $@ $<

test_atomic: test_atomic.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TESTS) *.o

test: test-kernel test-user
	@echo "All tests completed"

test-kernel:
	./run_tests.sh kernel

test-user:
	./run_tests.sh user
