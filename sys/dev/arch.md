# sys/dev Directory Architecture

## Overview
This document summarizes the top-level, git-tracked contents of `sys/dev`.

## Top-level entries
### Directories (5)
- `i2c/`
- `ic/`
- `pci/`
- `stbi/`
- `videomode/`

### Files (7)
- `Makefile`
- `arch.md`
- `clock_subr.c`
- `clock_subr.h`
- `dev_verbose.c`
- `dev_verbose.h`
- `vndvar.h`

## Notes
- Entries listed are tracked in git; untracked and ignored files are omitted.
- Log files (`*.log` and `*.log.*`) and `_tmp` directories are intentionally excluded.
- VCS metadata directories (.git, .hg, .svn, .bzr) are omitted.
