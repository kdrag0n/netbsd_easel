/* $NetBSD: intr.h,v 1.5 2006/12/21 15:55:23 yamt Exp $ */

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Tohru Nishimura.
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
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
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

#ifndef _MACHINE_INTR_H
#define _MACHINE_INTR_H

#ifdef _KERNEL

/*
 * spl functions; all but spl0 are done in-line
 */
#include <machine/psl.h>

#define splnone()	spl0()
#define spllowersoftclock()  spl1()
#define splsoftclock()  splraise1()
#define splsoftnet()    splraise1()
#define splbio()        spl2()
#define splnet()        spl3()
#define spltty()        spl6()
#define splclock()      spl5()
#define splstatclock()	spl5()
#define splvm()         spl7()
#define splhigh()       spl7()
#define splsched()      spl7()
#define spllock()	spl7()

/* watch out for side effects */
#define splx(s)         ((s) & PSL_IPL ? _spl(s) : spl0())

int spl0 __P((void));

#define	IPL_NONE	0
#define	IPL_SOFTCLOCK	1
#define	IPL_SOFTNET	2
#define	IPL_BIO		3
#define	IPL_NET		4
#define	IPL_CLOCK	5
#define	IPL_STATCLOCK	6
#define	IPL_TTY		7
#define	IPL_VM		8
#define	IPL_SCHED	9
#define	IPL_HIGH	10
#define	IPL_LOCK	11
#define	NIPLS		12

extern const int ipl2spl_table[NIPLS];

typedef int ipl_t;
typedef struct {
	int _spl;
} ipl_cookie_t;

static inline ipl_cookie_t
makeiplcookie(ipl_t ipl)
{

	return (ipl_cookie_t){._spl = ipl2spl_table[ipl]};
}

static inline int
splraiseipl(ipl_cookie_t icookie)
{

	return _splraise(icookie._spl);
}
#endif /* _KERNEL */

#endif	/* _MACHINE_INTR_H */
