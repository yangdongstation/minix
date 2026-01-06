# RISC-V 64 Test Matrix

This document describes a stricter test strategy for the RISC-V port, with
clear tiers and example commands.

## Tiers

### Tier 0: Quick checks (local)
- Goal: fast feedback on build wiring and compile errors.
- Commands:
  - `./minix/tests/riscv64/run_tests.sh build`
  - `./minix/tests/riscv64/run_tests.sh user`

### Tier 1: QEMU boot + I/O smoke
- Goal: prove the kernel boots and the virtio-blk-mmio path can do basic I/O.
- Command:
  - `./minix/tests/riscv64/run_tests.sh kernel`
- Notes:
  - The smoke test builds `test_virtio_blk_mmio` into
    `obj/destdir.evbriscv64/bin/test_virtio_blk_mmio` when a cross-compiler is
    available. If the binary is not present in the guest, the test falls back
    to a `dd`/`cmp` I/O check.
  - The test runs in QEMU and targets `/dev/c0d0` by default. Provide a
    device path if needed.
  - If the test binary and `dd`/`cmp` are missing in the guest, the smoke test
    skips.

### Tier 2: Functional guest tests
- Goal: validate userland behavior and device interaction under real loads.
- Example in guest (manual):
  - `/bin/test_virtio_blk_mmio -p /dev/c0d0 -o 1048576 -s 4096 -b`
- Use a dedicated disk image where possible to avoid corrupting a root disk.

### Tier 3: Negative and stress tests
- Goal: exercise error paths, alignment checks, and concurrency.
- Examples:
  - Unaligned sizes or offsets (expect EINVAL from block devices).
  - Multiple concurrent readv/writev loops.
  - Repeated driver restarts under I/O load.

## Smoke test configuration

The QEMU smoke test is driven by `minix/tests/riscv64/qemu_io_smoke.py` and is
invoked from `run_tests.sh kernel`.

Environment variables:
- `SMOKE_DISK_IMAGE`: optional host path to a disk image for QEMU.
- `SMOKE_DEVICE`: guest device path (default: `/dev/c0d0`).
- `SMOKE_OFFSET`: guest offset in bytes (default: `1048576`).
- `SMOKE_SIZE`: total I/O size in bytes (default: `4096`).
- `SMOKE_FILE_FALLBACK`: if set, use a guest temp file instead of a block device.
  This only validates readv/writev plumbing, not the virtio block driver.
- `SMOKE_DD_UNSAFE`: if set, allow the dd/cmp fallback to write to block devices.
  This is unsafe for root disks; use only with a disposable image. Auto-generated
  temp images are treated as safe.

Direct usage:
```
python3 minix/tests/riscv64/qemu_io_smoke.py \
  --qemu-script minix/scripts/qemu-riscv64.sh \
  --kernel minix/kernel/obj/kernel \
  --destdir obj/destdir.evbriscv64 \
  --test-bin obj/destdir.evbriscv64/bin/test_virtio_blk_mmio \
  --device /dev/c0d0 \
  --offset 1048576 \
  --size 4096 \
  --require-block
```

## Building the test binary (manual)

If you want to build and install the test binary manually:
```
CC=/path/to/riscv64-elf32-minix-gcc
DESTDIR=obj/destdir.evbriscv64
$CC -march=RV64IMAFD -mcmodel=medany --sysroot=$DESTDIR \
  -I$DESTDIR/usr/include -O2 -Wall -std=gnu99 \
  -Wl,--defsym,__global_pointer$=_gp \
  -o $DESTDIR/bin/test_virtio_blk_mmio \
  minix/tests/riscv64/test_virtio_blk_mmio.c
```
