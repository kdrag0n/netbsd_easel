/*	$NetBSD: res_state.c,v 1.3 2004/05/24 01:20:17 christos Exp $	*/

/*-
 * Copyright (c) 2004 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Christos Zoulas.
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

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: res_state.c,v 1.3 2004/05/24 01:20:17 christos Exp $");
#endif

#include <sys/types.h>
#include <sys/queue.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <stdlib.h>
#include <unistd.h>
#include <resolv.h>
#include <netdb.h>

#include "pthread.h"
#include "pthread_int.h"

#define	MY_LIST_HEAD(name, type)					\
struct name {								\
	union type *lh_first;	/* first element */			\
}

#define	MY_LIST_ENTRY(type)						\
struct {								\
	union type *le_next;	/* next element */			\
	union type **le_prev;	/* address of previous next element */	\
}

static MY_LIST_HEAD(, _res_st) res_list = LIST_HEAD_INITIALIZER(&res_list);

union _res_st {
	MY_LIST_ENTRY(_res_st)	st_list;
	struct __res_state	st_res;
};

static pthread_mutex_t res_mtx = PTHREAD_MUTEX_INITIALIZER;

res_state __res_state(void);
res_state __res_get_state(void);
void __res_put_state(res_state);

#ifdef RES_STATE_DEBUG
static void
res_state_debug(const char *msg, void *p)
{
	char buf[512];
	pthread_t self = pthread_self();
	int len = snprintf(buf, sizeof(buf), "%p: %s %p\n", self, msg, p);

	(void)write(STDOUT_FILENO, buf, (size_t)len);
}
#else
#define res_state_debug(a, b)
#endif


res_state
__res_get_state(void)
{
	res_state res;
	union _res_st *st;
	pthread_mutex_lock(&res_mtx);
	st = LIST_FIRST(&res_list);
	if (st != NULL) {
		res_state_debug("checkout from list", st);
		LIST_REMOVE(st, st_list);
		/* XXX: because we trash it with pointers from the list */
		/* we should probably put the pointers elsewhere */
		res = &st->st_res;
		res->options = 0;
	} else {
		res = (res_state)malloc(sizeof(union _res_st));
		if (res == NULL) {
			pthread_mutex_unlock(&res_mtx);
			h_errno = NETDB_INTERNAL;
			return NULL;
		}
		res->options = 0;
		res_state_debug("alloc new", res);
	}
	pthread_mutex_unlock(&res_mtx);
	if ((res->options & RES_INIT) == 0 && res_ninit(res) == -1) {
		h_errno = NETDB_INTERNAL;
		__res_put_state(res);
		return NULL;
	}
	return res;
}

void
/*ARGSUSED*/
__res_put_state(res_state res)
{
	pthread_mutex_lock(&res_mtx);
	res_state_debug("free", res);
	LIST_INSERT_HEAD(&res_list, (union _res_st *)(void *)res, st_list);
	pthread_mutex_unlock(&res_mtx);
}

/*
 * This is aliased via a macro to _res; don't allow multi-threaded programs
 * to use it.
 */
res_state
__res_state(void)
{
	static const char res[] = "_res is not supported for multi-threaded"
	    " programs.\n";
	(void)write(STDERR_FILENO, res, sizeof(res) - 1);
	abort();
	return NULL;
}
