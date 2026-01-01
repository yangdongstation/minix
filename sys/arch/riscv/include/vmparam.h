/*	$NetBSD$	*/

/*
 * RISC-V 64 virtual memory parameters for MINIX.
 */

#ifndef _RISCV_VMPARAM_H_
#define _RISCV_VMPARAM_H_

/*
 * Kernel memory layout.
 */
#define KERNBASE	0xffffffff80000000UL	/* kernel base virtual address */
#define KERNTEXTOFF	0xffffffff80200000UL	/* kernel text start */
#define KERNLOAD	0x80200000UL		/* kernel physical load address */

/*
 * User VM space constants.
 */
#define USRSTACK	0x0000003fffff0000UL	/* user stack top */
#define USRSTACKSIZE	(16*1024*1024)		/* 16MB user stack */

/*
 * Virtual memory constants.
 */
#ifndef NBPG
#define NBPG		4096			/* page size */
#endif
#ifndef PGSHIFT
#define PGSHIFT		12			/* LOG2(NBPG) */
#endif
#define PGOFSET		(NBPG - 1)		/* byte offset into page */
#define NPTEPG		(NBPG / sizeof(void *))

#ifndef PAGE_SHIFT
#define PAGE_SHIFT	PGSHIFT
#define PAGE_SIZE	NBPG
#define PAGE_MASK	(PAGE_SIZE - 1)
#endif

/*
 * Memory sizes.
 */
#define MAXTSIZ		(64*1024*1024)		/* max text size */
#define MAXDSIZ		(512*1024*1024)		/* max data size */
#ifndef MAXSSIZ
#define MAXSSIZ		(16*1024*1024)		/* max stack size */
#endif

#endif /* _RISCV_VMPARAM_H_ */
