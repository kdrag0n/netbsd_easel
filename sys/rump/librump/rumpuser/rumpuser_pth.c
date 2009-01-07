/*	$NetBSD: rumpuser_pth.c,v 1.23 2009/01/07 22:50:08 pooka Exp $	*/

/*
 * Copyright (c) 2007 Antti Kantee.  All Rights Reserved.
 *
 * Development of this software was supported by the
 * Finnish Cultural Foundation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#if !defined(lint)
__RCSID("$NetBSD: rumpuser_pth.c,v 1.23 2009/01/07 22:50:08 pooka Exp $");
#endif /* !lint */

#ifdef __linux__
#define _XOPEN_SOURCE 500
#define _BSD_SOURCE
#define _FILE_OFFSET_BITS 64
#endif

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <rump/rumpuser.h>

#include "rumpuser_int.h"

static pthread_key_t curlwpkey;
static pthread_key_t isintr;

#define NOFAIL(a) do {if (!(a)) abort();} while (/*CONSTCOND*/0)
#define NOFAIL_ERRNO(a)							\
do {									\
	int fail_rv = (a);						\
	if (fail_rv) {							\
		printf("panic: rumpuser fatal failure %d (%s)\n",	\
		    fail_rv, strerror(fail_rv));			\
		    abort();						\
	}								\
} while (/*CONSTCOND*/0)

struct rumpuser_mtx {
	pthread_mutex_t pthmtx;
};

struct rumpuser_rw {
	pthread_rwlock_t pthrw;
};

struct rumpuser_cv {
	pthread_cond_t pthcv;
};

struct rumpuser_mtx rumpuser_aio_mtx;
struct rumpuser_cv rumpuser_aio_cv;
int rumpuser_aio_head, rumpuser_aio_tail;
struct rumpuser_aio *rumpuser_aios[N_AIOS];

struct rumpuser_rw rumpspl;

kernel_lockfn	rumpuser__klock;
kernel_unlockfn	rumpuser__kunlock;
int		rumpuser__wantthreads;

static void *
/*ARGSUSED*/
iothread(void *arg)
{
	struct rumpuser_aio *rua;
	rump_biodone_fn biodone = arg;

	NOFAIL_ERRNO(pthread_mutex_lock(&rumpuser_aio_mtx.pthmtx));
	for (;;) {
		while (rumpuser_aio_head == rumpuser_aio_tail) {
			NOFAIL_ERRNO(pthread_cond_wait(&rumpuser_aio_cv.pthcv,
			    &rumpuser_aio_mtx.pthmtx));
		}

		rua = rumpuser_aios[rumpuser_aio_tail];
		rumpuser_aio_tail = (rumpuser_aio_tail+1) % (N_AIOS-1);
		pthread_mutex_unlock(&rumpuser_aio_mtx.pthmtx);

		if (rua->rua_op)
			rumpuser_read_bio(rua->rua_fd, rua->rua_data,
			    rua->rua_dlen, rua->rua_off, biodone, rua->rua_bp);
		else
			rumpuser_write_bio(rua->rua_fd, rua->rua_data,
			    rua->rua_dlen, rua->rua_off, biodone, rua->rua_bp);

		free(rua);
		NOFAIL_ERRNO(pthread_mutex_lock(&rumpuser_aio_mtx.pthmtx));
	}
}

void
rumpuser_thrinit(kernel_lockfn lockfn, kernel_unlockfn unlockfn, int threads)
{

	pthread_mutex_init(&rumpuser_aio_mtx.pthmtx, NULL);
	pthread_cond_init(&rumpuser_aio_cv.pthcv, NULL);
	pthread_rwlock_init(&rumpspl.pthrw, NULL);

	pthread_key_create(&curlwpkey, NULL);
	pthread_key_create(&isintr, NULL);

	rumpuser__klock = lockfn;
	rumpuser__kunlock = unlockfn;
	rumpuser__wantthreads = threads;
}

int
rumpuser_bioinit(rump_biodone_fn biodone)
{
	pthread_t iothr;

	if (rumpuser__wantthreads)
		pthread_create(&iothr, NULL, iothread, biodone);

	return 0;
}

#if 0
void
rumpuser__thrdestroy()
{

	pthread_key_delete(curlwpkey);
}
#endif

int
rumpuser_thread_create(void *(*f)(void *), void *arg, const char *thrname)
{
	pthread_t ptid;
	int rv;

	rv = pthread_create(&ptid, NULL, f, arg);
#ifdef __NetBSD__
	if (rv == 0 && thrname)
		pthread_setname_np(ptid, thrname, NULL);
#endif

	return rv;
}

void
rumpuser_thread_exit()
{

	pthread_exit(NULL);
}

void
rumpuser_mutex_init(struct rumpuser_mtx **mtx)
{
	NOFAIL(*mtx = malloc(sizeof(struct rumpuser_mtx)));
	NOFAIL_ERRNO(pthread_mutex_init(&((*mtx)->pthmtx), NULL));
}

void
rumpuser_mutex_recursive_init(struct rumpuser_mtx **mtx)
{
	pthread_mutexattr_t mattr;

	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);

	NOFAIL(*mtx = malloc(sizeof(struct rumpuser_mtx)));
	NOFAIL_ERRNO(pthread_mutex_init(&((*mtx)->pthmtx), &mattr));

	pthread_mutexattr_destroy(&mattr);
}

void
rumpuser_mutex_enter(struct rumpuser_mtx *mtx)
{

	KLOCK_WRAP(NOFAIL_ERRNO(pthread_mutex_lock(&mtx->pthmtx)));
}

int
rumpuser_mutex_tryenter(struct rumpuser_mtx *mtx)
{

	return pthread_mutex_trylock(&mtx->pthmtx) == 0;
}

void
rumpuser_mutex_exit(struct rumpuser_mtx *mtx)
{

	NOFAIL_ERRNO(pthread_mutex_unlock(&mtx->pthmtx));
}

void
rumpuser_mutex_destroy(struct rumpuser_mtx *mtx)
{

	NOFAIL_ERRNO(pthread_mutex_destroy(&mtx->pthmtx));
	free(mtx);
}

int
rumpuser_mutex_held(struct rumpuser_mtx *mtx)
{

	return pthread_mutex_held_np(&mtx->pthmtx);
}

void
rumpuser_rw_init(struct rumpuser_rw **rw)
{

	NOFAIL(*rw = malloc(sizeof(struct rumpuser_rw)));
	NOFAIL_ERRNO(pthread_rwlock_init(&((*rw)->pthrw), NULL));
}

void
rumpuser_rw_enter(struct rumpuser_rw *rw, int write)
{

	if (write)
		KLOCK_WRAP(NOFAIL_ERRNO(pthread_rwlock_wrlock(&rw->pthrw)));
	else
		KLOCK_WRAP(NOFAIL_ERRNO(pthread_rwlock_rdlock(&rw->pthrw)));
}

int
rumpuser_rw_tryenter(struct rumpuser_rw *rw, int write)
{

	if (write)
		return pthread_rwlock_trywrlock(&rw->pthrw) == 0;
	else
		return pthread_rwlock_tryrdlock(&rw->pthrw) == 0;
}

void
rumpuser_rw_exit(struct rumpuser_rw *rw)
{

	NOFAIL_ERRNO(pthread_rwlock_unlock(&rw->pthrw));
}

void
rumpuser_rw_destroy(struct rumpuser_rw *rw)
{

	NOFAIL_ERRNO(pthread_rwlock_destroy(&rw->pthrw));
	free(rw);
}

int
rumpuser_rw_held(struct rumpuser_rw *rw)
{

	return pthread_rwlock_held_np(&rw->pthrw);
}

int
rumpuser_rw_rdheld(struct rumpuser_rw *rw)
{

	return pthread_rwlock_rdheld_np(&rw->pthrw);
}

int
rumpuser_rw_wrheld(struct rumpuser_rw *rw)
{

	return pthread_rwlock_wrheld_np(&rw->pthrw);
}

void
rumpuser_cv_init(struct rumpuser_cv **cv)
{

	NOFAIL(*cv = malloc(sizeof(struct rumpuser_cv)));
	NOFAIL_ERRNO(pthread_cond_init(&((*cv)->pthcv), NULL));
}

void
rumpuser_cv_destroy(struct rumpuser_cv *cv)
{

	NOFAIL_ERRNO(pthread_cond_destroy(&cv->pthcv));
	free(cv);
}

void
rumpuser_cv_wait(struct rumpuser_cv *cv, struct rumpuser_mtx *mtx)
{

	KLOCK_WRAP(NOFAIL_ERRNO(pthread_cond_wait(&cv->pthcv, &mtx->pthmtx)));
}

int
rumpuser_cv_timedwait(struct rumpuser_cv *cv, struct rumpuser_mtx *mtx,
	int stdticks)
{
	struct timespec ts;
	int rv;

	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec  += stdticks / 100;
	ts.tv_nsec += (stdticks % 100) * 10000000;
	ts.tv_sec  += ts.tv_nsec / 1000000000;
	ts.tv_nsec %= 1000000000;

	KLOCK_WRAP(rv = pthread_cond_timedwait(&cv->pthcv, &mtx->pthmtx, &ts));
	if (rv != 0 && rv != ETIMEDOUT)
		abort();

	if (rv == ETIMEDOUT)
		rv = EWOULDBLOCK;
	return rv;
}

void
rumpuser_cv_signal(struct rumpuser_cv *cv)
{

	NOFAIL_ERRNO(pthread_cond_signal(&cv->pthcv));
}

void
rumpuser_cv_broadcast(struct rumpuser_cv *cv)
{

	NOFAIL_ERRNO(pthread_cond_broadcast(&cv->pthcv));
}

int
rumpuser_cv_has_waiters(struct rumpuser_cv *cv)
{

	return pthread_cond_has_waiters_np(&cv->pthcv);
}

/*
 * curlwp
 */

void
rumpuser_set_curlwp(struct lwp *l)
{

	assert(pthread_getspecific(curlwpkey) == NULL || l == NULL);
	pthread_setspecific(curlwpkey, l);
}

struct lwp *
rumpuser_get_curlwp()
{

	return pthread_getspecific(curlwpkey);
}

/*
 * I am the interrupt
 */

void
rumpuser_set_ipl(int what)
{
	int cur;

	if (what == RUMPUSER_IPL_INTR) {
		pthread_setspecific(isintr, (void *)RUMPUSER_IPL_INTR);
	} else  {
		cur = (int)(intptr_t)pthread_getspecific(isintr);
		pthread_setspecific(isintr, (void *)(intptr_t)(cur+1));
	}
}

int
rumpuser_whatis_ipl()
{

	return (int)(intptr_t)pthread_getspecific(isintr);
}

void
rumpuser_clear_ipl(int what)
{
	int cur;

	if (what == RUMPUSER_IPL_INTR)
		cur = 1;
	else
		cur = (int)(intptr_t)pthread_getspecific(isintr);
	cur--;

	pthread_setspecific(isintr, (void *)(intptr_t)cur);
}
