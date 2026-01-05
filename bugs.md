# Bugs Found

This list captures issues found during a repository-wide review. Each item includes severity and a file reference.

## Critical
- `minix/drivers/storage/virtio_blk_mmio/virtio_blk_mmio.c:270` Data buffer physical address is hard-coded to 0, so block I/O will DMA to physical address 0 instead of the user buffer; this can corrupt memory or crash the system. Map the user buffer and use the real physical address.

## High
- `minix/kernel/arch/riscv64/bsp/virt/bsp_timer.c:61` Timer interrupt handler never calls the kernel clock handler, so scheduling/timekeeping callbacks are skipped. Wire the timer interrupt into the kernel clock path.
- `minix/kernel/arch/riscv64/klib.S:80` FPU context save/restore is stubbed; processes using floating-point will corrupt each other's state. Implement fsd/fld save/restore into the proc structure and manage FS state.
- `minix/kernel/arch/riscv64/memory.c:245` `vm_init` is a stub, so per-process page tables are never created. This breaks address-space isolation and can lead to faults once user space is enabled.
- `minix/kernel/arch/riscv64/phys_copy.S:71` The fault handler for `phys_copy` is not registered, so copy faults will escape to the general trap handler instead of returning an error.
- `minix/kernel/arch/riscv64/bsp/virt/bsp_init.c:31` FDT parsing is stubbed and always forces 128MB RAM and 1 CPU; running with different QEMU `-m` or `-smp` will misconfigure memory bounds and CPU counts.

## Medium
- `minix/kernel/arch/riscv64/bsp/virt/bsp_serial.c:164` UART interrupt handler drops RX characters and never drains the TX queue; interrupt-driven serial I/O will lose data unless polling is used.
- `minix/servers/vfs/misc.c:685` When revoking a controlling TTY on session leader exit, the code closes character devices but skips the select/poll wakeup path, leaving waiters potentially stuck.
