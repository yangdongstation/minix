# minix/kernel/arch/riscv64 Directory Architecture

## Overview
This document summarizes the top-level, git-tracked contents of `minix/kernel/arch/riscv64`.

## Top-level entries
### Directories (2)
- `bsp/`
- `include/`

### Files (26)
- `Makefile.inc`
- `README.md`
- `arch.md`
- `arch_clock.c`
- `arch_do_vmctl.c`
- `arch_reset.c`
- `arch_system.c`
- `console.c`
- `direct_tty_utils.c`
- `exc.S`
- `exception.c`
- `head.S`
- `hw_intr.c`
- `kernel.c`
- `kernel.lds`
- `klib.S`
- `memory.c`
- `mpx.S`
- `pg_utils.c`
- `phys_copy.S`
- `phys_memset.S`
- `plic.c`
- `pre_init.c`
- `procoffsets.cf`
- `protect.c`
- `sbi.c`

## Notes
- Entries listed are tracked in git; untracked and ignored files are omitted.
- Log files (`*.log` and `*.log.*`) and `_tmp` directories are intentionally excluded.
- VCS metadata directories (.git, .hg, .svn, .bzr) are omitted.
