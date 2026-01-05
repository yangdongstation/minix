# tools/compat Directory Architecture

## Overview
This document summarizes the top-level, git-tracked contents of `tools/compat`.

## Top-level entries
### Directories (5)
- `crypto/`
- `dev/`
- `fs/`
- `sys/`
- `ufs/`

### Files (45)
- `Makefile`
- `README`
- `arch.md`
- `cdbr.h`
- `cdbw.h`
- `compat_defs.h`
- `compat_getopt.h`
- `compat_pwd.h`
- `configure`
- `configure.ac`
- `db.h`
- `defs.mk.in`
- `dprintf.c`
- `fgetln.c`
- `flock.c`
- `fpurge.c`
- `fts.h`
- `getline.c`
- `getmode.c`
- `glob.h`
- `issetugid.c`
- `lchflags.c`
- `lchmod.c`
- `lchown.c`
- `md2.h`
- `md4.h`
- `md5.h`
- `mpool.h`
- `namespace.h`
- `nbtool_config.h.in`
- `ndbm.h`
- `nl_types.h`
- `pread.c`
- `putc_unlocked.c`
- `pwrite.c`
- `rmd160.h`
- `setenv.c`
- `setgroupent.c`
- `setpassent.c`
- `setprogname.c`
- `sha1.h`
- `sha2.h`
- `snprintf.c`
- `tzfile.h`
- `vis.h`

## Notes
- Entries listed are tracked in git; untracked and ignored files are omitted.
- Log files (`*.log` and `*.log.*`) and `_tmp` directories are intentionally excluded.
- VCS metadata directories (.git, .hg, .svn, .bzr) are omitted.
