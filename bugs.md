# Bugs Found

This list captures issues found during a repository-wide review. Each item includes severity and a file reference.

## Critical
- `minix/kernel/arch/riscv64/exception.c:145` `minix/kernel/arch/riscv64/exception.c:161` `minix/kernel/arch/riscv64/exception.c:169` User-mode illegal instruction/misaligned/access faults call `panic()` instead of delivering SIGILL/SIGBUS/SIGSEGV, allowing user processes to crash the kernel.

## High
- `minix/kernel/arch/riscv64/memory.c:570` `minix/kernel/vm.h:10` `virtual_copy_f` calls `phys_copy` without enabling `catch_pagefaults`, so a copy fault will panic the kernel instead of returning `EFAULT`/`VMSUSPEND`.
- `minix/kernel/arch/riscv64/phys_copy.S:86` `minix/kernel/arch/riscv64/pg_utils.c:178` `phys_copy` dereferences physical addresses as virtual; this only works for identity-mapped ranges (currently the first 1GB). Copies involving RAM above 1GB can fault or corrupt.
- `minix/kernel/arch/riscv64/exception.c:138` `minix/servers/vm/arch/riscv64/pagetable.h:70` `minix/servers/vm/pagefaults.c:165` `minix/servers/vm/region.c:281` Instruction page faults are not treated as execute-protection faults, so non-executable regions can be faulted in as readable-only and then refault forever instead of raising SIGSEGV.
- `minix/kernel/arch/riscv64/head.S:308` `minix/kernel/arch/riscv64/klib.S:26` Return-to-user always sets `sscratch` to `_boot_stack_top`, which is a single global stack; on SMP, all harts will reuse the same kernel stack and corrupt each other.

## Medium
- `minix/kernel/system/do_safecopy.c:23` `minix/kernel/system/do_safecopy.c:187` `MEM_TOP` is 32-bit; direct grants with addresses above 4GB are rejected by the wrap check on 64-bit kernels.
- `minix/kernel/arch/riscv64/exception.c:156` Breakpoint handling always advances `sepc` by 2 bytes; for 32-bit `ebreak` this skips into the next instruction and can trigger spurious faults.
- `minix/kernel/arch/riscv64/exception.c:11` `m_pagefault` is a static global message; concurrent page faults on SMP can corrupt each other's message contents.
- `minix/kernel/arch/riscv64/protect.c:177` VM ELF segments are mapped with write permission during load and never tightened after copy, leaving code pages writable (W^X violation).

## Low
- `minix/kernel/arch/riscv64/memory.c:252` `vm_init` is a stub; currently unused, but if it gets wired in then per-process page tables will never be created.
- `minix/kernel/arch/riscv64/bsp/virt/bsp_timer.c:61` Timer interrupt handler never calls the kernel clock handler; this BSP timer path is currently unused (arch_clock handles timers), so it will only matter if it gets wired in.

## Notes
- `minix/kernel/system/do_safecopy.c:342` `minix/kernel/system/do_safecopy.c:392` rv64: `SYS_SAFECOPYTO` with `CPF_TRY` can return `-14 (EFAULT)` on first attempt when the target user page is not mapped yet (e.g. `kcall safecopy err=0xfffffffffffffff2 ... gid=0x20a00002`). This is a soft-fault path; VFS should see `GRANT_FAULTED`, call `vm_vfs_procctl_handlemem`, and retry. Treat as a bug only if the retry does not succeed or if no retry happens.

## Resolved
- `minix/kernel/arch/riscv64/klib.S:80` FPU context save/restore is stubbed; processes using floating-point will corrupt each other's state. Implement fsd/fld save/restore into the proc structure and manage FS state.
- `minix/kernel/arch/riscv64/pg_utils.c:156` `minix/kernel/arch/riscv64/memory.c:316` Page-table walkers dereference physical addresses as virtual; this only works while all page-table pages stay inside the identity-mapped windows (currently first 1GB). With larger RAM or different mappings, walking can fault.
- `minix/kernel/arch/riscv64/arch_do_vmctl.c:64` `minix/kernel/arch/riscv64/arch_do_vmctl.c:78` `minix/servers/vm/pagetable.c:2208` VMCTL PTROOT/GET_PDBR uses 32-bit fields on riscv64; page-table roots above 4GB get truncated, switching to the wrong SATP.
- `minix/kernel/arch/riscv64/bsp/virt/bsp_init.c:80` FDT parsing is stubbed and always forces 128MB RAM and 1 CPU; running with different QEMU `-m` or `-smp` will misconfigure memory bounds and CPU counts.
- `minix/drivers/storage/virtio_blk_mmio/virtio_blk_mmio.c:318` Fixed DMA mapping by translating iovec buffers with `sys_vumap` and setting descriptor direction flags.
- `minix/kernel/arch/riscv64/bsp/virt/bsp_serial.c:312` UART interrupt handler drops RX characters and never drains the TX queue; interrupt-driven serial I/O will lose data unless polling is used.
- `minix/servers/vfs/misc.c:685` When revoking a controlling TTY on session leader exit, the code closes character devices but skips the select/poll wakeup path, leaving waiters potentially stuck.
- `minix/kernel/arch/riscv64/pg_utils.c:434` Page-table destruction is unimplemented, so page-table pages are never freed.
