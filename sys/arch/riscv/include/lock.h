/*	$NetBSD$	*/

#ifndef _RISCV_LOCK_H_
#define _RISCV_LOCK_H_

static __inline int
__SIMPLELOCK_LOCKED_P(__cpu_simple_lock_t *__ptr)
{
	return *__ptr == __SIMPLELOCK_LOCKED;
}

static __inline int
__SIMPLELOCK_UNLOCKED_P(__cpu_simple_lock_t *__ptr)
{
	return *__ptr == __SIMPLELOCK_UNLOCKED;
}

static __inline void
__cpu_simple_lock_clear(__cpu_simple_lock_t *__ptr)
{
	*__ptr = __SIMPLELOCK_UNLOCKED;
}

static __inline void
__cpu_simple_lock_set(__cpu_simple_lock_t *__ptr)
{
	*__ptr = __SIMPLELOCK_LOCKED;
}

#ifdef _KERNEL
#define	mb_read()	__asm__ volatile("fence r,r" ::: "memory")
#define	mb_write()	__asm__ volatile("fence w,w" ::: "memory")
#define	mb_memory()	__asm__ volatile("fence rw,rw" ::: "memory")
#endif

#endif /* _RISCV_LOCK_H_ */
