/* $NetBSD: wsevent.c,v 1.30 2009/01/15 04:22:11 yamt Exp $ */

/*-
 * Copyright (c) 2006, 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Julio M. Merino Vidal.
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
 * Copyright (c) 1996, 1997 Christopher G. Demetriou.  All rights reserved.
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
 *      This product includes software developed by Christopher G. Demetriou
 *	for the NetBSD Project.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Lawrence Berkeley Laboratory.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)event.c	8.1 (Berkeley) 6/11/93
 */

/*
 * Internal "wscons_event" queue interface for the keyboard and mouse drivers.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: wsevent.c,v 1.30 2009/01/15 04:22:11 yamt Exp $");

#include "opt_compat_netbsd.h"

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/fcntl.h>
#include <sys/malloc.h>
#include <sys/proc.h>
#include <sys/systm.h>
#include <sys/vnode.h>
#include <sys/select.h>
#include <sys/poll.h>

#include <dev/wscons/wsconsio.h>
#include <dev/wscons/wseventvar.h>

/*
 * Size of a wsevent queue (measured in number of events).
 * Should be a power of two so that `%' is fast.
 * At the moment, the value below makes the queues use 2 Kbytes each; this
 * value may need tuning.
 */
#define	WSEVENT_QSIZE	256

/*
 * Priority of code managing wsevent queues.  PWSEVENT is set just above
 * PSOCK, which is just above TTIPRI, on the theory that mouse and keyboard
 * `user' input should be quick.
 */
#define	PWSEVENT	23
#define	splwsevent()	spltty()

static void	wsevent_intr(void *);

/*
 * Initialize a wscons_event queue.
 */
void
wsevent_init(struct wseventvar *ev, struct proc *p)
{

	if (ev->q != NULL) {
#ifdef DIAGNOSTIC
		printf("wsevent_init: already init\n");
#endif
		return;
	}
#if defined(COMPAT_50) || defined(MODULAR)
	ev->version = 0;
#endif /* defined(COMPAT_50) || defined(MODULAR) */
	ev->get = ev->put = 0;
	/*
	 * We allocate the maximum structure size here, so that we don't
	 * need to malloc/free again in setversion.
	 */
	ev->q = malloc(WSEVENT_QSIZE * sizeof(struct wscons_event),
	    M_DEVBUF, M_WAITOK|M_ZERO);
	selinit(&ev->sel);
	ev->io = p;
	ev->sih = softint_establish(SOFTINT_MPSAFE | SOFTINT_CLOCK,
	    wsevent_intr, ev);
}

/*
 * Tear down a wscons_event queue.
 */
void
wsevent_fini(struct wseventvar *ev)
{
	if (ev->q == NULL) {
#ifdef DIAGNOSTIC
		printf("wsevent_fini: already fini\n");
#endif
		return;
	}
	seldestroy(&ev->sel);
	free(ev->q, M_DEVBUF);
	ev->q = NULL;
	softint_disestablish(ev->sih);
}

/*
 * User-level interface: read, poll.
 * (User cannot write an event queue.)
 */
int
wsevent_read(struct wseventvar *ev, struct uio *uio, int flags)
{
	int s, n, cnt, error;

	/*
	 * Make sure we can return at least 1.
	 */
	if (uio->uio_resid < EVSIZE(ev))
		return (EMSGSIZE);	/* ??? */
	s = splwsevent();
	while (ev->get == ev->put) {
		if (flags & IO_NDELAY) {
			splx(s);
			return (EWOULDBLOCK);
		}
		ev->wanted = 1;
		error = tsleep(ev, PWSEVENT | PCATCH, "wsevent_read", 0);
		if (error) {
			splx(s);
			return (error);
		}
	}
	/*
	 * Move wscons_event from tail end of queue (there is at least one
	 * there).
	 */
	if (ev->put < ev->get)
		cnt = WSEVENT_QSIZE - ev->get;	/* events in [get..QSIZE) */
	else
		cnt = ev->put - ev->get;	/* events in [get..put) */
	splx(s);
	n = howmany(uio->uio_resid, EVSIZE(ev));
	if (cnt > n)
		cnt = n;
	error = uiomove(EVARRAY(ev, ev->get), cnt * EVSIZE(ev), uio);
	n -= cnt;
	/*
	 * If we do not wrap to 0, used up all our space, or had an error,
	 * stop.  Otherwise move from front of queue to put index, if there
	 * is anything there to move.
	 */
	if ((ev->get = (ev->get + cnt) % WSEVENT_QSIZE) != 0 ||
	    n == 0 || error || (cnt = ev->put) == 0)
		return (error);
	if (cnt > n)
		cnt = n;
	error = uiomove(EVARRAY(ev, 0), cnt * EVSIZE(ev), uio);
	ev->get = cnt;
	return (error);
}

int
wsevent_poll(struct wseventvar *ev, int events, struct lwp *l)
{
	int revents = 0;
	int s = splwsevent();

        if (events & (POLLIN | POLLRDNORM)) {
		if (ev->get != ev->put)
			revents |= events & (POLLIN | POLLRDNORM);
		else
			selrecord(l, &ev->sel);
	}

	splx(s);
	return (revents);
}

static void
filt_wseventrdetach(struct knote *kn)
{
	struct wseventvar *ev = kn->kn_hook;
	int s;

	s = splwsevent();
	SLIST_REMOVE(&ev->sel.sel_klist, kn, knote, kn_selnext);
	splx(s);
}

static int
filt_wseventread(struct knote *kn, long hint)
{
	struct wseventvar *ev = kn->kn_hook;

	if (ev->get == ev->put)
		return (0);

	if (ev->get < ev->put)
		kn->kn_data = ev->put - ev->get;
	else
		kn->kn_data = (WSEVENT_QSIZE - ev->get) + ev->put;

	kn->kn_data *= EVSIZE(ev);

	return (1);
}

static const struct filterops wsevent_filtops =
	{ 1, NULL, filt_wseventrdetach, filt_wseventread };

int
wsevent_kqfilter(struct wseventvar *ev, struct knote *kn)
{
	struct klist *klist;
	int s;

	switch (kn->kn_filter) {
	case EVFILT_READ:
		klist = &ev->sel.sel_klist;
		kn->kn_fop = &wsevent_filtops;
		break;

	default:
		return (EINVAL);
	}

	kn->kn_hook = ev;

	s = splwsevent();
	SLIST_INSERT_HEAD(klist, kn, kn_selnext);
	splx(s);

	return (0);
}

/*
 * Wakes up all listener of the 'ev' queue.
 */
void
wsevent_wakeup(struct wseventvar *ev)
{

	selnotify(&ev->sel, 0, 0);

	if (ev->wanted) {
		ev->wanted = 0;
		wakeup(ev);
	}

	if (ev->async) {
		softint_schedule(ev->sih);
	}
}

/*
 * Soft interrupt handler: sends signal to async proc.
 */
static void
wsevent_intr(void *cookie)
{
	struct wseventvar *ev;

	ev = cookie;

	if (ev->async) {
		mutex_enter(proc_lock);
		psignal(ev->io, SIGIO);
		mutex_exit(proc_lock);
	}
}

/*
 * Injects the set of events given in 'events', whose size is 'nevents',
 * into the 'ev' queue.  If there is not enough free space to inject them
 * all, returns ENOSPC and the queue is left intact; otherwise returns 0
 * and wakes up all listeners.
 */
int
wsevent_inject(struct wseventvar *ev, struct wscons_event *events,
    size_t nevents)
{
	size_t avail, i;
	struct timespec t;

	/* Calculate number of free slots in the queue. */
	if (ev->put < ev->get)
		avail = ev->get - ev->put;
	else
		avail = WSEVENT_QSIZE - (ev->put - ev->get);
	KASSERT(avail <= WSEVENT_QSIZE);

	/* Fail if there is all events will not fit in the queue. */
	if (avail < nevents)
		return ENOSPC;

	/* Use the current time for all events. */
	getnanotime(&t);

	/* Inject the events. */
	switch (ev->version) {
#if defined(COMPAT_50) || defined(MODULAR)
	case 0:
		for (i = 0; i < nevents; i++) {
			struct owscons_event *we;

			we = EVARRAY(ev, ev->put);
			we->type = events[i].type;
			we->value = events[i].value;
			timespec_to_timespec50(&t, &we->time);

			ev->put = (ev->put + 1) % WSEVENT_QSIZE;
		}
		break;
#endif /* defined(COMPAT_50) || defined(MODULAR) */
	case WSEVENT_VERSION:
		for (i = 0; i < nevents; i++) {
			struct wscons_event *we;

			we = EVARRAY(ev, ev->put);
			we->type = events[i].type;
			we->value = events[i].value;
			we->time = t;

			ev->put = (ev->put + 1) % WSEVENT_QSIZE;
		}
		break;

	default:
		return EINVAL;
	}

	wsevent_wakeup(ev);

	return 0;
}

int
wsevent_setversion(struct wseventvar *ev, int vers)
{
	if (ev == NULL)
		return EINVAL;

	switch (vers) {
	case 0:
	case WSEVENT_VERSION:
		break;
	default:
		return EINVAL;
	}

	if (vers == ev->version)
		return 0;

	ev->get = ev->put = 0;
	ev->version = vers;
	return 0;
}
