/*	$NetBSD: event.c,v 1.5 2000/08/20 13:44:14 eeh Exp $	*/

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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
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
 * Internal `Firm_event' interface for the keyboard and mouse drivers.
 */
#include "opt_compat_netbsd32.h"

#include <sys/param.h>
#include <sys/fcntl.h>
#include <sys/malloc.h>
#include <sys/proc.h>
#include <sys/systm.h>
#include <sys/vnode.h>
#include <sys/select.h>
#include <sys/poll.h>

#include <machine/vuid_event.h>
#include <dev/sun/event_var.h>

#ifdef COMPAT_NETBSD32
int ev_out32 __P((struct firm_event *, int, struct uio *));

/*
 * Write out a series of 32-bit firm_events.
 */
int
ev_out32(e, n, uio)
	struct firm_event *e;
	int n;
	struct uio *uio;
{
	struct firm_event32 e32;
	int error = 0;

	while (n-- && error == 0) {
		e32.id = e->id;
		e32.value = e->value;
		e32.time.tv_sec = e->time.tv_sec;
		e32.time.tv_usec = e->time.tv_usec;
		error = uiomove((caddr_t)&e32, sizeof(e32), uio);
		e++;
	}
	return (error);
}
#endif

/*
 * Initialize a firm_event queue.
 */
void
ev_init(ev)
	struct evvar *ev;
{

	ev->ev_get = ev->ev_put = 0;
	ev->ev_q = malloc((u_long)EV_QSIZE * sizeof(struct firm_event),
	    M_DEVBUF, M_WAITOK);
	bzero((caddr_t)ev->ev_q, EV_QSIZE * sizeof(struct firm_event));
}

/*
 * Tear down a firm_event queue.
 */
void
ev_fini(ev)
	struct evvar *ev;
{

	free(ev->ev_q, M_DEVBUF);
}

/*
 * User-level interface: read, select.
 * (User cannot write an event queue.)
 */
int
ev_read(ev, uio, flags)
	struct evvar *ev;
	struct uio *uio;
	int flags;
{
	int s, n, cnt, error;

	/*
	 * Make sure we can return at least 1.
	 */
	if (uio->uio_resid < sizeof(struct firm_event))
		return (EMSGSIZE);	/* ??? */
	s = splev();
	while (ev->ev_get == ev->ev_put) {
		if (flags & IO_NDELAY) {
			splx(s);
			return (EWOULDBLOCK);
		}
		ev->ev_wanted = 1;
		error = tsleep((caddr_t)ev, PEVENT | PCATCH, "firm_event", 0);
		if (error) {
			splx(s);
			return (error);
		}
	}
	/*
	 * Move firm_events from tail end of queue (there is at least one
	 * there).
	 */
	if (ev->ev_put < ev->ev_get)
		cnt = EV_QSIZE - ev->ev_get;	/* events in [get..QSIZE) */
	else
		cnt = ev->ev_put - ev->ev_get;	/* events in [get..put) */
	splx(s);
	n = howmany(uio->uio_resid, sizeof(struct firm_event));
	if (cnt > n)
		cnt = n;
#ifdef COMPAT_NETBSD32
	if (curproc->p_flag & P_32) 
		error = ev_out32(&ev->ev_q[ev->ev_get], cnt, uio);
	else
#endif
	error = uiomove((caddr_t)&ev->ev_q[ev->ev_get],
	    cnt * sizeof(struct firm_event), uio);
	n -= cnt;
	/*
	 * If we do not wrap to 0, used up all our space, or had an error,
	 * stop.  Otherwise move from front of queue to put index, if there
	 * is anything there to move.
	 */
	if ((ev->ev_get = (ev->ev_get + cnt) % EV_QSIZE) != 0 ||
	    n == 0 || error || (cnt = ev->ev_put) == 0)
		return (error);
	if (cnt > n)
		cnt = n;
#ifdef COMPAT_NETBSD32
	if (curproc->p_flag & P_32) 
		error = ev_out32(&ev->ev_q[0], cnt, uio);
	else
#endif
	error = uiomove((caddr_t)&ev->ev_q[0],
	    cnt * sizeof(struct firm_event), uio);
	ev->ev_get = cnt;
	return (error);
}

int
ev_poll(ev, events, p)
	struct evvar *ev;
	int events;
	struct proc *p;
{
	int s = splev(), revents = 0;

	if (events & (POLLIN | POLLRDNORM)) {
		if (ev->ev_get == ev->ev_put)
			selrecord(p, &ev->ev_sel);
		else
			revents |= events & (POLLIN | POLLRDNORM);
	}
	revents |= events & (POLLOUT | POLLWRNORM);

	splx(s);
	return (revents);
}
