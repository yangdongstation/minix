# minix/servers/vm Directory Architecture

## Overview
This document summarizes the top-level, git-tracked contents of `minix/servers/vm`.

## Top-level entries
### Directories (1)
- `arch/`

### Files (45)
- `Makefile`
- `acl.c`
- `alloc.c`
- `arch.md`
- `break.c`
- `cache.c`
- `cache.h`
- `cavl_if.h`
- `cavl_impl.h`
- `exit.c`
- `fdref.c`
- `fdref.h`
- `fork.c`
- `glo.h`
- `gp.c`
- `main.c`
- `mem_anon.c`
- `mem_anon_contig.c`
- `mem_cache.c`
- `mem_directphys.c`
- `mem_file.c`
- `mem_shared.c`
- `memlist.h`
- `memtype.h`
- `mmap.c`
- `pagefaults.c`
- `pagetable.c`
- `pb.c`
- `phys_region.h`
- `proto.h`
- `pt.h`
- `region.c`
- `region.h`
- `regionavl.c`
- `regionavl.h`
- `regionavl_defs.h`
- `rs.c`
- `sanitycheck.h`
- `slaballoc.c`
- `unavl.h`
- `util.h`
- `utility.c`
- `vfs.c`
- `vm.h`
- `vmproc.h`

## Notes
- Entries listed are tracked in git; untracked and ignored files are omitted.
- Log files (`*.log` and `*.log.*`) and `_tmp` directories are intentionally excluded.
- VCS metadata directories (.git, .hg, .svn, .bzr) are omitted.
