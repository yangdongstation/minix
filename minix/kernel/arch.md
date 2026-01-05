# minix/kernel Directory Architecture

## Overview
This document summarizes the top-level, git-tracked contents of `minix/kernel`.

## Top-level entries
### Directories (2)
- `arch/`
- `system/`

### Files (39)
- `Makefile`
- `arch.md`
- `clock.c`
- `clock.h`
- `config.h`
- `const.h`
- `cpulocals.c`
- `cpulocals.h`
- `debug.c`
- `debug.h`
- `extract-errno.sh`
- `extract-mfield.sh`
- `extract-mtype.sh`
- `glo.h`
- `interrupt.c`
- `interrupt.h`
- `ipc.h`
- `ipc_filter.h`
- `kernel.h`
- `main.c`
- `priv.h`
- `proc.c`
- `proc.h`
- `procoffsets.h.tmp`
- `profile.c`
- `profile.h`
- `proto.h`
- `smp.c`
- `smp.h`
- `spinlock.h`
- `system.c`
- `system.h`
- `table.c`
- `type.h`
- `usermapped_data.c`
- `utility.c`
- `vm.h`
- `watchdog.c`
- `watchdog.h`

## Notes
- Entries listed are tracked in git; untracked and ignored files are omitted.
- Log files (`*.log` and `*.log.*`) and `_tmp` directories are intentionally excluded.
- VCS metadata directories (.git, .hg, .svn, .bzr) are omitted.
