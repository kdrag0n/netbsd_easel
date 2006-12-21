/*	$NetBSD: interrupt.c,v 1.4 2006/12/21 15:55:23 yamt Exp $	*/

/*
 * Copyright (c) 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Wayne Knowles
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
__KERNEL_RCSID(0, "$NetBSD: interrupt.c,v 1.4 2006/12/21 15:55:23 yamt Exp $");

#include <sys/param.h>
#include <sys/malloc.h>

#include <uvm/uvm_extern.h>

#include <net/netisr.h>			/* Legacy softnet support */

#include <machine/intr.h>
#include <machine/sysconf.h>

struct mipsco_intr softintr_tab[SI_NQUEUES];

/* XXX For legacy software interrupts. */
struct mipsco_intrhand *softnet_intrhand;

u_int32_t ssir;

/*
 * softintr_init:
 *
 *	Initialize the software interrupt system.
 */
void
softintr_init()
{
	static const char *softintr_names[] = SI_QUEUENAMES;
	struct mipsco_intr *sip;
	int i;

	for (i = 0; i < SI_NQUEUES; i++) {
		sip = &softintr_tab[i];
		sip->intr_siq = i;
		LIST_INIT(&sip->intr_q);
		evcnt_attach_dynamic(&sip->ih_evcnt, EVCNT_TYPE_INTR,
		    NULL, "soft", softintr_names[i]);
	}

	/* XXX Establish legacy software interrupt handlers. */
	softnet_intrhand = softintr_establish(IPL_SOFTNET,
	    (void (*)(void *))netintr, NULL);

	KASSERT(softnet_intrhand != NULL);
}

/*
 * softintr_dispatch:
 *
 *	Process pending software interrupts.
 *
 *      Called at splsoft()
 */
void
softintr_dispatch()
{
	struct mipsco_intr *sip;
	struct mipsco_intrhand *sih;
	u_int32_t n, i, s;

	s = splhigh();
	n = ssir; ssir = 0;
	splx(s);
	sip = softintr_tab;
  	for (i = 0; i < SI_NQUEUES; sip++, i++) {
	    if ((n & (1 << i)) == 0)
		continue;
	    sip->ih_evcnt.ev_count++;

	    LIST_FOREACH(sih, &sip->intr_q, ih_q) {
		if (sih->ih_pending) {
		    uvmexp.softs++;
		    sih->ih_pending = 0;
		    (*sih->ih_fun)(sih->ih_arg);
		}
	    }
	}
}

static int
ipl2si(ipl_t ipl)
{
	int si;

	switch (ipl) {
	case IPL_SOFT:
		si = SI_SOFT;
		break;
	case IPL_SOFTCLOCK:
		si = SI_SOFTCLOCK;
		break;
	case IPL_SOFTNET:
		si = SI_SOFTNET;
		break;
	case IPL_SOFTSERIAL:
		si = SI_SOFTSERIAL;
		break;
	default:
		panic("ipl2si: %d", ipl);
	}
	return si;
}

/*
 * softintr_establish:		[interface]
 *
 *	Register a software interrupt handler.
 */
void *
softintr_establish(int ipl, void (*func)(void *), void *arg)
{
	struct mipsco_intr *sip;
	struct mipsco_intrhand *sih;
	int si;
	int s;

	si = ipl2si(ipl);
	sip = &softintr_tab[si];

	sih = malloc(sizeof(*sih), M_DEVBUF, M_NOWAIT);
	if (__predict_true(sih != NULL)) {
		sih->ih_fun = (void *)func;
		sih->ih_arg = arg;
		sih->ih_intrhead = sip;
		sih->ih_pending = 0;

		s = splsoft();
		LIST_INSERT_HEAD(&sip->intr_q, sih, ih_q);
		splx(s);
	}
	return (sih);
}

/*
 * softintr_disestablish:	[interface]
 *
 *	Unregister a software interrupt handler.
 */
void
softintr_disestablish(void *arg)
{
	struct mipsco_intrhand *ih = arg;
	int s;

	s = splsoft();
	LIST_REMOVE(ih, ih_q);
	splx(s);
	free(ih, M_DEVBUF);
}

void
cpu_intr(status, cause, pc, ipending)
	u_int32_t status;
	u_int32_t cause;
	u_int32_t pc;
	u_int32_t ipending;
{
	uvmexp.intrs++;

	/* device interrupts */
	(*platform.iointr)(status, cause, pc, ipending);

	/* software simulated interrupt */
	if ((ipending & MIPS_SOFT_INT_MASK_1)
		    || (ssir && (status & MIPS_SOFT_INT_MASK_1))) {
	    _clrsoftintr(MIPS_SOFT_INT_MASK_1);
	    softintr_dispatch();
	}
}

static const int ipl_sr_bits[] = {
	[IPL_NONE] = 0,
	[IPL_SOFTCLOCK] = MIPS_INT_MASK_SPL_SOFT0,
	[IPL_SOFTNET] = MIPS_INT_MASK_SPL_SOFT1,
	[IPL_BIO] = MIPS_INT_MASK_SPL1,
	[IPL_NET] = MIPS_INT_MASK_SPL0,
	[IPL_TTY] = MIPS_INT_MASK_SPL0,
	[IPL_VM] = MIPS_INT_MASK_SPL2,
	[IPL_CLOCK] = MIPS_INT_MASK_SPL2,
	[IPL_STATCLOCK] = MIPS_INT_MASK_SPL2,
	[IPL_HIGH] = MIPS_INT_MASK_SPL2,
};

ipl_cookie_t
makeiplcookie(ipl_t ipl)
{

	return (ipl_cookie_t){._sr = ipl_sr_bits[ipl]};
}
