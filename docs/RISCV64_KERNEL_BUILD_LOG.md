# RISC-V MINIX Kernel Build Log

This log tracks each step taken to build the RISC-V MINIX kernel in this
repository. Steps are updated after each change.

## Environment

- Workspace: `/home/donz/minix`
- Target: `evbriscv64` (MACHINE_ARCH: `riscv64`)
- Toolchain: `riscv64-unknown-elf-gcc` (expected in PATH)

## Steps

1. Create build log (this file): done.
2. Build tools (`./build.sh -U -j2 -m evbriscv64 tools`): initial run timed out
   after 120s. Second run with `HAVE_LLVM=no MKLLVM=no` failed in
   `tools/compat` because `nbtool_config.h` was missing. Clean rerun fixed
   that but later failed in `tools/binutils` with `fatal error: bfd.h:
   No such file or directory`. A single-job rerun (`-j1`) hit the same
   `bfd.h` error in `tools/binutils/obj/build/bfd`.
   Manual attempt to generate `bfd.h` via `nbmake -C tools/binutils/obj/build/bfd bfd.h`
   failed with `nbmake: no system rules (sys.mk)` when using a wildcard
   tooldir. Retried with the specific tooldir and `-m /home/donz/minix/share/mk`,
   which successfully generated `bfd.h`. Final rerun with `-u -j1` completed
   successfully; tools installed to `/home/donz/minix/obj/tooldir.Linux-6.6.99-09058-g594deca50d73-x86_64`.
3. Build kernel (via `./build.sh -U -u -j1 -m evbriscv64 distribution`):
   failed while building `lib/csu` because
   `/home/donz/minix/obj/tooldir.../bin/riscv64-elf32-minix-clang` was not found.
   Attempted to rebuild tools with `MKGCC=yes` to provide a GCC toolchain,
   but the tools build aborted in `tools/gmake` due to missing distfiles
   (network fetch blocked).

## Update (2026-01-02)

- Workspace: `/root/minix`
- Build completed with current workarounds:
  ```
  MKPCI=no HOST_CFLAGS="-O -fcommon" HAVE_GOLD=no HAVE_LLVM=no MKLLVM=no \
  ./build.sh -j$(nproc) -m evbriscv64 \
    -V AVAILABLE_COMPILER=gcc -V ACTIVE_CC=gcc -V ACTIVE_CPP=gcc -V ACTIVE_CXX=gcc -V ACTIVE_OBJC=gcc \
    -V RISCV_ARCH_FLAGS='-march=RV64IMAFD -mcmodel=medany' \
    -V NOGCCERROR=yes \
    -V MKPIC=no -V MKPICLIB=no -V MKPICINSTALL=no \
    -V MKCXX=no -V MKLIBSTDCXX=no -V MKATF=no \
    -V USE_PCI=no \
    -V CHECKFLIST_FLAGS='-m -e' \
    distribution
  ```
- Outputs: kernel at `minix/kernel/obj/kernel`, modules in `obj/destdir.evbriscv64`
- Note: `CHECKFLIST_FLAGS='-m -e'` allows missing/extra files while sets are incomplete
