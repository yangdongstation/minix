/*	$NetBSD$	*/

#ifndef _RISCV_DISKLABEL_H_
#define _RISCV_DISKLABEL_H_

#define LABELUSESMBR		1	/* use MBR partitionning */
#define LABELSECTOR		1	/* sector containing label */
#define LABELOFFSET		0	/* offset of label in sector */
#define MAXPARTITIONS		16	/* number of partitions */
#define OLDMAXPARTITIONS	8	/* number of partitions before 1.6 */
#define RAW_PART		2	/* raw partition: XX?c */

/*
 * We use the highest bit of the minor number for the partition number.
 * This maintains backward compatibility with device nodes created before
 * MAXPARTITIONS was increased.
 */
#define __RISCV_MAXDISKS	((1 << 20) / MAXPARTITIONS)
#define DISKUNIT(dev)	((minor(dev) / OLDMAXPARTITIONS) % __RISCV_MAXDISKS)
#define DISKPART(dev)	((minor(dev) % OLDMAXPARTITIONS) + \
    ((minor(dev) / (__RISCV_MAXDISKS * OLDMAXPARTITIONS)) * OLDMAXPARTITIONS))
#define	DISKMINOR(unit, part) \
    (((unit) * OLDMAXPARTITIONS) + ((part) % OLDMAXPARTITIONS) + \
     ((part) / OLDMAXPARTITIONS) * (__RISCV_MAXDISKS * OLDMAXPARTITIONS))

/* Pull in MBR partition definitions. */
#if HAVE_NBTOOL_CONFIG_H
#include <nbinclude/sys/bootblock.h>
#else
#include <sys/bootblock.h>
#endif /* HAVE_NBTOOL_CONFIG_H */

#ifndef __ASSEMBLER__
#if HAVE_NBTOOL_CONFIG_H
#include <nbinclude/sys/dkbad.h>
#else
#include <sys/dkbad.h>
#endif /* HAVE_NBTOOL_CONFIG_H */
struct cpu_disklabel {
#define __HAVE_DISKLABEL_DKBAD
	struct dkbad bad;
};
#endif /* __ASSEMBLER__ */

#endif /* _RISCV_DISKLABEL_H_ */
