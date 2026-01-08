/*	$NetBSD$	*/

#ifndef _RISCV_INT_FMTIO_H_
#define _RISCV_INT_FMTIO_H_

/*
 * 7.8.1 Macros for format specifiers
 */

#ifdef __riscv64
#define __PRI64_PREFIX "l"
#define __SCN64_PREFIX "l"
#else
#define __PRI64_PREFIX "ll"
#define __SCN64_PREFIX "ll"
#endif

/* fprintf macros for signed integers */
#define PRId8		"d"
#define PRId16		"d"
#define PRId32		"d"
#define PRId64		__PRI64_PREFIX "d"

#define PRIdLEAST8	"d"
#define PRIdLEAST16	"d"
#define PRIdLEAST32	"d"
#define PRIdLEAST64	__PRI64_PREFIX "d"

#define PRIdFAST8	"d"
#define PRIdFAST16	"d"
#define PRIdFAST32	"d"
#define PRIdFAST64	__PRI64_PREFIX "d"

#define PRIdMAX		__PRI64_PREFIX "d"
#ifdef __riscv64
#define PRIdPTR		"ld"
#else
#define PRIdPTR		"d"
#endif

#define PRIi8		"i"
#define PRIi16		"i"
#define PRIi32		"i"
#define PRIi64		__PRI64_PREFIX "i"

#define PRIiLEAST8	"i"
#define PRIiLEAST16	"i"
#define PRIiLEAST32	"i"
#define PRIiLEAST64	__PRI64_PREFIX "i"

#define PRIiFAST8	"i"
#define PRIiFAST16	"i"
#define PRIiFAST32	"i"
#define PRIiFAST64	__PRI64_PREFIX "i"

#define PRIiMAX		__PRI64_PREFIX "i"
#ifdef __riscv64
#define PRIiPTR		"li"
#else
#define PRIiPTR		"i"
#endif

/* fprintf macros for unsigned integers */
#define PRIo8		"o"
#define PRIo16		"o"
#define PRIo32		"o"
#define PRIo64		__PRI64_PREFIX "o"

#define PRIoLEAST8	"o"
#define PRIoLEAST16	"o"
#define PRIoLEAST32	"o"
#define PRIoLEAST64	__PRI64_PREFIX "o"

#define PRIoFAST8	"o"
#define PRIoFAST16	"o"
#define PRIoFAST32	"o"
#define PRIoFAST64	__PRI64_PREFIX "o"

#define PRIoMAX		__PRI64_PREFIX "o"
#ifdef __riscv64
#define PRIoPTR		"lo"
#else
#define PRIoPTR		"o"
#endif

#define PRIu8		"u"
#define PRIu16		"u"
#define PRIu32		"u"
#define PRIu64		__PRI64_PREFIX "u"

#define PRIuLEAST8	"u"
#define PRIuLEAST16	"u"
#define PRIuLEAST32	"u"
#define PRIuLEAST64	__PRI64_PREFIX "u"

#define PRIuFAST8	"u"
#define PRIuFAST16	"u"
#define PRIuFAST32	"u"
#define PRIuFAST64	__PRI64_PREFIX "u"

#define PRIuMAX		__PRI64_PREFIX "u"
#ifdef __riscv64
#define PRIuPTR		"lu"
#else
#define PRIuPTR		"u"
#endif

#define PRIx8		"x"
#define PRIx16		"x"
#define PRIx32		"x"
#define PRIx64		__PRI64_PREFIX "x"

#define PRIxLEAST8	"x"
#define PRIxLEAST16	"x"
#define PRIxLEAST32	"x"
#define PRIxLEAST64	__PRI64_PREFIX "x"

#define PRIxFAST8	"x"
#define PRIxFAST16	"x"
#define PRIxFAST32	"x"
#define PRIxFAST64	__PRI64_PREFIX "x"

#define PRIxMAX		__PRI64_PREFIX "x"
#ifdef __riscv64
#define PRIxPTR		"lx"
#else
#define PRIxPTR		"x"
#endif

#define PRIX8		"X"
#define PRIX16		"X"
#define PRIX32		"X"
#define PRIX64		__PRI64_PREFIX "X"

#define PRIXLEAST8	"X"
#define PRIXLEAST16	"X"
#define PRIXLEAST32	"X"
#define PRIXLEAST64	__PRI64_PREFIX "X"

#define PRIXFAST8	"X"
#define PRIXFAST16	"X"
#define PRIXFAST32	"X"
#define PRIXFAST64	__PRI64_PREFIX "X"

#define PRIXMAX		__PRI64_PREFIX "X"
#ifdef __riscv64
#define PRIXPTR		"lX"
#else
#define PRIXPTR		"X"
#endif

/* fscanf macros for signed integers */
#define SCNd8		"hhd"
#define SCNd16		"hd"
#define SCNd32		"d"
#define SCNd64		__SCN64_PREFIX "d"

#define SCNdLEAST8	"hhd"
#define SCNdLEAST16	"hd"
#define SCNdLEAST32	"d"
#define SCNdLEAST64	__SCN64_PREFIX "d"

#define SCNdFAST8	"d"
#define SCNdFAST16	"d"
#define SCNdFAST32	"d"
#define SCNdFAST64	__SCN64_PREFIX "d"

#define SCNdMAX		__SCN64_PREFIX "d"
#ifdef __riscv64
#define SCNdPTR		"ld"
#else
#define SCNdPTR		"d"
#endif

#define SCNi8		"hhi"
#define SCNi16		"hi"
#define SCNi32		"i"
#define SCNi64		__SCN64_PREFIX "i"

#define SCNiLEAST8	"hhi"
#define SCNiLEAST16	"hi"
#define SCNiLEAST32	"i"
#define SCNiLEAST64	__SCN64_PREFIX "i"

#define SCNiFAST8	"i"
#define SCNiFAST16	"i"
#define SCNiFAST32	"i"
#define SCNiFAST64	__SCN64_PREFIX "i"

#define SCNiMAX		__SCN64_PREFIX "i"
#ifdef __riscv64
#define SCNiPTR		"li"
#else
#define SCNiPTR		"i"
#endif

/* fscanf macros for unsigned integers */
#define SCNo8		"hho"
#define SCNo16		"ho"
#define SCNo32		"o"
#define SCNo64		__SCN64_PREFIX "o"

#define SCNoLEAST8	"hho"
#define SCNoLEAST16	"ho"
#define SCNoLEAST32	"o"
#define SCNoLEAST64	__SCN64_PREFIX "o"

#define SCNoFAST8	"o"
#define SCNoFAST16	"o"
#define SCNoFAST32	"o"
#define SCNoFAST64	__SCN64_PREFIX "o"

#define SCNoMAX		__SCN64_PREFIX "o"
#ifdef __riscv64
#define SCNoPTR		"lo"
#else
#define SCNoPTR		"o"
#endif

#define SCNu8		"hhu"
#define SCNu16		"hu"
#define SCNu32		"u"
#define SCNu64		__SCN64_PREFIX "u"

#define SCNuLEAST8	"hhu"
#define SCNuLEAST16	"hu"
#define SCNuLEAST32	"u"
#define SCNuLEAST64	__SCN64_PREFIX "u"

#define SCNuFAST8	"u"
#define SCNuFAST16	"u"
#define SCNuFAST32	"u"
#define SCNuFAST64	__SCN64_PREFIX "u"

#define SCNuMAX		__SCN64_PREFIX "u"
#ifdef __riscv64
#define SCNuPTR		"lu"
#else
#define SCNuPTR		"u"
#endif

#define SCNx8		"hhx"
#define SCNx16		"hx"
#define SCNx32		"x"
#define SCNx64		__SCN64_PREFIX "x"

#define SCNxLEAST8	"hhx"
#define SCNxLEAST16	"hx"
#define SCNxLEAST32	"x"
#define SCNxLEAST64	__SCN64_PREFIX "x"

#define SCNxFAST8	"x"
#define SCNxFAST16	"x"
#define SCNxFAST32	"x"
#define SCNxFAST64	__SCN64_PREFIX "x"

#define SCNxMAX		__SCN64_PREFIX "x"
#ifdef __riscv64
#define SCNxPTR		"lx"
#else
#define SCNxPTR		"x"
#endif

#endif /* _RISCV_INT_FMTIO_H_ */
