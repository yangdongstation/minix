# Bugs Found

This list captures issues found during a repository-wide review. Each item includes severity and a file reference.

## Critical
- (none)

## High
- `minix/kernel/arch/riscv64/klib.S:80` FPU context save/restore is stubbed; processes using floating-point will corrupt each other's state. Implement fsd/fld save/restore into the proc structure and manage FS state.
- `minix/kernel/arch/riscv64/pg_utils.c:133` `minix/kernel/arch/riscv64/memory.c:309` `minix/kernel/arch/riscv64/pg_utils.c:155` Page-table walkers dereference physical addresses as virtual; this only works while all page-table pages stay inside the identity-mapped windows (currently first 1GB). With larger RAM or different mappings, walking can fault.
- `minix/kernel/arch/riscv64/arch_do_vmctl.c:64` `minix/kernel/arch/riscv64/arch_do_vmctl.c:78` `minix/servers/vm/pagetable.c:1701` VMCTL PTROOT/GET_PDBR uses 32-bit fields on riscv64; page-table roots above 4GB get truncated, switching to the wrong SATP.
- `minix/kernel/arch/riscv64/phys_copy.S:71` The fault handler for `phys_copy` is not registered, so copy faults will escape to the general trap handler instead of returning an error.
- `minix/kernel/arch/riscv64/bsp/virt/bsp_init.c:31` FDT parsing is stubbed and always forces 128MB RAM and 1 CPU; running with different QEMU `-m` or `-smp` will misconfigure memory bounds and CPU counts.

## Medium
- `minix/kernel/arch/riscv64/bsp/virt/bsp_serial.c:164` UART interrupt handler drops RX characters and never drains the TX queue; interrupt-driven serial I/O will lose data unless polling is used.
- `minix/servers/vfs/misc.c:685` When revoking a controlling TTY on session leader exit, the code closes character devices but skips the select/poll wakeup path, leaving waiters potentially stuck.
- `minix/kernel/arch/riscv64/pg_utils.c:361` Page-table destruction is unimplemented, so page-table pages are never freed.

## Low
- `minix/kernel/arch/riscv64/memory.c:245` `vm_init` is a stub; currently unused, but if it gets wired in then per-process page tables will never be created.
- `minix/kernel/arch/riscv64/bsp/virt/bsp_timer.c:61` Timer interrupt handler never calls the kernel clock handler; this BSP timer path is currently unused (arch_clock handles timers), so it will only matter if it gets wired in.

## Resolved
- `minix/drivers/storage/virtio_blk_mmio/virtio_blk_mmio.c:270` Fixed DMA mapping by translating iovec buffers with `sys_vumap` and setting descriptor direction flags.
