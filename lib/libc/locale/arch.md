# lib/libc/locale Directory Architecture

## Overview
This document summarizes the top-level, git-tracked contents of `lib/libc/locale`.

## Top-level entries
### Directories (0)
- (none)

### Files (81)
- `Makefile.inc`
- `__mb_cur_max.c`
- `__wctoint.h`
- `_wcstod.h`
- `_wcstol.h`
- `_wcstoul.h`
- `_wctrans.c`
- `_wctrans_local.h`
- `_wctype.c`
- `_wctype_local.h`
- `aliasname_local.h`
- `arch.md`
- `btowc.3`
- `ctype_local.h`
- `dummy_lc_collate.c`
- `dummy_lc_template.h`
- `duplocale.c`
- `fix_grouping.c`
- `fix_grouping.h`
- `freelocale.c`
- `generic_lc_all.c`
- `generic_lc_template_decl.h`
- `global_locale.c`
- `iswalnum.3`
- `iswctype.3`
- `iswctype_mb.c`
- `localeconv.c`
- `localeio.h`
- `mblen.3`
- `mbrlen.3`
- `mbrtowc.3`
- `mbsinit.3`
- `mbsrtowcs.3`
- `mbstowcs.3`
- `mbtowc.3`
- `multibyte.h`
- `multibyte_amd1.c`
- `multibyte_c90.c`
- `nb_lc_messages_misc.h`
- `nb_lc_monetary_misc.h`
- `nb_lc_numeric_misc.h`
- `nb_lc_template.h`
- `nb_lc_template_decl.h`
- `nb_lc_time_misc.h`
- `newlocale.c`
- `nl_langinfo.3`
- `nl_langinfo.c`
- `rune.c`
- `runetable.c`
- `runetype_file.h`
- `runetype_local.h`
- `setlocale.3`
- `setlocale.c`
- `setlocale_local.h`
- `towctrans.3`
- `towlower.3`
- `wcrtomb.3`
- `wcscoll.3`
- `wcscoll.c`
- `wcsftime.3`
- `wcsftime.c`
- `wcsrtombs.3`
- `wcstod.3`
- `wcstod.c`
- `wcstof.c`
- `wcstoimax.c`
- `wcstol.3`
- `wcstol.c`
- `wcstold.c`
- `wcstoll.c`
- `wcstombs.3`
- `wcstoul.c`
- `wcstoull.c`
- `wcstoumax.c`
- `wcsxfrm.3`
- `wcsxfrm.c`
- `wctob.3`
- `wctomb.3`
- `wctrans.3`
- `wctype.3`
- `wcwidth.3`

## Notes
- Entries listed are tracked in git; untracked and ignored files are omitted.
- Log files (`*.log` and `*.log.*`) and `_tmp` directories are intentionally excluded.
- VCS metadata directories (.git, .hg, .svn, .bzr) are omitted.
