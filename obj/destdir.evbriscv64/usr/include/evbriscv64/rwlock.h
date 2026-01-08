/*	$NetBSD$	*/

#ifndef _RISCV_RWLOCK_H_
#define	_RISCV_RWLOCK_H_

struct krwlock {
	volatile uintptr_t	rw_owner;
};

#ifdef __RWLOCK_PRIVATE

#define	__HAVE_SIMPLE_RW_LOCKS		1

#ifdef MULTIPROCESSOR
#define	RW_RECEIVE(rw)			__asm __volatile("fence r,rw" ::: "memory")
#define	RW_GIVE(rw)			__asm __volatile("fence rw,w" ::: "memory")
#else
#define	RW_RECEIVE(rw)			/* nothing */
#define	RW_GIVE(rw)			/* nothing */
#endif

#define	RW_CAS(p, o, n)						\
    (__sync_bool_compare_and_swap((volatile unsigned long *)(p), (o), (n)))

#endif	/* __RWLOCK_PRIVATE */

#endif /* _RISCV_RWLOCK_H_ */
