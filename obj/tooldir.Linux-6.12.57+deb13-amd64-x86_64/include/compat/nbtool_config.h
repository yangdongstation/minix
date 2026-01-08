/* nbtool_config.h.  Generated automatically by configure.  */
/* nbtool_config.h.in.  Generated automatically from configure.ac by autoheader.  */

/*      $NetBSD: nbtool_config.h.in,v 1.37 2015/07/26 14:01:53 kamil Exp $    */

#ifndef __NETBSD_NBTOOL_CONFIG_H__
#define __NETBSD_NBTOOL_CONFIG_H__

/* Define to 1 if your `fparseln' function is broken. */
/* #undef BROKEN_FPARSELN */

/* Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
   systems. This function is required for `alloca.c' support on those systems.
   */
/* #undef CRAY_STACKSEG_END */

/* Define if using `alloca.c'. */
/* #undef C_ALLOCA */

/* Define if you have `alloca', as a function or macro. */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix). */
#define HAVE_ALLOCA_H 1

/* Define if you have the <arpa/nameser.h> header file. */
#define HAVE_ARPA_NAMESER_H 1

/* Define if you have the `asnprintf' function. */
/* #undef HAVE_ASNPRINTF */

/* Define if you have the `asprintf' function. */
#define HAVE_ASPRINTF 1

/* Define if you have the <assert.h> header file. */
#define HAVE_ASSERT_H 1

/* Define if you have the `atoll' function. */
#define HAVE_ATOLL 1

/* Define if you have the `basename' function. */
#define HAVE_BASENAME 1

/* Define if you have the <ctype.h> header file. */
#define HAVE_CTYPE_H 1

/* Define to 1 if you have the declaration of `be16dec', and to 0 if you
   don't. */
#define HAVE_DECL_BE16DEC 0

/* Define to 1 if you have the declaration of `be16enc', and to 0 if you
   don't. */
#define HAVE_DECL_BE16ENC 0

/* Define to 1 if you have the declaration of `be16toh', and to 0 if you
   don't. */
#define HAVE_DECL_BE16TOH 1

/* Define to 1 if you have the declaration of `be32dec', and to 0 if you
   don't. */
#define HAVE_DECL_BE32DEC 0

/* Define to 1 if you have the declaration of `be32enc', and to 0 if you
   don't. */
#define HAVE_DECL_BE32ENC 0

/* Define to 1 if you have the declaration of `be32toh', and to 0 if you
   don't. */
#define HAVE_DECL_BE32TOH 1

/* Define to 1 if you have the declaration of `be64dec', and to 0 if you
   don't. */
#define HAVE_DECL_BE64DEC 0

/* Define to 1 if you have the declaration of `be64enc', and to 0 if you
   don't. */
#define HAVE_DECL_BE64ENC 0

/* Define to 1 if you have the declaration of `be64toh', and to 0 if you
   don't. */
#define HAVE_DECL_BE64TOH 1

/* Define to 1 if you have the declaration of `bswap16', and to 0 if you
   don't. */
#define HAVE_DECL_BSWAP16 0

/* Define to 1 if you have the declaration of `bswap32', and to 0 if you
   don't. */
#define HAVE_DECL_BSWAP32 0

/* Define to 1 if you have the declaration of `bswap64', and to 0 if you
   don't. */
#define HAVE_DECL_BSWAP64 0

/* Define to 1 if you have the declaration of `errc', and to 0 if you don't.
   */
#define HAVE_DECL_ERRC 0

/* Define to 1 if you have the declaration of `fstatvfs', and to 0 if you
   don't. */
#define HAVE_DECL_FSTATVFS 1

/* Define to 1 if you have the declaration of `gid_from_group', and to 0 if
   you don't. */
#define HAVE_DECL_GID_FROM_GROUP 0

/* Define to 1 if you have the declaration of `group_from_gid', and to 0 if
   you don't. */
#define HAVE_DECL_GROUP_FROM_GID 0

/* Define to 1 if you have the declaration of `htobe16', and to 0 if you
   don't. */
#define HAVE_DECL_HTOBE16 1

/* Define to 1 if you have the declaration of `htobe32', and to 0 if you
   don't. */
#define HAVE_DECL_HTOBE32 1

/* Define to 1 if you have the declaration of `htobe64', and to 0 if you
   don't. */
#define HAVE_DECL_HTOBE64 1

/* Define to 1 if you have the declaration of `htole16', and to 0 if you
   don't. */
#define HAVE_DECL_HTOLE16 1

/* Define to 1 if you have the declaration of `htole32', and to 0 if you
   don't. */
#define HAVE_DECL_HTOLE32 1

/* Define to 1 if you have the declaration of `htole64', and to 0 if you
   don't. */
#define HAVE_DECL_HTOLE64 1

/* Define to 1 if you have the declaration of `lchflags', and to 0 if you
   don't. */
#define HAVE_DECL_LCHFLAGS 0

/* Define to 1 if you have the declaration of `lchmod', and to 0 if you don't.
   */
#define HAVE_DECL_LCHMOD 1

/* Define to 1 if you have the declaration of `lchown', and to 0 if you don't.
   */
#define HAVE_DECL_LCHOWN 1

/* Define to 1 if you have the declaration of `le16dec', and to 0 if you
   don't. */
#define HAVE_DECL_LE16DEC 0

/* Define to 1 if you have the declaration of `le16enc', and to 0 if you
   don't. */
#define HAVE_DECL_LE16ENC 0

/* Define to 1 if you have the declaration of `le16toh', and to 0 if you
   don't. */
#define HAVE_DECL_LE16TOH 1

/* Define to 1 if you have the declaration of `le32dec', and to 0 if you
   don't. */
#define HAVE_DECL_LE32DEC 0

/* Define to 1 if you have the declaration of `le32enc', and to 0 if you
   don't. */
#define HAVE_DECL_LE32ENC 0

/* Define to 1 if you have the declaration of `le32toh', and to 0 if you
   don't. */
#define HAVE_DECL_LE32TOH 1

/* Define to 1 if you have the declaration of `le64dec', and to 0 if you
   don't. */
#define HAVE_DECL_LE64DEC 0

/* Define to 1 if you have the declaration of `le64enc', and to 0 if you
   don't. */
#define HAVE_DECL_LE64ENC 0

/* Define to 1 if you have the declaration of `le64toh', and to 0 if you
   don't. */
#define HAVE_DECL_LE64TOH 1

/* Define to 1 if you have the declaration of `optind', and to 0 if you don't.
   */
#define HAVE_DECL_OPTIND 1

/* Define to 1 if you have the declaration of `optreset', and to 0 if you
   don't. */
#define HAVE_DECL_OPTRESET 0

/* Define to 1 if you have the declaration of `pwcache_groupdb', and to 0 if
   you don't. */
#define HAVE_DECL_PWCACHE_GROUPDB 0

/* Define to 1 if you have the declaration of `pwcache_userdb', and to 0 if
   you don't. */
#define HAVE_DECL_PWCACHE_USERDB 0

/* Define to 1 if you have the declaration of `setgroupent', and to 0 if you
   don't. */
#define HAVE_DECL_SETGROUPENT 0

/* Define to 1 if you have the declaration of `setpassent', and to 0 if you
   don't. */
#define HAVE_DECL_SETPASSENT 0

/* Define to 1 if you have the declaration of `strndup', and to 0 if you
   don't. */
#define HAVE_DECL_STRNDUP 1

/* Define to 1 if you have the declaration of `strnlen', and to 0 if you
   don't. */
#define HAVE_DECL_STRNLEN 1

/* Define to 1 if you have the declaration of `strsuftoll', and to 0 if you
   don't. */
#define HAVE_DECL_STRSUFTOLL 0

/* Define to 1 if you have the declaration of `sys_signame', and to 0 if you
   don't. */
#define HAVE_DECL_SYS_SIGNAME 0

/* Define to 1 if you have the declaration of `uid_from_user', and to 0 if you
   don't. */
#define HAVE_DECL_UID_FROM_USER 0

/* Define to 1 if you have the declaration of `user_from_uid', and to 0 if you
   don't. */
#define HAVE_DECL_USER_FROM_UID 0

/* Define to 1 if you have the declaration of `verrc', and to 0 if you don't.
   */
#define HAVE_DECL_VERRC 0

/* Define to 1 if you have the declaration of `vwarnc', and to 0 if you don't.
   */
#define HAVE_DECL_VWARNC 0

/* Define to 1 if you have the declaration of `warnc', and to 0 if you don't.
   */
#define HAVE_DECL_WARNC 0

/* Define if you have the `devname' function. */
/* #undef HAVE_DEVNAME */

/* Define if you have the <dirent.h> header file, and it defines `DIR'. */
#define HAVE_DIRENT_H 1

/* Define if you have the `dirfd' function. */
#define HAVE_DIRFD 1

/* Define if you have the `dirname' function. */
#define HAVE_DIRNAME 1

/* Define if `dd_fd' is member of `DIR'. */
/* #undef HAVE_DIR_DD_FD */

/* Define if `__dd_fd' is member of `DIR'. */
/* #undef HAVE_DIR___DD_FD */

/* Define if you have the `dprintf' function. */
#define HAVE_DPRINTF 1

/* Define if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define if you have the <err.h> header file. */
#define HAVE_ERR_H 1

/* Define if you have the `esetfunc' function. */
/* #undef HAVE_ESETFUNC */

/* Define if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define if you have the <features.h> header file. */
#define HAVE_FEATURES_H 1

/* Define if you have the `fgetln' function. */
/* #undef HAVE_FGETLN */

/* Define if you have the `flock' function. */
#define HAVE_FLOCK 1

/* Define if you have the `fparseln' function. */
/* #undef HAVE_FPARSELN */

/* Define if you have the `fpurge' function. */
/* #undef HAVE_FPURGE */

/* Define if you have the `futimes' function. */
#define HAVE_FUTIMES 1

/* Define if you have the `getline' function. */
#define HAVE_GETLINE 1

/* Define if you have the `getopt' function. */
#define HAVE_GETOPT 1

/* Define if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Define if you have the `getopt_long' function. */
#define HAVE_GETOPT_LONG 1

/* Define if you have the `gid_from_group' function. */
/* #undef HAVE_GID_FROM_GROUP */

/* Define if you have the `group_from_gid' function. */
/* #undef HAVE_GROUP_FROM_GID */

/* Define if you have the <grp.h> header file. */
#define HAVE_GRP_H 1

/* Define if you have the `heapsort' function. */
/* #undef HAVE_HEAPSORT */

/* Define if the system has the type `id_t'. */
#define HAVE_ID_T 1

/* Define if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if you have the `isblank' function. */
#define HAVE_ISBLANK 1

/* Define if you have the `issetugid' function. */
/* #undef HAVE_ISSETUGID */

/* Define if you have the `lchflags' function. */
/* #undef HAVE_LCHFLAGS */

/* Define if you have the `lchmod' function. */
#define HAVE_LCHMOD 1

/* Define if you have the `lchown' function. */
#define HAVE_LCHOWN 1

/* Define if you have the <libgen.h> header file. */
#define HAVE_LIBGEN_H 1

/* Define if you have the `regex' library (-lregex). */
/* #undef HAVE_LIBREGEX */

/* Define if you have the `rt' library (-lrt). */
#define HAVE_LIBRT 1

/* Define if you have the `z' library (-lz). */
#define HAVE_LIBZ 1

/* Define if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define if the system has the type `long long'. */
#define HAVE_LONG_LONG 1

/* Define if you have the `lutimes' function. */
#define HAVE_LUTIMES 1

/* Define if you have the <machine/bswap.h> header file. */
/* #undef HAVE_MACHINE_BSWAP_H */

/* Define if you have the <machine/endian.h> header file. */
/* #undef HAVE_MACHINE_ENDIAN_H */

/* Define if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you have the `mkdtemp' function. */
#define HAVE_MKDTEMP 1

/* Define if you have the `mkstemp' function. */
#define HAVE_MKSTEMP 1

/* Define if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define if you have the <netconfig.h> header file. */
/* #undef HAVE_NETCONFIG_H */

/* Define if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define if you have the <paths.h> header file. */
#define HAVE_PATHS_H 1

/* Define if you have the `poll' function. */
#define HAVE_POLL 1

/* Define if you have the `pread' function. */
#define HAVE_PREAD 1

/* Define if you have the <pthread.h> header file. */
#define HAVE_PTHREAD_H 1

/* Define if you have the `putc_unlocked' function. */
#define HAVE_PUTC_UNLOCKED 1

/* Define if you have the `pwcache_groupdb' function. */
/* #undef HAVE_PWCACHE_GROUPDB */

/* Define if you have the `pwcache_userdb' function. */
/* #undef HAVE_PWCACHE_USERDB */

/* Define if you have the <pwd.h> header file. */
#define HAVE_PWD_H 1

/* Define if you have the `pwrite' function. */
#define HAVE_PWRITE 1

/* Define if you have the `raise_default_signal' function. */
/* #undef HAVE_RAISE_DEFAULT_SIGNAL */

/* Define if you have the `random' function. */
#define HAVE_RANDOM 1

/* Define if you have the `reallocarr' function. */
/* #undef HAVE_REALLOCARR */

/* Define if you have the <resolv.h> header file. */
#define HAVE_RESOLV_H 1

/* Define if you have the <rpc/types.h> header file. */
/* #undef HAVE_RPC_TYPES_H */

/* Define if you have the `setenv' function. */
#define HAVE_SETENV 1

/* Define if you have the `setgroupent' function. */
/* #undef HAVE_SETGROUPENT */

/* Define if you have the `setpassent' function. */
/* #undef HAVE_SETPASSENT */

/* Define if you have the `setprogname' function. */
/* #undef HAVE_SETPROGNAME */

/* Define if you have the <signal.h> header file. */
#define HAVE_SIGNAL_H 1

/* Define if you have the `snprintb_m' function. */
/* #undef HAVE_SNPRINTB_M */

/* Define if you have the `snprintf' function. */
#define HAVE_SNPRINTF 1

/* Define if you have the socklen_t type. */
#define HAVE_SOCKLEN_T 1

/* Define if you have the <stdarg.h> header file. */
#define HAVE_STDARG_H 1

/* Define if you have the <stddef.h> header file. */
#define HAVE_STDDEF_H 1

/* Define if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define if you have the `strlcat' function. */
#define HAVE_STRLCAT 1

/* Define if you have the `strlcpy' function. */
#define HAVE_STRLCPY 1

/* Define if you have the `strmode' function. */
/* #undef HAVE_STRMODE */

/* Define if you have the `strndup' function. */
#define HAVE_STRNDUP 1

/* Define if you have the `strnlen' function. */
#define HAVE_STRNLEN 1

/* Define if you have the `strsep' function. */
#define HAVE_STRSEP 1

/* Define if you have the `strsuftoll' function. */
/* #undef HAVE_STRSUFTOLL */

/* Define if you have the `strtoi' function. */
/* #undef HAVE_STRTOI */

/* Define if you have the `strtoll' function. */
#define HAVE_STRTOLL 1

/* Define if you have the `strtou' function. */
/* #undef HAVE_STRTOU */

/* Define if `d_namlen' is member of `struct dirent'. */
/* #undef HAVE_STRUCT_DIRENT_D_NAMLEN */

/* Define if `f_iosize' is member of `struct statvfs'. */
/* #undef HAVE_STRUCT_STATVFS_F_IOSIZE */

/* Define if `st_atim' is member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_ATIM */

/* Define if `st_birthtime' is member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_BIRTHTIME */

/* Define if `st_birthtimensec' is member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_BIRTHTIMENSEC */

/* Define if `st_flags' is member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_FLAGS */

/* Define if `st_gen' is member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_GEN */

/* Define if `st_mtimensec' is member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_MTIMENSEC */

/* Define if `tm_gmtoff' is member of `struct tm'. */
#define HAVE_STRUCT_TM_TM_GMTOFF 1

/* Define if you have the <sys/bswap.h> header file. */
/* #undef HAVE_SYS_BSWAP_H */

/* Define if you have the <sys/cdefs.h> header file. */
#define HAVE_SYS_CDEFS_H 1

/* Define if you have the <sys/dir.h> header file, and it defines `DIR'. */
/* #undef HAVE_SYS_DIR_H */

/* Define if you have the <sys/endian.h> header file. */
/* #undef HAVE_SYS_ENDIAN_H */

/* Define if you have the <sys/featuretest.h> header file. */
/* #undef HAVE_SYS_FEATURETEST_H */

/* Define if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1

/* Define if you have the <sys/mtio.h> header file. */
#define HAVE_SYS_MTIO_H 1

/* Define if you have the <sys/ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_SYS_NDIR_H */

/* Define if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define if you have the <sys/poll.h> header file. */
#define HAVE_SYS_POLL_H 1

/* Define if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/syslimits.h> header file. */
/* #undef HAVE_SYS_SYSLIMITS_H */

/* Define if you have the <sys/sysmacros.h> header file. */
#define HAVE_SYS_SYSMACROS_H 1

/* Define if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the <sys/utsname.h> header file. */
#define HAVE_SYS_UTSNAME_H 1

/* Define if you have the <sys/wait.h> header file. */
#define HAVE_SYS_WAIT_H 1

/* Define if you have the <termios.h> header file. */
#define HAVE_TERMIOS_H 1

/* Define if you have the `uid_from_user' function. */
/* #undef HAVE_UID_FROM_USER */

/* Define if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define if you have the `user_from_uid' function. */
/* #undef HAVE_USER_FROM_UID */

/* Define if you have the <util.h> header file. */
/* #undef HAVE_UTIL_H */

/* Define if the system has the type `u_char'. */
#define HAVE_U_CHAR 1

/* Define if the system has the type `u_int'. */
#define HAVE_U_INT 1

/* Define if the system has the type `u_long'. */
#define HAVE_U_LONG 1

/* Define if the system has the type `u_quad_t'. */
#define HAVE_U_QUAD_T 1

/* Define if the system has the type `u_short'. */
#define HAVE_U_SHORT 1

/* Define if you have the `vasnprintf' function. */
/* #undef HAVE_VASNPRINTF */

/* Define if you have the `vasprintf' function. */
#define HAVE_VASPRINTF 1

/* Define if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Define if you have the `__fpurge' function. */
#define HAVE___FPURGE 1

/* Path to sh(1). */
#define PATH_BSHELL "/usr/bin/sh"

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
        STACK_DIRECTION > 0 => grows toward higher addresses
        STACK_DIRECTION < 0 => grows toward lower addresses
        STACK_DIRECTION = 0 => direction of growth unknown */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files. */
/* #undef STDC_HEADERS */

/* Define if your processor stores words with the most significant byte first
   (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

/* Define for NetBSD headers. */
/* #undef _POSIX_C_SOURCE */

/* Define for NetBSD headers. */
/* #undef _POSIX_SOURCE */

/* Define for NetBSD headers. */
/* #undef _XOPEN_SOURCE */

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define if you have uint16_t, but not u_int16_t. */
/* #undef u_int16_t */

/* Define if you have uint32_t, but not u_int32_t. */
/* #undef u_int32_t */

/* Define if you have uint64_t, but not u_int64_t. */
/* #undef u_int64_t */

/* Define if you have uint8_t, but not u_int8_t. */
/* #undef u_int8_t */

/* Define if you have u_int16_t, but not uint16_t. */
/* #undef uint16_t */

/* Define if you have u_int32_t, but not uint32_t. */
/* #undef uint32_t */

/* Define if you have u_int64_t, but not uint64_t. */
/* #undef uint64_t */

/* Define if you have u_int8_t, but not uint8_t. */
/* #undef uint8_t */

#include "compat_defs.h"
#endif /* !__NETBSD_NBTOOL_CONFIG_H__ */
