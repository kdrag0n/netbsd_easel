/*	$NetBSD: pthread_int.h,v 1.65 2008/01/08 20:56:08 christos Exp $	*/

/*-
 * Copyright (c) 2001, 2002, 2003, 2006, 2007 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Nathan J. Williams and Andrew Doran.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * NOTE: when changing anything in this file, please ensure that
 * libpthread_dbg still compiles.
 */

#ifndef _LIB_PTHREAD_INT_H
#define _LIB_PTHREAD_INT_H

/* #define PTHREAD__DEBUG */
#define ERRORCHECK

#include "pthread_types.h"
#include "pthread_queue.h"
#include "pthread_md.h"

#include <sys/tree.h>

#include <lwp.h>
#include <signal.h>

#ifdef __GNUC__
#define	PTHREAD_HIDE	__attribute__ ((visibility("hidden")))
#else
#define	PTHREAD_HIDE	/* nothing */
#endif

#define PTHREAD_KEYS_MAX 	256
#define	PTHREAD__UNPARK_MAX	32

/*
 * The size of this structure needs to be no larger than struct
 * __pthread_cleanup_store, defined in pthread.h.
 */
struct pt_clean_t {
	PTQ_ENTRY(pt_clean_t)	ptc_next;
	void	(*ptc_cleanup)(void *);
	void	*ptc_arg;
};

/* Private data for pthread_attr_t */
struct pthread_attr_private {
	char ptap_name[PTHREAD_MAX_NAMELEN_NP];
	void *ptap_namearg;
	void *ptap_stackaddr;
	size_t ptap_stacksize;
	size_t ptap_guardsize;
};

struct pthread_lock_ops {
	void	(*plo_init)(__cpu_simple_lock_t *);
	int	(*plo_try)(__cpu_simple_lock_t *);
	void	(*plo_unlock)(__cpu_simple_lock_t *);
	void	(*plo_lock)(__cpu_simple_lock_t *);
};

struct	__pthread_st {
	pthread_t	pt_self;	/* Must be first. */
	unsigned int	pt_magic;	/* Magic number */
	int		pt_state;	/* running, blocked, etc. */
	pthread_mutex_t	pt_lock;	/* lock on state */
	int		pt_flags;	/* see PT_FLAG_* below */
	int		pt_cancel;	/* Deferred cancellation */
	int		pt_errno;	/* Thread-specific errno. */
	stack_t		pt_stack;	/* Our stack */
	void		*pt_exitval;	/* Read by pthread_join() */
	char		*pt_name;	/* Thread's name, set by the app. */
	int		pt_willpark;	/* About to park */
	lwpid_t		pt_unpark;	/* Unpark this when parking */
	void		*pt_unparkhint;	/* Hint for the above */
	struct pthread_lock_ops pt_lockops;/* Cached to avoid PIC overhead */
	pthread_mutex_t	*pt_droplock;	/* Drop this lock if cancelled */
	pthread_cond_t	pt_joiners;	/* Threads waiting to join. */

	/* Threads to defer waking, usually until pthread_mutex_unlock(). */
	lwpid_t		pt_waiters[PTHREAD__UNPARK_MAX];
	size_t		pt_nwaiters;

	/* Stack of cancellation cleanup handlers and their arguments */
	PTQ_HEAD(, pt_clean_t)	pt_cleanup_stack;

	/* LWP ID and entry on the list of all threads. */
	lwpid_t		pt_lid;
	RB_ENTRY(__pthread_st) pt_alltree;
	PTQ_ENTRY(__pthread_st) pt_allq;
	PTQ_ENTRY(__pthread_st)	pt_deadq;

	/*
	 * General synchronization data.  We try to align, as threads
	 * on other CPUs will access this data frequently.
	 */
	int		pt_dummy1 __aligned(128);
	struct lwpctl 	*pt_lwpctl;	/* Kernel/user comms area */
	volatile int	pt_blocking;	/* Blocking in userspace */
	volatile int	pt_rwlocked;	/* Handed rwlock successfully */
	volatile int	pt_sleeponq;	/* On a sleep queue */
	volatile int	pt_signalled;	/* Received pthread_cond_signal() */
	void * volatile	pt_sleepobj;	/* Object slept on */
	PTQ_ENTRY(__pthread_st) pt_sleep;
	void		(*pt_early)(void *);
	int		pt_dummy2 __aligned(128);

	/* Thread-specific data.  Large so it sits close to the end. */
	int		pt_havespecific;
	void		*pt_specific[PTHREAD_KEYS_MAX];

	/*
	 * Context for thread creation.  At the end as it's cached
	 * and then only ever passed to _lwp_create(). 
	 */
	ucontext_t	pt_uc;
};

/* Thread states */
#define PT_STATE_RUNNING	1
#define PT_STATE_ZOMBIE		5
#define PT_STATE_DEAD		6

/* Flag values */

#define PT_FLAG_DETACHED	0x0001
#define PT_FLAG_CS_DISABLED	0x0004	/* Cancellation disabled */
#define PT_FLAG_CS_ASYNC	0x0008  /* Cancellation is async */
#define PT_FLAG_CS_PENDING	0x0010
#define PT_FLAG_SCOPE_SYSTEM	0x0040
#define PT_FLAG_EXPLICIT_SCHED	0x0080
#define PT_FLAG_SUSPENDED	0x0100	/* In the suspended queue */

#define PT_MAGIC	0x11110001
#define PT_DEAD		0xDEAD0001

#define PT_ATTR_MAGIC	0x22220002
#define PT_ATTR_DEAD	0xDEAD0002

extern int	pthread__stacksize_lg;
extern size_t	pthread__stacksize;
extern vaddr_t	pthread__stackmask;
extern vaddr_t	pthread__threadmask;
extern int	pthread__nspins;
extern int	pthread__concurrency;
extern int 	pthread__osrev;
extern int 	pthread__unpark_max;

/* Flag to be used in a ucontext_t's uc_flags indicating that
 * the saved register state is "user" state only, not full
 * trap state.
 */
#define _UC_USER_BIT		30
#define _UC_USER		(1LU << _UC_USER_BIT)

/* Utility functions */
void	pthread__unpark_all(pthread_t, pthread_spin_t *, pthread_queue_t *)
			    PTHREAD_HIDE;
void	pthread__unpark(pthread_t, pthread_spin_t *, pthread_queue_t *,
			pthread_t) PTHREAD_HIDE;
int	pthread__park(pthread_t, pthread_spin_t *, pthread_queue_t *,
		      const struct timespec *, int, const void *)
		      PTHREAD_HIDE;

/* Internal locking primitives */
void	pthread__lockprim_init(void) PTHREAD_HIDE;
void	pthread_lockinit(pthread_spin_t *) PTHREAD_HIDE;

static inline void pthread__spinlock(pthread_t, pthread_spin_t *)
    __attribute__((__always_inline__));
static inline void
pthread__spinlock(pthread_t self, pthread_spin_t *lock)
{
	if (__predict_true((*self->pt_lockops.plo_try)(lock)))
		return;
	(*self->pt_lockops.plo_lock)(lock);
}

static inline int pthread__spintrylock(pthread_t, pthread_spin_t *)
    __attribute__((__always_inline__));
static inline int
pthread__spintrylock(pthread_t self, pthread_spin_t *lock)
{
	return (*self->pt_lockops.plo_try)(lock);
}

static inline void pthread__spinunlock(pthread_t, pthread_spin_t *)
    __attribute__((__always_inline__));
static inline void
pthread__spinunlock(pthread_t self, pthread_spin_t *lock)
{
	(*self->pt_lockops.plo_unlock)(lock);
}

extern const struct pthread_lock_ops *pthread__lock_ops;

int	pthread__simple_locked_p(__cpu_simple_lock_t *) PTHREAD_HIDE;
#define	pthread__simple_lock_init(alp)	(*pthread__lock_ops->plo_init)(alp)
#define	pthread__simple_lock_try(alp)	(*pthread__lock_ops->plo_try)(alp)
#define	pthread__simple_unlock(alp)	(*pthread__lock_ops->plo_unlock)(alp)

#ifndef _getcontext_u
int	_getcontext_u(ucontext_t *) PTHREAD_HIDE;
#endif
#ifndef _setcontext_u
int	_setcontext_u(const ucontext_t *) PTHREAD_HIDE;
#endif
#ifndef _swapcontext_u
int	_swapcontext_u(ucontext_t *, const ucontext_t *) PTHREAD_HIDE;
#endif

void	pthread__testcancel(pthread_t) PTHREAD_HIDE;
int	pthread__find(pthread_t) PTHREAD_HIDE;

#ifndef PTHREAD_MD_INIT
#define PTHREAD_MD_INIT
#endif

#ifndef _INITCONTEXT_U_MD
#define _INITCONTEXT_U_MD(ucp)
#endif

#define _INITCONTEXT_U(ucp) do {					\
	(ucp)->uc_flags = _UC_CPU | _UC_STACK;				\
	_INITCONTEXT_U_MD(ucp)						\
	} while (/*CONSTCOND*/0)

/* Stack location of pointer to a particular thread */
#define pthread__id(sp) \
	((pthread_t) (((vaddr_t)(sp)) & pthread__threadmask))

#ifdef PTHREAD__HAVE_THREADREG
#define	pthread__self()		pthread__threadreg_get()
#else
#define pthread__self() 	(pthread__id(pthread__sp()))
#endif

#define pthread__abort()						\
	pthread__assertfunc(__FILE__, __LINE__, __func__, "unreachable")

#define pthread__assert(e) do {						\
	if (__predict_false(!(e)))					\
       	       pthread__assertfunc(__FILE__, __LINE__, __func__, #e);	\
        } while (/*CONSTCOND*/0)

#define pthread__error(err, msg, e) do {				\
	if (__predict_false(!(e))) {					\
       	       pthread__errorfunc(__FILE__, __LINE__, __func__, msg);	\
	       return (err);						\
	} 								\
        } while (/*CONSTCOND*/0)

void	pthread__destroy_tsd(pthread_t) PTHREAD_HIDE;
void	pthread__assertfunc(const char *, int, const char *, const char *)
			    PTHREAD_HIDE;
void	pthread__errorfunc(const char *, int, const char *, const char *)
			   PTHREAD_HIDE;
char	*pthread__getenv(const char *) PTHREAD_HIDE;
void	pthread__cancelled(void) PTHREAD_HIDE;

void	*pthread__atomic_cas_ptr(volatile void *, const void *, const void *) PTHREAD_HIDE;
void	*pthread__atomic_swap_ptr(volatile void *, const void *) PTHREAD_HIDE;
void	pthread__atomic_or_ulong(volatile unsigned long *, unsigned long) PTHREAD_HIDE;
void	pthread__membar_full(void) PTHREAD_HIDE;
void	pthread__membar_producer(void) PTHREAD_HIDE;
void	pthread__membar_consumer(void) PTHREAD_HIDE;

int	pthread__mutex_deferwake(pthread_t, pthread_mutex_t *) PTHREAD_HIDE;

#ifndef pthread__smt_pause
#define	pthread__smt_pause()	/* nothing */
#endif

/*
 * Bits in the owner field of the lock that indicate lock state.  If the
 * WRITE_LOCKED bit is clear, then the owner field is actually a count of
 * the number of readers.
 */
#define	RW_HAS_WAITERS		0x01	/* lock has waiters */
#define	RW_WRITE_WANTED		0x02	/* >= 1 waiter is a writer */
#define	RW_WRITE_LOCKED		0x04	/* lock is currently write locked */
#define	RW_UNUSED		0x08	/* currently unused */

#define	RW_FLAGMASK		0x0f

#define	RW_READ_COUNT_SHIFT	4
#define	RW_READ_INCR		(1 << RW_READ_COUNT_SHIFT)
#define	RW_THREAD		((uintptr_t)-RW_READ_INCR)
#define	RW_OWNER(rw)		((rw)->rw_owner & RW_THREAD)
#define	RW_COUNT(rw)		((rw)->rw_owner & RW_THREAD)
#define	RW_FLAGS(rw)		((rw)->rw_owner & ~RW_THREAD)

#define	ptr_owner		ptr_writer

#endif /* _LIB_PTHREAD_INT_H */
