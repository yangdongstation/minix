/*	$NetBSD$	*/

#ifndef _RISCV_CPU_H_
#define _RISCV_CPU_H_

/*
 * User-visible CTL_MACHDEP definitions.
 */
#define	CPU_DEBUG		1	/* int: misc kernel debug control */
#define	CPU_BOOTED_DEVICE	2	/* string: device we booted from */
#define	CPU_BOOTED_KERNEL	3	/* string: kernel we booted */
#define	CPU_CONSDEV		4	/* struct: dev_t of our console */
#define	CPU_POWERSAVE		5	/* int: use CPU powersave mode */
#define	CPU_MAXID		6	/* number of valid machdep ids */

#endif /* _RISCV_CPU_H_ */
