# Bugs Found

This list captures issues found during a repository-wide review. Each item includes severity and a file reference.

## Critical
- (none)

## High
- (none)

## Medium
- (none)

## Low
- `minix/kernel/arch/riscv64/memory.c:252` `vm_init` is a stub; currently unused, but if it gets wired in then per-process page tables will never be created.

## Notes
- `minix/kernel/system/do_safecopy.c:342` `minix/kernel/system/do_safecopy.c:392` rv64: `SYS_SAFECOPYTO` with `CPF_TRY` can return `-14 (EFAULT)` on first attempt when the target user page is not mapped yet (e.g. `kcall safecopy err=0xfffffffffffffff2 ... gid=0x20a00002`). This is a soft-fault path; VFS should see `GRANT_FAULTED`, call `vm_vfs_procctl_handlemem`, and retry. Treat as a bug only if the retry does not succeed or if no retry happens.

## Validation
- `./minix/tests/riscv64/run_tests.sh kernel`: PASS Kernel boot, PASS Timer interrupt, PASS VirtIO block I/O smoke, SKIP SMP initialization (not yet implemented) (re-run)

## Resolved
- `minix/kernel/system/do_safecopy.c:23` `minix/kernel/system/do_safecopy.c:187` `MEM_TOP` now uses `vir_bytes` width, so direct grants above 4GB are no longer rejected by a 32-bit wrap check.
- `minix/kernel/arch/riscv64/exception.c:147` `minix/kernel/arch/riscv64/exception.c:171` `minix/kernel/arch/riscv64/exception.c:183` User-mode illegal instruction/misaligned/access faults now deliver SIGILL/SIGBUS/SIGSEGV instead of panicking the kernel.
- `minix/kernel/arch/riscv64/exception.c:162` Breakpoints now signal SIGEMT in user mode; kernel breakpoints advance `sepc` by 4 bytes.
- `minix/kernel/arch/riscv64/exception.c:239` Pagefault IPC message is now stack-local to avoid SMP races.
- `minix/kernel/arch/riscv64/memory.c:570` `virtual_copy_f` now wraps `phys_copy` with `PHYS_COPY_CATCH`, so copy faults return `EFAULT`/`VMSUSPEND` instead of panicking.
- `minix/kernel/arch/riscv64/phys_copy.S:71` Physical addresses are now translated to the kernel direct map before dereference, so copies above the identity-mapped range no longer fault.
- `minix/servers/vm/arch/riscv64/pagetable.h:74` `minix/servers/vm/pagefaults.c:238` Instruction page faults on non-executable regions now raise SIGSEGV instead of looping forever.
- `minix/kernel/arch/riscv64/head.S:308` `minix/kernel/arch/riscv64/klib.S:26` Return-to-user now sets `sscratch` to a per-hart kernel stack to avoid SMP stack corruption.
- `minix/kernel/arch/riscv64/pg_utils.c:261` `minix/kernel/arch/riscv64/protect.c:235` VM ELF segments now have final permissions applied after loading to restore W^X.
- `minix/kernel/arch/riscv64/bsp/virt/bsp_timer.c:61` BSP timer interrupt now calls the kernel clock handler when used.
