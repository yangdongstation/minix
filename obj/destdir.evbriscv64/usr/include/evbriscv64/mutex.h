/*	$NetBSD$	*/

#ifndef _RISCV_MUTEX_H_
#define	_RISCV_MUTEX_H_

struct kmutex {
	union {
		volatile uintptr_t	mtxa_owner;
#ifdef __MUTEX_PRIVATE
		struct {
			volatile uint8_t	mtxs_dummy;
			ipl_cookie_t		mtxs_ipl;
			__cpu_simple_lock_t	mtxs_lock;
			volatile uint8_t	mtxs_unused;
		} s;
#endif
	} u;
};

#ifdef __MUTEX_PRIVATE

#define	mtx_owner			u.mtxa_owner
#define	mtx_ipl				u.s.mtxs_ipl
#define	mtx_lock			u.s.mtxs_lock

#define	__HAVE_SIMPLE_MUTEXES		1

#ifdef MULTIPROCESSOR
#define	MUTEX_RECEIVE(mtx)		__asm __volatile("fence r,rw" ::: "memory")
#define	MUTEX_GIVE(mtx)			__asm __volatile("fence rw,w" ::: "memory")
#else
#define	MUTEX_RECEIVE(mtx)		/* nothing */
#define	MUTEX_GIVE(mtx)			/* nothing */
#endif

#define	MUTEX_CAS(p, o, n)					\
    (__sync_bool_compare_and_swap((volatile unsigned long *)(p), (o), (n)))

#endif	/* __MUTEX_PRIVATE */

#endif /* _RISCV_MUTEX_H_ */
